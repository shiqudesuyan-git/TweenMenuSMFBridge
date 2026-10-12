# Tween Menu - SKSE Menu Framework Bridge

Adds **SKSE MOD SETTINGS** to Tween Menu Overhaul. Selecting it closes Tween Menu and opens SKSE Menu Framework's main panel.

## AI disclosure

This mod was created with AI. OpenAI Codex wrote the bridge code and prepared the build and documentation, with direction and feedback from the uploader. Third-party SDKs and libraries remain the work of their respective authors. Bug reports and feedback are welcome.

## Requirements

- Skyrim SE/AE on Windows x64, SKSE64 and Address Library matching the runtime.
- SkyUI and Tween Menu Overhaul, including their requirements.
- SKSE Menu Framework providing the public `GetMainWindow` interface. Development used the 3.x source and API.

## Installation

Download the [ready-to-install NG archive](downloads/TweenMenuSMFBridge-1.0.0-NG.zip?raw=true). The [source archive](downloads/TweenMenuSMFBridge-1.0.0-source.zip?raw=true) and [SHA256 checksums](downloads/SHA256SUMS.txt) are also available.

Install the main ZIP with MO2 or Vortex, enable it, and launch through SKSE. Open Tween Menu and select **SKSE MOD SETTINGS**. The default Classic category is `right`. The same JSON entry supports the Alternative layout.

No ESP/ESL or Papyrus scripts are included. The separate MCM shortcut addon can be installed alongside this bridge.

## Configuration

Edit `Interface/tweenoptions/TweenMenuSMFBridge.json` and restart the game. Change `menuName`, `category` or `priority` as needed. Leave `papyrus_event` unchanged. Use UTF-8 when saving a translated label.

## Compatibility and status

Compiled with CommonLibSSE-NG for SE 1.5.97 / AE 1.6.x, with VR support disabled. Local build, interface and sequencing checks passed; these do not establish compatibility with every runtime or UI setup. The menu entry has been used in the uploader's modlist, but complete in-game opening/input behavior and compatibility require broader feedback.

Opening uses the framework's public window flag. Addons depending on its internal hotkey-open notification may need additional compatibility work. See `VALIDATION.md` and `THIRD_PARTY_NOTICES.md` for details.

## Reporting problems

Include your game runtime, SKSE and framework versions, Tween Menu layout, reproduction steps and the bridge log. The log is normally `Documents/My Games/Skyrim Special Edition/SKSE/TweenMenuSMFBridge.log`.

Please open a GitHub issue to report a problem or suggest an improvement. Feedback is welcome.

## Source and license

Source is included in this repository and in a separate download archive. The original bridge code is MIT licensed. Dependency license notices are included. See [BUILD.md](BUILD.md) for build instructions and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for credits.
