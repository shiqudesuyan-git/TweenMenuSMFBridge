#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "FrameworkClient.h"
#include "OpenController.h"

namespace
{
    constexpr std::string_view kOpenEvent = "TweenMenuSMFBridge_Open";

    void Queue(bridge::OpenController::Task task)
    {
        SKSE::GetTaskInterface()->AddTask(std::move(task));
    }

    void OpenFramework()
    {
        const auto result = bridge::OpenFrameworkMainWindow();
        if (result == bridge::OpenResult::Opened) {
            SKSE::log::info("Opened framework after Tween Menu closed");
        } else {
            SKSE::log::error("{}", bridge::Describe(result));
            RE::DebugNotification(bridge::Describe(result));
        }
    }

    class EventSink final : public RE::BSTEventSink<SKSE::ModCallbackEvent>,
                            public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static EventSink& Get()
        {
            static EventSink sink;
            return sink;
        }

        void Register()
        {
            if (registered_) {
                return;
            }
            const auto callbacks = SKSE::GetModCallbackEventSource();
            const auto ui = RE::UI::GetSingleton();
            if (!callbacks || !ui) {
                SKSE::log::error("Cannot register: SKSE mod event source or game UI is missing");
                return;
            }
            callbacks->AddEventSink(this);
            ui->AddEventSink<RE::MenuOpenCloseEvent>(this);
            registered_ = true;
            SKSE::log::info("Listening for {} (SE/AE NG build)", kOpenEvent);
        }

        void Cancel()
        {
            ++session_;
            controller_.Cancel();
        }

        RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* event,
            RE::BSTEventSource<SKSE::ModCallbackEvent>*) override
        {
            if (event && event->eventName == kOpenEvent) {
                const auto session = session_.load();
                Queue([session] {
                    if (session == Get().session_.load()) {
                        Get().controller_.Request();
                    }
                });
            }
            return RE::BSEventNotifyControl::kContinue;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event,
            RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
        {
            if (event && !event->opening && event->menuName == RE::TweenMenu::MENU_NAME) {
                const auto session = session_.load();
                Queue([session] {
                    if (session == Get().session_.load()) {
                        Get().controller_.TweenClosed();
                    }
                });
            }
            return RE::BSEventNotifyControl::kContinue;
        }

    private:
        EventSink() : controller_([] {
            const auto ui = RE::UI::GetSingleton();
            return ui && ui->IsMenuOpen(RE::TweenMenu::MENU_NAME);
        }, Queue, OpenFramework) {}

        bridge::OpenController controller_;
        std::atomic<std::uint64_t> session_{0};
        bool registered_{false};
    };

    void OnSKSEMessage(SKSE::MessagingInterface::Message* message)
    {
        if (!message) {
            return;
        }
        switch (message->type) {
        case SKSE::MessagingInterface::kDataLoaded:
            EventSink::Get().Register();
            break;
        case SKSE::MessagingInterface::kPreLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            EventSink::Get().Cancel();
            break;
        default:
            break;
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);
    if (const auto directory = SKSE::log::log_directory()) {
        const auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            (*directory / "TweenMenuSMFBridge.log").string(), true);
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("TweenMenuSMFBridge", sink));
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);
    }
    if (!SKSE::GetTaskInterface() || !SKSE::GetMessagingInterface()) {
        SKSE::log::error("Required SKSE task or messaging interface is unavailable");
        return false;
    }
    const auto registered = SKSE::GetMessagingInterface()->RegisterListener(OnSKSEMessage);
    SKSE::log::info("TweenMenuSMFBridge 1.0.0, runtime {}", skse->RuntimeVersion().string());
    return registered;
}

