#include "EditorLayer.h"

#include "Engine/Types/Types.h"
#include "Scene/EditorSceneManager.h"

#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer2D.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <filesystem>
#include <nfd.h>

#include "Tools/EditorTool.h"
#include "Tools/ResourceTool.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/ToolbarPanel.h"
#include "Panels/ConsolePanel.h"

namespace Engine
{
	EditorLayer::EditorLayer() : Layer(TEXT("Editor Layer"))
	{

	}

	void EditorLayer::OnAttach()
	{
		ResourceTool::LoadResources();

		PManager.AddPanels({
			new ViewportPanel(),
			new SceneHierarchyPanel(),
			new ToolbarPanel(),
			new ConsolePanel(),
			new ContentBrowserPanel()
		});

		if (!ProjectManager::IsProjectLoaded())
			PManager.DisableAllPanels();

		EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

		EditorSceneManager::OnEditorSceneChange().AddCallback(BIND_CLASS_FN(EditorLayer::OnActiveSceneChange));
		EditorTool::OnBeginPlay().AddCallback(BIND_CLASS_FN(EditorLayer::OnBeginPlay));
		EditorTool::OnEndPlay().AddCallback(BIND_CLASS_FN(EditorLayer::OnEndPlay));

		ProjectManager::OnLoadProject().AddCallback(BIND_CLASS_FN(EditorLayer::OnProjectLoad));
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		Engine::Renderer2D::ResetStats();

		PManager.Update(DeltaTime);
	}

	void EditorLayer::OnImGuiRender(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", nullptr, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("Exit"))
					Engine::Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Project", "Ctrl+N"))
				{
					//std::filesystem::path FullPath = FileDialogs::SaveFile(L"Project Files (*.deproject*) \0*.deproject*\0");

					nfdchar_t* outPath = nullptr;
					nfdresult_t result = NFD_PickFolder(nullptr, &outPath);

					if (result == NFD_OKAY)
					{
						std::filesystem::path FullPath = outPath;
						free(outPath);

						std::string ProjectName = TypeUtils::FromUTF16(FullPath.filename());

						ProjectManager::CreateProject(FullPath, ProjectName);
					}
					else if (result == NFD_CANCEL)
					{
						DE_CORE_INFO("User cancelled file dialog");
					}
					else
					{
						DE_CORE_INFO("Error: {0}", NFD_GetError());
					}
				}
				if (ImGui::MenuItem("Load Project", "Ctrl+L"))
				{
					//CString FullPath = FileDialogs::OpenFile(L"Project Files (*.deproject*) \0*.deproject*\0");

					nfdchar_t* outPath = nullptr;
					nfdresult_t result = NFD_OpenDialog("deproject;", nullptr, &outPath);

					if (result == NFD_OKAY)
					{
						std::filesystem::path FullPath = outPath;
						free(outPath);

						ProjectManager::LoadProject(FullPath);
					}
					else if (result == NFD_CANCEL)
					{
						DE_CORE_INFO("User cancelled file dialog");
					}
					else
					{
						DE_CORE_INFO("Error: {0}", NFD_GetError());
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		auto stats = Engine::Renderer2D::GetStats();

		PManager.Render(DeltaTime);

		style.WindowMinSize.x = minWinSizeX;

		ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& event)
	{
		PManager.OnEvent(event);
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