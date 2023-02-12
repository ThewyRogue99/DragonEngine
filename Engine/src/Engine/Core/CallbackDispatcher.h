#pragma once

#include "Engine/Core/Core.h"

#include <vector>
#include <functional>

namespace Engine
{
    template<typename ReturnType, typename... Args>
    class CallbackDispatcher
    {
    public:
        CallbackDispatcher() = default;

        using CallbackType = std::function<typename ReturnType(Args...)>;

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
            CallbackDispatcher* Manager = nullptr;
        };

        CallbackHandle GetHandle() { return CallbackHandle(this); }

        void Run(const Args&... args)
        {
            for (auto& Callback : CallbackList)
                Callback(args...);
        }

        auto begin() { return CallbackList.begin(); }
        auto end() { return CallbackList.end(); }

        void Clear()
        {
            CallbackList.clear();
        }

        friend class CallbackHandle;

    private:
        std::vector<CallbackType> CallbackList = { };
    };
}