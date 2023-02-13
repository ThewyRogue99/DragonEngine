#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Events/Event.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	struct WindowProps
	{
		const char* Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(
			const char* Title = "Dragon Engine",
			unsigned int Width = 1280,
			unsigned int Height = 720
		) : Title(Title), Width(Width), Height(Height) { }
	};

	class ENGINE_API Window
	{
	public:
		virtual ~Window() { }

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		inline CallbackDispatcher<void, Event&>::CallbackHandle OnEvent()
			{ return OnEventDispatch.GetHandle(); }

		virtual void SetVSync(bool Enabled) = 0;
		virtual bool IsVsync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Ref<Window> Create(const WindowProps& props = WindowProps());

	protected:
		CallbackDispatcher<void, Event&> OnEventDispatch;
	};
}