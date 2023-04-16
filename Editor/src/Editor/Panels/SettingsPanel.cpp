#include "SettingsPanel.h"

#include "IconsFontAwesome6.h"

namespace Engine
{
    SettingsPanel::SettingsPanel() : EditorPanel(ICON_FA_GEAR "  Settings")
    {
        SetShowCloseButton(true);
    }

    void SettingsPanel::OnRender(float DeltaTime)
	{
        ImGui::Columns(2, "SettingsColumns");

        uint32_t CategoryCount = sizeof(AvailableCategories) / sizeof(SettingsCategory);

        ImGui::BeginChild("##categories");

        // Show categories on the left
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
        for (uint32_t i = 0; i < CategoryCount; i++)
        {
            SettingsCategory s = (SettingsCategory)i;

            const bool isSelected = (SelectedCategory == s);

            if (ImGui::Selectable(GetCategoryString(s).c_str(), isSelected))
            {
                SelectedCategory = s;
            }
        }
        ImGui::PopStyleVar();

        ImGui::EndChild();

        ImGui::NextColumn();

        RenderSettings();
	}

    void SettingsPanel::RenderSettings()
    {
        switch (SelectedCategory)
        {
        case SettingsCategory::Editor:
            ImGui::Text("Editor Settings");
            break;
        case SettingsCategory::Project:
            ImGui::Text("Project Settings");
            break;
        case SettingsCategory::Appearance:
            ImGui::Text("Appearance Settings");
            break;
        default:
            break;
        }
    }

    const CString SettingsPanel::GetCategoryString(SettingsCategory Category)
    {
        switch (Category)
        {
        case SettingsCategory::Editor:
            return "Editor";
        case SettingsCategory::Project:
            return "Project";
        case SettingsCategory::Appearance:
            return "Appearance";
        }
    }
}