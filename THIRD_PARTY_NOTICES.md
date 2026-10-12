# Sources and licenses

The bridge implementation is original code under the included MIT license.

## Upstream sources inspected

Source links were obtained from the authors' Nexus pages. Nexus's anonymous file download requires login; public author repositories provide the corresponding source without a Nexus file download.

- Tween Menu Overhaul: https://www.nexusmods.com/skyrimspecialedition/mods/143275
  - Source: https://github.com/jpsteel/skyrim-tweenmenu
  - Inspected commit: `0d77caaaf647775e78d563143c255e5e82171217`.
  - `src/Utility.cpp` documents the actual mod event dispatch and Tween close request. Both ActionScript layouts were inspected for the JSON contract.
  - JSON extension guide: https://www.nexusmods.com/skyrimspecialedition/articles/9866
- Open MCM Option Addon: https://www.nexusmods.com/skyrimspecialedition/mods/146153
  - Description and file listing inspected. Its binary files were not needed for this independent addon.
- SKSE Menu Framework: https://www.nexusmods.com/skyrimspecialedition/mods/120352
  - Source: https://github.com/QTR-Modding/SKSE-Menu-Framework-3
  - Inspected commit: `dc9182454556910b2fd873d770c345cfe6e53777`.
  - `GetMainWindow`, `WindowInterface`, framework pause handling, and menu input handling were inspected.

## Dependencies

- The unchanged `vendor/smf/SKSEMenuFramework.h` is from https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API at commit `1dcb70179076aae4ab626f43c5baab2735ca5877`.
  - Copyright and authorship remain with the upstream contributors.
  - The API repository is LGPL-2.1, included in `licenses/SKSEMenuFramework-API-LGPL-2.1.txt`.
  - The header is included with the source archive. The game framework remains a separately installed DLL.
- CommonLibSSE-NG: https://github.com/CharmedBaryon/CommonLibSSE-NG
  - Built from local checkout commit `b93280e832f263dbef44e44cbe2936622a02f91a`, with SE and AE enabled and VR disabled.
  - MIT; see `licenses/CommonLibSSE-NG-MIT.txt`.
- fmt and spdlog are linked from static dependency packages with the dynamic Microsoft C++ runtime.
  - Their license notices are included in `licenses/fmt-MIT.txt` and `licenses/spdlog-MIT.txt`.

The release ZIP does not redistribute Tween Menu Overhaul, Open MCM Option Addon, SKSE Menu Framework's implementation DLL, or the test framework DLLs.

