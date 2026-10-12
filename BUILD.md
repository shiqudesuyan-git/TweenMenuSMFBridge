# TweenMenuSMFBridge

A standalone SKSE plugin that adds **SKSE MOD SETTINGS** to Tween Menu Overhaul and opens the SKSE Menu Framework main control panel after Tween Menu has closed.

Install the release ZIP with MO2/Vortex. Requires SKSE64, Address Library, SkyUI, Tween Menu Overhaul, and an SKSE Menu Framework build exporting `GetMainWindow`. Both Classic and Alternative layouts use the same JSON. No ESP, Papyrus quest, custom SWF, key simulation, or save serialization is required. The existing Open MCM addon can be installed alongside this addon.

The NG build enables SE and AE and disables VR. Its intended runtime families are SE 1.5.97 and AE 1.6.x. Actual in-game compatibility has not yet been tested. This build does not claim compatibility with Skyrim 1.7.x.

## Build

Requires Windows x64, Visual Studio 2022 C++ Build Tools and Windows SDK, CMake 3.24+, CommonLibSSE-NG, and its fmt/spdlog/rapidcsv dependencies. Use matching static dependency packages with the dynamic MSVC runtime (`x64-windows-static-md`), or a compatible installed CommonLibSSE-NG package.

```powershell
git clone https://github.com/CharmedBaryon/CommonLibSSE-NG.git C:/dev/CommonLibSSE-NG
git -C C:/dev/CommonLibSSE-NG checkout b93280e832f263dbef44e44cbe2936622a02f91a
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 `
  -DCOMMONLIBSSE_SOURCE_DIR=C:/dev/CommonLibSSE-NG `
  -DCMAKE_PREFIX_PATH=C:/dev/dependencies/x64-windows-static-md
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
cmake --install build --config Release --component Mod --prefix out/release
```

Alternatively run `./scripts/build.ps1 -CommonLibSource C:/dev/CommonLibSSE-NG -DependencyPrefix C:/dev/dependencies/x64-windows-static-md`. Dependencies must already be installed. The script does not change any game or mod-manager directory.

To use an installed CommonLibSSE-NG package, omit `COMMONLIBSSE_SOURCE_DIR` and set `CMAKE_PREFIX_PATH` to the package prefix. Ensure that the installed package was built with both SE and AE enabled; an SE-only package will not produce an NG binary.

## How it works

The supplied JSON uses Tween Menu Overhaul's documented `papyrus_event` option. Despite the field name, Tween Menu sends this directly to the SKSE mod event dispatcher, so a native event sink can receive it without a Papyrus script.

The selection sends `TweenMenuSMFBridge_Open`. The bridge moves handling onto the SKSE task thread, checks whether Tween Menu is still open, waits for its close notification when necessary, and defers the framework opening to a subsequent task. Duplicate requests coalesce. A save load or new game cancels stale pending commands.

The framework DLL is resolved dynamically. The client uses the public SDK's `GetMainWindowFunction` and `WindowInterface`, setting `IsOpen` to true. Missing DLLs, missing exports, and an uninitialized main window are handled without dereferencing invalid pointers. It preserves `BlockUserInput`. This uses the SDK's window state API; it does not call the framework's internal `WindowManager::Open`, which also dispatches the framework-specific `kOpenMenu` event. Addons that depend on that internal notification need separate in-game compatibility evaluation.

## Validation

`BridgeChecks` loads a test DLL under the actual filename `SKSEMenuFramework.dll`. It checks the real public ABI and the sequencing controller without starting Skyrim. It also loads a legacy fixture without `GetMainWindow` to check the upgrade path. Test fixture DLLs are excluded from release packaging.

The checks cover missing dependencies, not-ready state, idempotent opening, preservation of input preference, deferred opening, both close/selection orderings, duplicate events, ordinary closes, load cancellation, and reopened menus. This does not validate Skyrim's UI/input/rendering behavior. Consult `VALIDATION.md` for the delivered build's results.

See `THIRD_PARTY_NOTICES.md` for exact upstream snapshots, licenses and source links. Upstream implementations were inspected, not included as replacement game plugins.

