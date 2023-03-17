#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Editor/Panels/EditorPanel.h"

namespace Engine
{
	class PerformancePanel : public EditorPanel
	{
	public:
		PerformancePanel() : EditorPanel("Performance") { }

		virtual void OnRender(float DeltaTime) override;
	};
}