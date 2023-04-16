#include "EditorLayer.h"

#include "Engine/Types/Types.h"
#include "Scene/EditorSceneManager.h"

#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Panels/SceneEditorPanel.h"
#include "Panels/SettingsPanel.h"

#include "Engine/Debug/Instrumentor.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <filesystem>
#include <nfd.h>

#include "Tools/EditorTool.h"
#include "Tools/ResourceTool.h"

namespace Engine
{
	EditorLayer::EditorLayer() : Layer(TEXT("Editor Layer"))
	{

	}

	void EditorLayer::OnAttach()
	{
		Layer::OnAttach();

		ResourceTool::LoadResources();

		PManager.AddPanels({
			{ TEXT("SceneEditor"), new SceneEditorPanel() },
			{ TEXT("Settings"), new SettingsPanel() }
		});

		PManager.GetPanel(TEXT("Settings"))->ClosePanel();

		EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

		EditorSceneManager::OnEditorSceneChange().AddCallback(BIND_CLASS_FN(EditorLayer::OnActiveSceneChange));
		EditorTool::OnBeginPlay().AddCallback(BIND_CLASS_FN(EditorLayer::OnBeginPlay));
		EditorTool::OnEndPlay().AddCallback(BIND_CLASS_FN(EditorLayer::OnEndPlay));
	}

	void EditorLayer::OnDetach()
	{
		Layer::OnDetach();
	}

	void EditorLayer::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		PManager.Update(DeltaTime);

		Engine::Renderer2D::ResetStats();
	}

	void EditorLayer::OnImGuiRender(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Editor", nullptr, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
			EditorTool::EditorDockspaceID = dockspace_id;
		}

		PManager.Render(DeltaTime);

		style.WindowMinSize.x = minWinSizeX;

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		PManager.OnEvent(event);
	}

	void EditorLayer::OpenSettingsPanel()
	{
		EditorPanel* Panel = PManager.GetPanel(TEXT("Settings"));
		Panel->OpenPanel();
		Data.bIsSettingsOpen = true;

		Panel->SetFocus();
	}

	void EditorLayer::OnActiveSceneChange(Scene* scene)
	{
		if(ActiveScene)
			ActiveScene->OnSceneEnd();

		ActiveScene = scene;

		ActiveScene->OnSceneBegin();
	}

	void EditorLayer::OnBeginPlay()
	{
		SceneManager::OnSetActiveScene().AddCallback(BIND_CLASS_FN(EditorLayer::OnActiveSceneChange));
		OnActiveSceneChange(SceneManager::GetActiveScene());
	}

	void EditorLayer::OnEndPlay()
	{
		OnActiveSceneChange(EditorSceneManager::GetEditorScene());
	}

	void EditorLayer::OnProjectLoad(const Project& project)
	{
		PManager.ActivateAllPanels();
	}
}