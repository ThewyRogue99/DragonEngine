#include "ToolbarPanel.h"

#include "../Scene/EditorSceneManager.h"
#include "../Tools/ResourceTool.h"
#include "../Tools/EditorTool.h"
#include "../Project/ProjectTools.h"

#pragma warning(disable : 4312)

namespace Engine
{
	ToolbarPanel::ToolbarPanel() : EditorPanel("##toolbar")
	{

	}

	void ToolbarPanel::OnCreate()
	{
		PlayIcon =  ResourceTool::GetIcon(TEXT("PlayButton"));
		StopIcon = ResourceTool::GetIcon(TEXT("StopButton"));
		SimulateIcon = ResourceTool::GetIcon(TEXT("SimulateButton"));

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 2.f));
		SetPanelStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.f, 0.f));

		SetPanelStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		SetPanelStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
		SetPanelStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

		SetWindowFlag(
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse
		);

		ActiveScene = EditorSceneManager::GetEditorScene();

		EditorSceneManager::OnEditorSceneChange().AddCallback(BIND_CLASS_FN(ToolbarPanel::OnEditorSceneChange));
		EditorTool::OnBeginPlay().AddCallback(BIND_CLASS_FN(ToolbarPanel::OnBeginPlay));
		EditorTool::OnEndPlay().AddCallback(BIND_CLASS_FN(ToolbarPanel::OnEndPlay));
	}

	void ToolbarPanel::OnRender(float DeltaTime)
	{
		float buttonSize = ImGui::GetWindowHeight() - 4.0f;

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - ((buttonSize * 2.f + 10.f) * 0.5f));

		// Play Button
		{
			bool bShouldDisable = ActiveSceneState == SceneState::Simulate;

			Ref<Texture2D> icon = (ActiveSceneState == SceneState::Edit || bShouldDisable) ? PlayIcon : StopIcon;

			if (bShouldDisable)
				ImGui::BeginDisabled();

			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				switch (ActiveSceneState)
				{
					case SceneState::Edit:
					{
						ProjectTools::CompileScriptProject();

						EditorTool::BeginPlay();

						ActiveSceneState = SceneState::Play;
					} break;
					case SceneState::Play:
					{
						EditorTool::EndPlay();

						ActiveSceneState = SceneState::Edit;
					} break;
					default: break;
				}
			}

			if (bShouldDisable)
				ImGui::EndDisabled();
		}

		ImGui::SameLine(0.f, 10.f);

		// Simulation Button
		{
			bool bShouldDisable = ActiveSceneState == SceneState::Play;

			Ref<Texture2D> icon = (ActiveSceneState == SceneState::Edit || bShouldDisable) ? SimulateIcon : StopIcon;

			if (bShouldDisable)
				ImGui::BeginDisabled();

			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				switch (ActiveSceneState)
				{
					case SceneState::Edit:
					{
						EditorTool::BeginSimulation();

						ActiveSceneState = SceneState::Simulate;
					} break;
					case SceneState::Simulate:
					{
						EditorTool::EndSimulation();

						ActiveSceneState = SceneState::Edit;
					} break;
					default: break;
				}
			}

			if (bShouldDisable)
				ImGui::EndDisabled();
		}
	}

	void ToolbarPanel::OnEditorSceneChange(EditorScene* NewScene)
	{
		if (!EditorTool::IsPlaying())
			ActiveScene = NewScene;
	}

	void ToolbarPanel::OnBeginPlay()
	{
		SceneManager::OnSetActiveScene().AddCallback([&] (Scene* NewScene) { ActiveScene = NewScene; });
		ActiveScene = SceneManager::GetActiveScene();
	}

	void ToolbarPanel::OnEndPlay()
	{
		ActiveScene = EditorSceneManager::GetEditorScene();
	}
}