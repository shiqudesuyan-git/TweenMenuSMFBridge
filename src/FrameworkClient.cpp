#include <atomic>
#include <filesystem>
#include <cstdint>
namespace RE { class InputEvent; }
#include "SKSEMenuFramework.h"
#include "FrameworkClient.h"

namespace bridge
{
    OpenResult OpenFrameworkMainWindow()
    {
        const auto module = GetModuleHandleW(L"SKSEMenuFramework.dll");
        if (!module) {
            return OpenResult::FrameworkMissing;
        }
        using GetMainWindow = SKSEMenuFramework::Model::GetMainWindowFunction;
        const auto getMainWindow = reinterpret_cast<GetMainWindow>(GetProcAddress(module, "GetMainWindow"));
        if (!getMainWindow) {
            return OpenResult::UnsupportedFramework;
        }
        const auto window = getMainWindow();
        if (!window) {
            return OpenResult::NotInitialized;
        }
        window->IsOpen.store(true);
        return OpenResult::Opened;
    }

    const char* Describe(OpenResult result)
    {
        switch (result) {
        case OpenResult::Opened:
            return "Opened SKSE Menu Framework";
        case OpenResult::FrameworkMissing:
            return "SKSE Menu Framework is not loaded. Check its installation and SKSE log.";
        case OpenResult::UnsupportedFramework:
            return "Update SKSE Menu Framework: this version does not export GetMainWindow.";
        case OpenResult::NotInitialized:
            return "SKSE Menu Framework is not ready. Try again after the game has finished loading.";
        }
        return "Unknown framework response";
    }
}

