#pragma once

#include "Engine/Core/Core.h"

#include <vector>
#include <functional>

namespace Engine
{
    template<typename... Args>
    class CallbackDispatcher
    {
    public:
        CallbackDispatcher() = default;

        using CallbackType = std::function<void(Args...)>;

        CallbackDispatcher(std::initializer_list<CallbackType> initializers)
            : CallbackList(initializers) { }

        class CallbackHandle
        {
        public:
            void AddCallback(const CallbackType& callback)
            {
                Manager->CallbackList.push_back(callback);
            }

            friend class CallbackDispatcher;

        private:
            CallbackHandle(CallbackDispatcher* manager)
                : Manager(manager) { }

        private:
            CallbackDispatcher* Manager;
        };

        CallbackHandle GetHandle() { return CallbackHandle(this); }

        void Run(const Args&... args)
        {
            for (auto& Callback : CallbackList)
                Callback(args...);
        }

        friend class CallbackHandle;

    private:
        std::vector<CallbackType> CallbackList = { };
    };
}