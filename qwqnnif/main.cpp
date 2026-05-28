#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace {

const int kBoardWidth = 25;
const int kBoardHeight = 15;
const int kMoveDelayMs = 110;

struct Point {
    int x;
    int y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

enum class Command {
    None,
    Up,
    Down,
    Left,
    Right,
    Pause,
    Restart,
    Quit
};

class TerminalSession {
public:
    TerminalSession() {
#ifdef _WIN32
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(output, &mode) != 0) {
                SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
        }
#else
        terminal_ready_ = tcgetattr(STDIN_FILENO, &original_terminal_) == 0;
        if (terminal_ready_) {
            termios raw = original_terminal_;
            raw.c_lflag &= static_cast<unsigned int>(~(ICANON | ECHO));
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
        }

        original_flags_ = fcntl(STDIN_FILENO, F_GETFL, 0);
        if (original_flags_ != -1) {
            fcntl(STDIN_FILENO, F_SETFL, original_flags_ | O_NONBLOCK);
        }
#endif
        std::cout << "\x1b[?25l";
    }

    ~TerminalSession() {
        std::cout << "\x1b[?25h\x1b[0m" << std::endl;
#ifndef _WIN32
        if (terminal_ready_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal_);
        }
        if (original_flags_ != -1) {
            fcntl(STDIN_FILENO, F_SETFL, original_flags_);
        }
#endif
    }

private:
#ifndef _WIN32
    termios original_terminal_{};
    bool terminal_ready_ = false;
    int original_flags_ = -1;
#endif
};

bool isOpposite(Direction a, Direction b) {
    return (a == Direction::Up && b == Direction::Down) ||
           (a == Direction::Down && b == Direction::Up) ||
           (a == Direction::Left && b == Direction::Right) ||
           (a == Direction::Right && b == Direction::Left);
}

Direction directionFromCommand(Command command) {
    switch (command) {
    case Command::Up:
        return Direction::Up;
    case Command::Down:
        return Direction::Down;
    case Command::Left:
        return Direction::Left;
    case Command::Right:
        return Direction::Right;
    default:
        return Direction::Right;
    }
}

Point moved(Point point, Direction direction) {
    switch (direction) {
    case Direction::Up:
        --point.y;
        break;
    case Direction::Down:
        ++point.y;
        break;
    case Direction::Left:
        --point.x;
        break;
    case Direction::Right:
        ++point.x;
        break;
    }
    return point;
}

Command readCommand() {
#ifdef _WIN32
    if (_kbhit() == 0) {
        return Command::None;
    }

    int key = _getch();
    if (key == 0 || key == 224) {
        key = _getch();
        switch (key) {
        case 72:
            return Command::Up;
        case 80:
            return Command::Down;
        case 75:
            return Command::Left;
        case 77:
            return Command::Right;
        default:
            return Command::None;
        }
    }
#else
    unsigned char key = 0;
    if (read(STDIN_FILENO, &key, 1) != 1) {
        return Command::None;
    }

    if (key == 27) {
        unsigned char sequence[2] = {0, 0};
        if (read(STDIN_FILENO, &sequence[0], 1) == 1 &&
            read(STDIN_FILENO, &sequence[1], 1) == 1 &&
            sequence[0] == '[') {
            switch (sequence[1]) {
            case 'A':
                return Command::Up;
            case 'B':
                return Command::Down;
            case 'C':
                return Command::Right;
            case 'D':
                return Command::Left;
            default:
                return Command::None;
            }
        }
        return Command::None;
    }
#endif

    switch (key) {
    case 'w':
    case 'W':
        return Command::Up;
    case 's':
    case 'S':
        return Command::Down;
    case 'a':
    case 'A':
        return Command::Left;
    case 'd':
    case 'D':
        return Command::Right;
    case 'p':
    case 'P':
        return Command::Pause;
    case 'r':
    case 'R':
        return Command::Restart;
    case 'q':
    case 'Q':
        return Command::Quit;
    default:
        return Command::None;
    }
}

class SnakeGame {
public:
    SnakeGame()
        : random_engine_(std::random_device{}()) {
        reset();
    }

    void reset() {
        snake_.clear();
        const int start_x = kBoardWidth / 2;
        const int start_y = kBoardHeight / 2;
        for (int i = 0; i < 4; ++i) {
            snake_.push_back({start_x - i, start_y});
        }

        direction_ = Direction::Right;
        next_direction_ = Direction::Right;
        score_ = 0;
        paused_ = false;
        game_over_ = false;
        won_ = false;
        placeFood();
    }

