"""
Create starter UMG Widget Blueprint assets for a one-on-one Unreal game.

Run inside Unreal Editor with Python enabled:

    exec(open(r"/absolute/path/to/tools/unreal/create_1v1_ui_blueprints.py").read())

You can also run this file with normal Python and `--print-spec` to inspect the
UI asset plan without requiring Unreal's Python module.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from typing import Any

try:
    import unreal  # type: ignore
except ImportError:  # Allows syntax checks and spec printing outside Unreal.
    unreal = None  # type: ignore


PACKAGE_PATH = "/Game/UI/OneVsOne"


@dataclass(frozen=True)
class WidgetSpec:
    asset_name: str
    purpose: str
    root: str
    widgets: tuple[str, ...]
    variables: tuple[str, ...]
    dispatchers: tuple[str, ...]


WIDGET_SPECS: tuple[WidgetSpec, ...] = (
    WidgetSpec(
        asset_name="WBP_1v1_MainMenu",
        purpose="Start screen, match setup, and quit flow.",
        root="CanvasPanel",
        widgets=(
            "TitleText: TextBlock",
            "ModeLabelText: TextBlock",
            "StartButton: Button",
            "PracticeButton: Button",
            "QuitButton: Button",
            "VersionText: TextBlock",
        ),
        variables=(),
        dispatchers=(
            "OnStartMatchRequested",
            "OnPracticeRequested",
            "OnQuitRequested",
        ),
    ),
    WidgetSpec(
        asset_name="WBP_1v1_HUD",
        purpose="In-match player status, score, timer, prompts, and pause overlay.",
        root="CanvasPanel",
        widgets=(
            "P1NameText: TextBlock",
            "P1HealthBar: ProgressBar",
            "P1MeterBar: ProgressBar",
            "P1ScoreText: TextBlock",
            "RoundText: TextBlock",
            "TimerText: TextBlock",
            "RoundStateText: TextBlock",
            "P2NameText: TextBlock",
            "P2HealthBar: ProgressBar",
            "P2MeterBar: ProgressBar",
            "P2ScoreText: TextBlock",
            "CountdownText: TextBlock",
            "ObjectiveText: TextBlock",
            "InputHintText: TextBlock",
        ),
        variables=(
            "P1DisplayName: Text",
            "P2DisplayName: Text",
            "P1HealthPercent: Float",
            "P2HealthPercent: Float",
            "P1MeterPercent: Float",
            "P2MeterPercent: Float",
            "P1RoundScore: Integer",
            "P2RoundScore: Integer",
            "RoundNumber: Integer",
            "RemainingTimeSeconds: Integer",
            "RoundState: E_1v1RoundState",
        ),
        dispatchers=(),
    ),
    WidgetSpec(
        asset_name="WBP_1v1_RoundResult",
        purpose="Win/loss/draw result panel and rematch flow.",
        root="CanvasPanel",
        widgets=(
            "Backdrop: Border",
            "ResultText: TextBlock",
            "ScoreSummaryText: TextBlock",
            "RematchButton: Button",
            "MainMenuButton: Button",
        ),
        variables=(
            "WinnerPlayerIndex: Integer",
            "P1FinalScore: Integer",
            "P2FinalScore: Integer",
        ),
        dispatchers=(
            "OnRematchRequested",
            "OnReturnToMenuRequested",
        ),
    ),
    WidgetSpec(
        asset_name="WBP_1v1_PauseMenu",
        purpose="Resume, restart, and return-to-menu controls.",
        root="CanvasPanel",
        widgets=(
            "Backdrop: Border",
            "TitleText: TextBlock",
            "ResumeButton: Button",
            "RestartButton: Button",
            "MainMenuButton: Button",
        ),
        variables=(),
        dispatchers=(
            "OnResumeRequested",
            "OnRestartRequested",
            "OnReturnToMenuRequested",
        ),
    ),
)


DEFAULT_TEXT = {
    "TitleText": "1V1 ARENA",
    "ModeLabelText": "Local Duel",
    "VersionText": "Prototype UI",
    "RoundText": "ROUND 1",
    "TimerText": "99",
    "RoundStateText": "Waiting for players",
    "P1NameText": "PLAYER 1",
    "P2NameText": "PLAYER 2",
    "P1ScoreText": "0",
    "P2ScoreText": "0",
    "CountdownText": "FIGHT",
    "ObjectiveText": "Defeat your opponent",
    "InputHintText": "Esc: Pause",
    "ResultText": "PLAYER 1 WINS",
    "ScoreSummaryText": "0 - 0",
    "TitleText_Pause": "PAUSED",
}


BUTTON_TEXT = {
    "StartButton": "Start Match",
    "PracticeButton": "Practice",
    "QuitButton": "Quit",
    "RematchButton": "Rematch",
    "MainMenuButton": "Main Menu",
    "ResumeButton": "Resume",
    "RestartButton": "Restart",
}


def log(message: str) -> None:
    if unreal is not None:
        unreal.log(message)
    else:
        print(message)


def warn(message: str) -> None:
    if unreal is not None:
        unreal.log_warning(message)
    else:
        print(f"WARNING: {message}")


def print_spec() -> None:
    log(f"Package path: {PACKAGE_PATH}")
    for spec in WIDGET_SPECS:
        log("")
        log(f"{spec.asset_name}: {spec.purpose}")
        log(f"  Root: {spec.root}")
        log("  Widgets:")
        for widget in spec.widgets:
            log(f"    - {widget}")
        if spec.variables:
            log("  Variables:")
            for variable in spec.variables:
                log(f"    - {variable}")
        if spec.dispatchers:
            log("  Dispatchers:")
            for dispatcher in spec.dispatchers:
                log(f"    - {dispatcher}")


def require_unreal() -> Any:
    if unreal is None:
        raise RuntimeError("This action must be run inside Unreal Editor Python.")
    return unreal


def set_property_if_available(obj: Any, property_name: str, value: Any) -> bool:
    try:
        obj.set_editor_property(property_name, value)
        return True
    except Exception:
        return False


def create_text(value: str) -> Any:
    ue = require_unreal()
    try:
        return ue.Text(value)
    except Exception:
        return value


def ensure_package_path() -> None:
    ue = require_unreal()
    if not ue.EditorAssetLibrary.does_directory_exist(PACKAGE_PATH):
        ue.EditorAssetLibrary.make_directory(PACKAGE_PATH)
        log(f"Created {PACKAGE_PATH}")


def set_widget_text(widget: Any, name: str, fallback_text: str | None = None) -> None:
    text_value = fallback_text or DEFAULT_TEXT.get(name) or BUTTON_TEXT.get(name)
    if not text_value:
        return

    for property_name in ("text", "Text"):
        if set_property_if_available(widget, property_name, create_text(text_value)):
            return

    if hasattr(widget, "set_text"):
        try:
            widget.set_text(create_text(text_value))
        except Exception:
            pass


def create_widget(widget_tree: Any, class_name: str, widget_name: str) -> Any | None:
    ue = require_unreal()
    widget_class = getattr(ue, class_name, None)
    if widget_class is None:
        warn(f"Unreal Python class {class_name} is not available.")
        return None

    try:
        return widget_tree.construct_widget(widget_class, widget_name)
    except Exception as exc:
        warn(f"Could not construct {widget_name} ({class_name}): {exc}")
        return None


def add_child(parent: Any, child: Any) -> None:
    if hasattr(parent, "add_child"):
        parent.add_child(child)
        return
    warn(f"Could not add {child.get_name()} to {parent.get_name()}.")


def create_button_with_label(widget_tree: Any, button_name: str, label_text: str) -> Any | None:
    button = create_widget(widget_tree, "Button", button_name)
    if button is None:
        return None

    label = create_widget(widget_tree, "TextBlock", f"{button_name}_Label")
    if label is not None:
        set_widget_text(label, f"{button_name}_Label", label_text)
        add_child(button, label)

    return button


def build_simple_widget_tree(blueprint: Any, spec: WidgetSpec) -> None:
    widget_tree = getattr(blueprint, "widget_tree", None)
    if widget_tree is None:
        warn(
            f"{spec.asset_name} was created, but this Unreal version did not expose "
            "WidgetBlueprint.widget_tree to Python. Build the hierarchy from the docs."
        )
        return

    root = create_widget(widget_tree, spec.root, "RootCanvas")
    if root is None:
        return

    try:
        widget_tree.root_widget = root
    except Exception:
        set_property_if_available(widget_tree, "root_widget", root)

    for widget_entry in spec.widgets:
        widget_name, widget_type = [part.strip() for part in widget_entry.split(":", 1)]

        if widget_type == "Button":
            child = create_button_with_label(
                widget_tree,
                widget_name,
                BUTTON_TEXT.get(widget_name, widget_name.replace("Button", "")),
            )
        else:
            child = create_widget(widget_tree, widget_type, widget_name)
            if widget_type == "TextBlock" and child is not None:
                fallback = DEFAULT_TEXT.get("TitleText_Pause") if spec.asset_name.endswith("PauseMenu") and widget_name == "TitleText" else None
                set_widget_text(child, widget_name, fallback)

        if child is not None:
            add_child(root, child)


def create_widget_blueprint(spec: WidgetSpec) -> Any:
    ue = require_unreal()
    asset_path = f"{PACKAGE_PATH}/{spec.asset_name}"

    if ue.EditorAssetLibrary.does_asset_exist(asset_path):
        log(f"Keeping existing asset: {asset_path}")
        return ue.EditorAssetLibrary.load_asset(asset_path)

    asset_tools = ue.AssetToolsHelpers.get_asset_tools()
    factory = ue.WidgetBlueprintFactory()

    user_widget_class = getattr(ue, "UserWidget", None)
    if user_widget_class is not None:
        set_property_if_available(factory, "parent_class", user_widget_class)
        set_property_if_available(factory, "ParentClass", user_widget_class)

    blueprint_class = getattr(ue, "WidgetBlueprint", None)
    blueprint = asset_tools.create_asset(
        spec.asset_name,
        PACKAGE_PATH,
        blueprint_class,
        factory,
    )

    if blueprint is None:
        raise RuntimeError(f"Failed to create {asset_path}")

    log(f"Created {asset_path}")
    build_simple_widget_tree(blueprint, spec)
    return blueprint


def save_and_compile(blueprint: Any) -> None:
    ue = require_unreal()

    kismet_utils = getattr(ue, "KismetEditorUtilities", None)
    if kismet_utils is not None and hasattr(kismet_utils, "compile_blueprint"):
        try:
            kismet_utils.compile_blueprint(blueprint)
        except Exception as exc:
            warn(f"Could not compile {blueprint.get_name()}: {exc}")

    try:
        ue.EditorAssetLibrary.save_loaded_asset(blueprint)
    except Exception as exc:
        warn(f"Could not save {blueprint.get_name()}: {exc}")


def create_assets() -> None:
    require_unreal()
    ensure_package_path()

    created_assets = []
    for spec in WIDGET_SPECS:
        created_assets.append(create_widget_blueprint(spec))

    for asset in created_assets:
        save_and_compile(asset)

    log("")
    log("1v1 UI Widget Blueprint skeletons are ready.")
    log("Next manual Blueprint steps:")
    log("  1. Mark listed widgets as variables where gameplay needs direct updates.")
    log("  2. Add the event dispatchers from docs/unreal_1v1_blueprint_ui.md.")
    log("  3. Implement ApplyPlayerState, ApplyMatchState, ShowCountdown, and pause flow.")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--print-spec",
        action="store_true",
        help="Print the planned UI assets without requiring Unreal Editor.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.print_spec:
        print_spec()
        return

    create_assets()


if __name__ == "__main__":
    main()
