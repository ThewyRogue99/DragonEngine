#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "EditorPanel.h"

namespace Engine
{
	class SettingsPanel : public EditorPanel
	{
	public:
		SettingsPanel() : EditorPanel("Settings")
		{
			SetShowCloseButton(true);
		}
	};
}