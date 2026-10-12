# Build and validation record

Date: 2026-10-11 (Asia/Shanghai).

## Delivered binary

- Windows x64 Release DLL, compiled with MSVC 19.44 / Visual Studio 2022 Build Tools and Windows SDK 10.0.26100.
- CommonLibSSE-NG `b93280e832f263dbef44e44cbe2936622a02f91a`.
- Compile definitions verified: `ENABLE_SKYRIM_SE=1`, `ENABLE_SKYRIM_AE=1`, `HAS_SKYRIM_MULTI_TARGETING=1`; VR support disabled.
- DLL SHA256: `f888fc76ec871e94bd7ec8a459ff3268f20bcbac8ca5c3bae9a81fd985891030`.
- SDK header SHA256: `48416e8220ca777e2fffc2ef2baf21f699ab2e6c409d437f44eec5e311c3524c`. The vendored header was compared with the downloaded upstream copy and is unchanged.

## Completed checks

1. `cmake --build build --config Release --parallel 4`: passed, producing the plugin DLL and headless integration executable.
2. `ctest --test-dir build -C Release --output-on-failure`: 1/1 integration test passed. The test loads simulated current and legacy framework DLLs and exercises multiple scenarios:
   - DLL absent, `GetMainWindow` absent, null/not-ready main window.
   - Actual public window interface opening and repeated opening without toggling closed.
   - Preservation of the framework's input/pause preference.
   - No opening while Tween Menu is open; opening deferred onto the queue after closure.
   - Selection-before-close and close-before-selection event ordering.
   - Duplicate request and close-event coalescing.
   - Ordinary Tween closes without selecting the shortcut.
   - Pending opening cancelled when loading a save/new game.
   - Tween reopening while an opening task is already queued.
3. Actual release DLL loaded into an isolated Python process. `SKSEPlugin_Query` was called with the SKSE `PluginInfo` ABI and returned the correct plugin name, info ABI version, and packed plugin version. `SKSEPlugin_Load` was not called outside Skyrim.
4. PE inspection confirmed an x64 DLL and the `SKSEPlugin_Load`, `SKSEPlugin_Query`, and `SKSEPlugin_Version` exports. Imports consist of Windows and Microsoft C++ runtime libraries; the framework is resolved dynamically.
5. Release archive integrity, Data-root layout, JSON event contract, packaged DLL hash, documentation, and exclusion of fake/test DLLs are checked by `scripts/verify_release.py`.

## Scope of these results

These are build, ABI, task-sequencing, and packaging checks. No real Skyrim session was started, so the release has not been verified in-game. In-game rendering, input handling, framework addon notifications, controller behavior, and different runtime versions still require a real game session.

The source README describes the use of the public window flag API and its difference from the framework's internal hotkey path. No compatibility with VR or Skyrim 1.7.x is asserted.

