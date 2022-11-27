#pragma once

#include "Engine/Core/Layer.h"

struct ImGuiStyle;

namespace Engine
{
	class ENGINE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();

		void End();

	private:
		void SetUITheme(bool Dark = true, float Alpha = 1.f);

		void SetUIDarkBlueTheme();

		float Time = 0.f;
	};
}
