#include <atomic>
#include <filesystem>
#include <cstdint>
namespace RE { class InputEvent; }
#include "SKSEMenuFramework.h"

namespace
{
    SKSEMenuFramework::Model::WindowInterface window;
    bool ready = true;
}

extern "C" __declspec(dllexport) SKSEMenuFramework::Model::WindowInterface* GetMainWindow()
{
    return ready ? &window : nullptr;
}
extern "C" __declspec(dllexport) bool TestIsOpen() { return window.IsOpen.load(); }
extern "C" __declspec(dllexport) void TestReset(bool isReady)
{
    ready = isReady;
    window.IsOpen.store(false);
    window.BlockUserInput.store(false);
}
extern "C" __declspec(dllexport) bool TestBlocksInput() { return window.BlockUserInput.load(); }

