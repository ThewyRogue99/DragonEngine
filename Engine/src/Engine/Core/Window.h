#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Events/Event.h"

#include "Engine/Core/CallbackDispatcher.h"

#include <glm/vec2.hpp>

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

	class Window
	{
	public:
		virtual ~Window() { }

		virtual void OnUpdate() = 0;

		// Returns the size of the window in screen coordinates
		virtual glm::vec2 GetSize() const = 0;

		// Returns the size of the window frame in pixels
		virtual glm::vec2 GetFrameSize() const = 0;

		inline CallbackDispatcher<void, Event&>::CallbackHandle OnEvent()
			{ return OnEventDispatch.GetHandle(); }

		virtual void SetVSync(bool Enabled) = 0;
		virtual bool IsVsync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		ENGINE_API static Ref<Window> Create(const WindowProps& props = WindowProps());

	protected:
		CallbackDispatcher<void, Event&> OnEventDispatch;
	};
}