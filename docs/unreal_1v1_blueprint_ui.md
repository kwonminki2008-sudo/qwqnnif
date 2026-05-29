# Unreal 1v1 Game Basic Blueprint UI

This guide defines a small UMG/Blueprint interface set for a one-on-one game.
The repository does not currently contain an Unreal `.uproject` or binary
`.uasset` files, so the UI is described as reproducible Blueprint setup plus an
editor helper script in `tools/unreal/create_1v1_ui_blueprints.py`.

## Target assets

Create these assets under `/Game/UI/OneVsOne`:

| Asset | Type | Purpose |
| --- | --- | --- |
| `WBP_1v1_MainMenu` | Widget Blueprint | Start screen, match setup, quit button |
| `WBP_1v1_HUD` | Widget Blueprint | In-match player status, score, timer, prompts |
| `WBP_1v1_RoundResult` | Widget Blueprint | Win/loss/draw result panel and rematch flow |
| `WBP_1v1_PauseMenu` | Widget Blueprint | Resume, restart, return-to-menu controls |

Optional Blueprint support assets:

| Asset | Type | Purpose |
| --- | --- | --- |
| `BPI_1v1_UIProvider` | Blueprint Interface | Lets GameMode/GameState/PlayerController provide UI data |
| `E_1v1RoundState` | Blueprint Enum | `Waiting`, `Countdown`, `Playing`, `SuddenDeath`, `Finished` |
| `S_1v1PlayerUIState` | Blueprint Struct | Display data for one player panel |

## Global style

- Canvas target: 1920 x 1080.
- Safe zone padding: 48 px.
- Font scale: large enough for split attention during combat.
- Color roles:
  - Player 1: blue/cyan accent.
  - Player 2: orange/red accent.
  - Neutral text: white or light gray.
  - Warning/low-health: red pulse animation.
- Use named variables for every dynamic text/progress widget so gameplay
  Blueprints can update them without relying on widget hierarchy names.

## `WBP_1v1_MainMenu`

### Layout

Root: `CanvasPanel`

- `TitleText` (`TextBlock`)
  - Anchor: top center.
  - Text: `1V1 ARENA`.
- `ModeLabelText` (`TextBlock`)
  - Text: `Local Duel`.
- `StartButton` (`Button`)
  - Child text: `Start Match`.
- `PracticeButton` (`Button`)
  - Child text: `Practice`.
- `QuitButton` (`Button`)
  - Child text: `Quit`.
- `VersionText` (`TextBlock`)
  - Anchor: bottom right.

### Blueprint events

Add these event dispatchers:

- `OnStartMatchRequested`
- `OnPracticeRequested`
- `OnQuitRequested`

Button bindings:

- `StartButton.OnClicked` -> `OnStartMatchRequested.Broadcast`
- `PracticeButton.OnClicked` -> `OnPracticeRequested.Broadcast`
- `QuitButton.OnClicked` -> `OnQuitRequested.Broadcast`

## `WBP_1v1_HUD`

### Layout

Root: `CanvasPanel`

Top left player panel:

- `P1NameText` (`TextBlock`)
- `P1HealthBar` (`ProgressBar`)
- `P1MeterBar` (`ProgressBar`)
- `P1ScoreText` (`TextBlock`)

Top center match panel:

- `RoundText` (`TextBlock`) - e.g. `ROUND 1`
- `TimerText` (`TextBlock`) - e.g. `99`
- `RoundStateText` (`TextBlock`) - hidden during normal play.

Top right player panel:

- `P2NameText` (`TextBlock`)
- `P2HealthBar` (`ProgressBar`)
- `P2MeterBar` (`ProgressBar`)
- `P2ScoreText` (`TextBlock`)

Center prompts:

- `CountdownText` (`TextBlock`) - `3`, `2`, `1`, `FIGHT`.
- `ObjectiveText` (`TextBlock`) - short contextual message.

Bottom center input hint:

- `InputHintText` (`TextBlock`) - e.g. `Esc: Pause`.

Pause overlay:

- Add `WBP_1v1_PauseMenu` as a child widget.
- Default visibility: `Collapsed`.

### Variables

Expose these as widget variables:

| Name | Type | Example |
| --- | --- | --- |
| `P1DisplayName` | `Text` | `Player 1` |
| `P2DisplayName` | `Text` | `Player 2` |
| `P1HealthPercent` | `Float` | `0.75` |
| `P2HealthPercent` | `Float` | `0.42` |
| `P1MeterPercent` | `Float` | `0.30` |
| `P2MeterPercent` | `Float` | `1.00` |
| `P1RoundScore` | `Integer` | `1` |
| `P2RoundScore` | `Integer` | `0` |
| `RoundNumber` | `Integer` | `2` |
| `RemainingTimeSeconds` | `Integer` | `63` |
| `RoundState` | `E_1v1RoundState` | `Playing` |

