#pragma once
#include <cstdint>
#include <functional>
#include <utility>

namespace bridge
{
    // All methods and queued callbacks run on Skyrim's task thread.
    // The close event may arrive before or after the selection event.
    class OpenController
    {
    public:
        using Task = std::function<void()>;
        OpenController(std::function<bool()> tweenIsOpen,
            std::function<void(Task)> enqueue, Task openFramework) :
            tweenIsOpen_(std::move(tweenIsOpen)), enqueue_(std::move(enqueue)),
            openFramework_(std::move(openFramework)) {}

        void Request()
        {
            pending_ = true;
            if (!tweenIsOpen_()) {
                QueueOpen();
            }
        }

        void TweenClosed()
        {
            if (pending_) {
                QueueOpen();
            }
        }

        void Cancel()
        {
            ++generation_;
            pending_ = false;
            queued_ = false;
        }

    private:
        void QueueOpen()
        {
            if (queued_) {
                return;
            }
            queued_ = true;
            const auto generation = generation_;
            enqueue_([this, generation] {
                if (generation != generation_) {
                    return;
                }
                queued_ = false;
                if (!pending_ || tweenIsOpen_()) {
                    return;
                }
                pending_ = false;
                openFramework_();
            });
        }

        std::function<bool()> tweenIsOpen_;
        std::function<void(Task)> enqueue_;
        Task openFramework_;
        std::uint64_t generation_{0};
        bool pending_{false};
        bool queued_{false};
    };
}

