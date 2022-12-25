#include "ToolbarPanel.h"

#include "Engine/Audio/AudioEngine.h"
#include "Engine/Scene/SceneManager.h"

#include "../Project/ProjectTools.h"

#pragma warning(disable : 4312)

namespace Engine
{
	ToolbarPanel::ToolbarPanel() : EditorPanel("##toolbar")
	{

	}

	void ToolbarPanel::OnCreate()
	{
		PlayIcon = Texture2D::Create(TEXT("Resource/Icon/PlayButton.png"));
		StopIcon = Texture2D::Create(TEXT("Resource/Icon/StopButton.png"));
		SimulateIcon = Texture2D::Create(TEXT("Resource/Icon/SimulateButton.png"));

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

		ActiveScene = SceneManager::GetActiveScene();

		SceneManager::OnSetActiveScene().AddCallback(BIND_CLASS_FN(ToolbarPanel::OnSetActiveScene));
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

						Scene* CopyScene = ActiveScene->Copy();

						SceneManager::AddScene(TEXT("Copy Scene"), CopyScene, true);
						SceneManager::SetActiveScene(TEXT("Copy Scene"));

						ActiveSceneState = SceneState::Play;
					} break;
					case SceneState::Play:
					{
						SceneManager::SetActiveScene(TEXT("Editor Scene"));
						SceneManager::RemoveScene(TEXT("Copy Scene"));

						AudioEngine::StopAllAudio();

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
						EditorScene* CopyScene = ((EditorScene*)ActiveScene)->CopyEditorScene();

						CopyScene->BeginSimulation();

						SceneManager::AddScene(TEXT("Copy Scene"), CopyScene, true);
						SceneManager::SetActiveScene(TEXT("Copy Scene"));

						ActiveSceneState = SceneState::Simulate;
					} break;
					case SceneState::Simulate:
					{
						((EditorScene*)ActiveScene)->EndSimulation();

						SceneManager::SetActiveScene(TEXT("Editor Scene"));
						SceneManager::RemoveScene(TEXT("Copy Scene"));

						ActiveSceneState = SceneState::Edit;
					} break;
					default: break;
				}
			}

			if (bShouldDisable)
				ImGui::EndDisabled();
		}
	}

	void ToolbarPanel::OnSetActiveScene(Scene* NewScene)
	{
		ActiveScene = (EditorScene*)NewScene;
	}
}