### Blueprint functions

Create these functions in the widget graph:

#### `ApplyPlayerState(PlayerIndex, DisplayName, HealthPercent, MeterPercent, RoundScore)`

1. Branch on `PlayerIndex`.
2. Set matching name text.
3. Clamp health and meter values from `0.0` to `1.0`.
4. Set progress bar percentages.
5. Set score text.
6. If health is below `0.25`, play the low-health pulse animation for that
   player panel.

#### `ApplyMatchState(RoundNumber, RemainingTimeSeconds, RoundState)`

1. Set `RoundText` to `ROUND {RoundNumber}`.
2. Set `TimerText` to the remaining seconds.
3. Switch on `RoundState`:
   - `Waiting`: show `Waiting for players`.
   - `Countdown`: show `Get ready`.
   - `Playing`: hide `RoundStateText`.
   - `SuddenDeath`: show `Sudden Death`.
   - `Finished`: show `Round Finished`.

#### `ShowCountdown(Value)`

1. Set `CountdownText` to `Value`.
2. Set visibility to `Visible`.
3. Play a short scale/fade animation.
4. Hide when `Value` is empty.

#### `SetPauseMenuVisible(IsVisible)`

1. Set pause menu visibility to `Visible` or `Collapsed`.
2. Set input mode in the PlayerController:
   - Pause open: `Set Input Mode Game and UI`, show mouse cursor.
   - Pause closed: `Set Input Mode Game Only`, hide mouse cursor.

## `WBP_1v1_RoundResult`

### Layout

Root: `CanvasPanel`

- `Backdrop` (`Border`)
  - Semi-transparent black.
- `ResultText` (`TextBlock`)
  - `PLAYER 1 WINS`, `PLAYER 2 WINS`, or `DRAW`.
- `ScoreSummaryText` (`TextBlock`)
  - e.g. `2 - 1`.
- `RematchButton` (`Button`)
- `MainMenuButton` (`Button`)

### Blueprint events

- `OnRematchRequested`
- `OnReturnToMenuRequested`

Button bindings:

- `RematchButton.OnClicked` -> `OnRematchRequested.Broadcast`
- `MainMenuButton.OnClicked` -> `OnReturnToMenuRequested.Broadcast`

## `WBP_1v1_PauseMenu`

### Layout

Root: `CanvasPanel`

- `Backdrop` (`Border`)
  - Fill screen with a dark translucent color.
- `TitleText` (`TextBlock`) - `PAUSED`.
- `ResumeButton` (`Button`)
- `RestartButton` (`Button`)
- `MainMenuButton` (`Button`)

### Blueprint events

- `OnResumeRequested`
- `OnRestartRequested`
- `OnReturnToMenuRequested`

## Game Blueprint integration

Recommended ownership:

1. `BP_1v1PlayerController`
   - On `BeginPlay`, create `WBP_1v1_HUD` and add it to viewport.
   - Owns pause input and calls `SetPauseMenuVisible`.
2. `BP_1v1GameState`
   - Stores replicated match data: round, timer, player scores, round state.
   - Emits update events when values change.
3. `BP_1v1PlayerState`
   - Stores player display name, health percent, meter percent, and score.
4. `BP_1v1GameMode`
   - Starts countdown, begins rounds, resolves win/draw, opens result widget.

Basic update flow:

```text
GameMode starts countdown
  -> GameState.RoundState = Countdown
  -> PlayerController.HUD.ShowCountdown("3")
  -> PlayerController.HUD.ShowCountdown("2")
  -> PlayerController.HUD.ShowCountdown("1")
  -> PlayerController.HUD.ShowCountdown("FIGHT")
  -> GameState.RoundState = Playing

Any health/meter/score change
  -> PlayerState updates replicated values
  -> PlayerController reads PlayerStates
  -> HUD.ApplyPlayerState for Player 1 and Player 2

Round end
  -> GameMode determines winner
  -> PlayerController creates WBP_1v1_RoundResult
  -> Result widget dispatches rematch or return-to-menu request
```

## Quick Blueprint checklist

- [ ] Enable UMG if the project template did not include it.
- [ ] Create `/Game/UI/OneVsOne`.
- [ ] Run `tools/unreal/create_1v1_ui_blueprints.py` in Unreal Editor, or create
      the four Widget Blueprints manually.
- [ ] Add the variables and event dispatchers listed above.
- [ ] Hook `BP_1v1PlayerController.BeginPlay` to create and cache the HUD.
- [ ] Add pause input action and route it to `SetPauseMenuVisible`.
- [ ] Drive HUD updates from replicated GameState/PlayerState values.
- [ ] Test local two-player flow: menu -> countdown -> playing -> pause -> result.
