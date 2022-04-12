#pragma once

#include "Engine/Layer.h"

namespace Engine
{
	class ENGINE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& event) override;

	private:
		float Time = 0.f;
	};
}
