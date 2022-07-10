#include "ToolbarPanel.h"

#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	ToolbarPanel::ToolbarPanel() : EditorPanel("##toolbar")
	{

	}

	void ToolbarPanel::OnCreate()
	{
		PlayIcon = Texture2D::Create(TEXT("Resource/Icon/PlayButton.png"));
		StopIcon = Texture2D::Create(TEXT("Resource/Icon/StopButton.png"));

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

		ActiveScene = std::static_pointer_cast<EditorScene>(SceneManager::GetActiveScene());

		SceneManager::OnSetActiveScene().AddCallback(BIND_CLASS_FN(ToolbarPanel::OnSetActiveScene));
	}

	void ToolbarPanel::OnRender(float DeltaTime)
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		float buttonSize = windowSize.y - 6.f;

		Scene::SceneState CurrentSceneState = ActiveScene->GetSceneState();

		Ref<Texture2D> icon = (CurrentSceneState == Scene::SceneState::Edit) ? PlayIcon : StopIcon;

		ImGui::SetCursorPos(ImVec2((windowSize.x * 0.5f) - (buttonSize * 0.5f), (windowSize.y * 0.5f) - (buttonSize * 0.5f)));

		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(buttonSize, buttonSize), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 0))
		{
			if (CurrentSceneState == Scene::SceneState::Edit)
				ActiveScene->SetSceneState(Scene::SceneState::Play);
			else if (CurrentSceneState == Scene::SceneState::Play)
				ActiveScene->SetSceneState(Scene::SceneState::Edit);
		}
	}

	void ToolbarPanel::OnSetActiveScene(Ref<Scene> NewScene)
	{
		ActiveScene = std::static_pointer_cast<EditorScene>(NewScene);
	}
}