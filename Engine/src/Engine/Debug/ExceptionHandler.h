#pragma once
#include "Exception.h"
#include "Engine/Core/CallbackDispatcher.h"

#include "Engine/Core/Application.h"

#include <iostream>

#include <unordered_map>
#include <typeindex>
#include <type_traits>

namespace Engine
{
	class ExceptionHandler
	{
	public:
		template<typename T, typename... Args>
		static typename std::enable_if<std::is_base_of<Exception, T>::value>::type ThrowException(const Args&... args)
		{
			T* exc = new T(args...);

			bool bIsHandled = false;

			auto it = DispatcherMap.find(typeid(T));
			if (it != DispatcherMap.end())
			{
				CallbackDispatcher<bool, const T*>* dispatch = (CallbackDispatcher<bool, const T*>*)(it->second);

				for (auto& callback : *dispatch)
				{
					bIsHandled |= callback(exc);
				}
			}

			if (!bIsHandled)
			{
				// Close Program
				Application::Get().Close();

				// TODO: Implement a native MessageBox to display error and stack

				delete exc;
				exit(-1);
			}
			else
			{
				delete exc;
			}
		}

		template<typename T>
		static typename std::enable_if<
			std::is_base_of<Exception, T>::value,
			typename CallbackDispatcher<bool, const T*>::CallbackHandle>::type OnException() {

			using ConstType = const T*;

			auto it = DispatcherMap.find(typeid(T));
			if (it != DispatcherMap.end())
			{
				CallbackDispatcher<bool, ConstType>* dispatch = (CallbackDispatcher<bool, ConstType>*)(it->second);

				return dispatch->GetHandle();
			}
			else
			{
				CallbackDispatcher<bool, ConstType>* dispatch = new CallbackDispatcher<bool, ConstType>();
				DispatcherMap[typeid(T)] = dispatch;

				return dispatch->GetHandle();
			}
		}

	private:
		static std::unordered_map<std::type_index, void*> DispatcherMap;
	};
}