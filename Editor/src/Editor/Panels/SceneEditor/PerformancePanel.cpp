#include "PerformancePanel.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	void PerformancePanel::OnRender(float DeltaTime)
	{
		const Renderer2D::Statistics& stats = Renderer2D::GetStats();

		ImGui::Text("FPS: %f", 1 / DeltaTime);
		ImGui::Text("Draw Calls: %i", stats.DrawCalls);
		ImGui::Text("Quad Count: %i", stats.QuadCount);
	}
}