    void handleCommand(Command command) {
        if (command == Command::Restart) {
            reset();
            return;
        }

        if (command == Command::Pause && !game_over_) {
            paused_ = !paused_;
            return;
        }

        if (command == Command::Up || command == Command::Down ||
            command == Command::Left || command == Command::Right) {
            const Direction requested = directionFromCommand(command);
            if (!game_over_ && !paused_ && !isOpposite(requested, direction_)) {
                next_direction_ = requested;
            }
        }
    }

    void update() {
        if (paused_ || game_over_) {
            return;
        }

        direction_ = next_direction_;
        const Point next_head = moved(snake_.front(), direction_);
        const bool eats_food = next_head == food_;

        if (next_head.x < 0 || next_head.x >= kBoardWidth ||
            next_head.y < 0 || next_head.y >= kBoardHeight) {
            game_over_ = true;
            return;
        }

        const std::size_t body_to_check = eats_food ? snake_.size() : snake_.size() - 1;
        for (std::size_t i = 0; i < body_to_check; ++i) {
            if (snake_[i] == next_head) {
                game_over_ = true;
                return;
            }
        }

        snake_.push_front(next_head);
        if (eats_food) {
            score_ += 10;
            if (static_cast<int>(snake_.size()) == kBoardWidth * kBoardHeight) {
                game_over_ = true;
                won_ = true;
            } else {
                placeFood();
            }
        } else {
            snake_.pop_back();
        }
    }

    void render() const {
        std::vector<std::string> board(kBoardHeight, std::string(kBoardWidth, ' '));
        board[food_.y][food_.x] = '*';

        for (std::size_t i = 0; i < snake_.size(); ++i) {
            const Point segment = snake_[i];
            board[segment.y][segment.x] = (i == 0) ? 'O' : 'o';
        }

        std::cout << "\x1b[2J\x1b[H";
        std::cout << "Google Snake - Console Edition\n";
        std::cout << "Score: " << score_;
        if (paused_) {
            std::cout << "  [Paused]";
        }
        std::cout << "\n";
        std::cout << "Controls: Arrow keys/WASD move | P pause | R restart | Q quit\n\n";

        std::cout << '+';
        for (int x = 0; x < kBoardWidth; ++x) {
            std::cout << '-';
        }
        std::cout << "+\n";

        for (int y = 0; y < kBoardHeight; ++y) {
            std::cout << '|';
            std::cout << board[y];
            std::cout << "|\n";
        }

        std::cout << '+';
        for (int x = 0; x < kBoardWidth; ++x) {
            std::cout << '-';
        }
        std::cout << "+\n";

        if (game_over_) {
            if (won_) {
                std::cout << "\nYou filled the board! Press R to play again or Q to quit.\n";
            } else {
                std::cout << "\nGame over! Press R to restart or Q to quit.\n";
            }
        }

        std::cout.flush();
    }

private:
    bool isSnakeAt(Point point) const {
        return std::find(snake_.begin(), snake_.end(), point) != snake_.end();
    }

    void placeFood() {
        std::vector<Point> empty_cells;
        empty_cells.reserve(kBoardWidth * kBoardHeight - static_cast<int>(snake_.size()));

        for (int y = 0; y < kBoardHeight; ++y) {
            for (int x = 0; x < kBoardWidth; ++x) {
                Point point{x, y};
                if (!isSnakeAt(point)) {
                    empty_cells.push_back(point);
                }
            }
        }

        if (empty_cells.empty()) {
            return;
        }

        std::uniform_int_distribution<std::size_t> distribution(0, empty_cells.size() - 1);
        food_ = empty_cells[distribution(random_engine_)];
    }

    std::deque<Point> snake_;
    Point food_{0, 0};
    Direction direction_ = Direction::Right;
    Direction next_direction_ = Direction::Right;
    int score_ = 0;
    bool paused_ = false;
    bool game_over_ = false;
    bool won_ = false;
    std::mt19937 random_engine_;
};

} // namespace

int main() {
    TerminalSession terminal;
    SnakeGame game;
    game.render();

    bool running = true;
    auto next_move = std::chrono::steady_clock::now() + std::chrono::milliseconds(kMoveDelayMs);

    while (running) {
        const Command command = readCommand();
        if (command == Command::Quit) {
            running = false;
            continue;
        }

        if (command != Command::None) {
            game.handleCommand(command);
            game.render();
            next_move = std::chrono::steady_clock::now() + std::chrono::milliseconds(kMoveDelayMs);
        }

        const auto now = std::chrono::steady_clock::now();
        if (now >= next_move) {
            game.update();
            game.render();
            next_move = now + std::chrono::milliseconds(kMoveDelayMs);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }

    return 0;
}
