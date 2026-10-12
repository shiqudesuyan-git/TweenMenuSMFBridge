#include <Windows.h>
#include <deque>
#include <iostream>
#include <stdexcept>
#include "FrameworkClient.h"
#include "OpenController.h"

void Require(bool condition, const char* message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

int main(int argc, char** argv)
{
    try {
        Require(argc == 3, "Expected current and legacy fixture paths");
        using bridge::OpenResult;
        Require(bridge::OpenFrameworkMainWindow() == OpenResult::FrameworkMissing, "Missing DLL must be handled");
        const auto legacy = LoadLibraryA(argv[2]);
        Require(legacy != nullptr, "Could not load legacy fixture");
        Require(bridge::OpenFrameworkMainWindow() == OpenResult::UnsupportedFramework, "Missing export must be handled");
        FreeLibrary(legacy);
        const auto module = LoadLibraryA(argv[1]);
        Require(module != nullptr, "Could not load current fixture");
        const auto reset = reinterpret_cast<void (*)(bool)>(GetProcAddress(module, "TestReset"));
        const auto isOpen = reinterpret_cast<bool (*)()>(GetProcAddress(module, "TestIsOpen"));
        const auto blocks = reinterpret_cast<bool (*)()>(GetProcAddress(module, "TestBlocksInput"));
        Require(reset && isOpen && blocks, "Fixture controls are missing");
        reset(false);
        Require(bridge::OpenFrameworkMainWindow() == OpenResult::NotInitialized, "Null window must be handled");
        reset(true);
        Require(bridge::OpenFrameworkMainWindow() == OpenResult::Opened && isOpen(), "Public API must open the actual exported window");
        Require(bridge::OpenFrameworkMainWindow() == OpenResult::Opened && isOpen(), "Repeated opens must not toggle closed");
        Require(!blocks(), "Opening must preserve framework pause/input preference");

        bool tweenOpen = true;
        int opens = 0;
        std::deque<bridge::OpenController::Task> tasks;
        bridge::OpenController controller([&] { return tweenOpen; },
            [&](auto task) { tasks.push_back(std::move(task)); }, [&] {
                Require(!tweenOpen, "Framework was opened before Tween Menu closed");
                Require(bridge::OpenFrameworkMainWindow() == OpenResult::Opened, "Framework failed to open");
                ++opens;
            });
        const auto drain = [&] {
            while (!tasks.empty()) {
                auto task = std::move(tasks.front());
                tasks.pop_front();
                task();
            }
        };
        reset(true);
        controller.Request();
        controller.Request();
        drain();
        Require(!isOpen() && opens == 0, "Selection while Tween is open must wait");
        tweenOpen = false;
        controller.TweenClosed();
        controller.TweenClosed();
        Require(!isOpen(), "Opening must be deferred until the queued game task");
        drain();
        Require(isOpen() && opens == 1, "Duplicate events must coalesce into one opening");

        reset(true);
        controller.TweenClosed();
        drain();
        Require(!isOpen() && opens == 1, "Closing Tween without selecting the shortcut must do nothing");
        controller.Request();
        drain();
        Require(isOpen() && opens == 2, "Close-before-request ordering must work");

        reset(true);
        controller.Request();
        controller.Cancel();
        drain();
        Require(!isOpen() && opens == 2, "Loading a save must cancel queued opening");
        controller.Request();
        tweenOpen = true;
        drain();
        Require(!isOpen(), "Reopened Tween must delay an already queued opening");
        tweenOpen = false;
        controller.TweenClosed();
        drain();
        Require(isOpen() && opens == 3, "Delayed opening must resume after the next close");
        FreeLibrary(module);
        std::cout << "PASS: DLL ABI, dependency failures, close ordering, duplicate events, cancellation, and reopened-menu handling\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}

