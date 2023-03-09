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

		virtual void OnRender(float DeltaTime) override;

	private:
		enum class SettingsCategory : uint32_t
		{
			Editor,
			Project,
			Appearance
		};

	private:
		void RenderSettings();

		const std::string GetCategoryString(SettingsCategory Category);

	private:
		SettingsCategory SelectedCategory = SettingsCategory::Editor;

		const SettingsCategory AvailableCategories[3] = { 
			SettingsCategory::Editor,
			SettingsCategory::Project,
			SettingsCategory::Appearance
		};
	};
}