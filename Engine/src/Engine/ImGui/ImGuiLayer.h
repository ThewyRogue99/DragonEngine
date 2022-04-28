#pragma once

#include "Engine/Layer.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

namespace Engine
{
	class ENGINE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender(Timestep DeltaTime) override;

		void Begin();

		void End();

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& event);

		float Time = 0.f;

		bool bShowDemoWindow = false;
	};
}
