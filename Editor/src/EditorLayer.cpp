#include "EditorLayer.h"

#include "Engine/Types/Types.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"

#include "ImGuizmo/ImGuizmo.h"

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
		EditorScene* scene = new EditorScene();

		if (SceneManager::AddScene(TEXT("Editor Scene"), scene))
		{
			if(SceneManager::SetActiveScene(TEXT("Editor Scene")))
				ActiveScene = scene;
		}

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];

			CString path_wstring = TypeUtils::FromUTF8(sceneFilePath);

			SceneSerializer serializer(ActiveScene);
			serializer.Deserialize(path_wstring);
		}

		PManager.AddPanels({
			new ViewportPanel(),
			new SceneHierarchyPanel(),
			new ToolbarPanel(),
			new ConsolePanel(),
			new ContentBrowserPanel()
		});

		SceneManager::OnSetActiveScene().AddCallback(BIND_CLASS_FN(EditorLayer::OnActiveSceneChange));
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
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
					SaveScene();

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

	void EditorLayer::NewScene()
	{
		EditorScene* ref = new EditorScene();

		if (SceneManager::AddScene(TEXT("Editor Scene"), ref, true))
			ActiveScene = ref;
	}

	void EditorLayer::OpenScene()
	{
		CString filepath = FileDialogs::OpenFile(TEXT("Dragon Engine Scene (*.descene)\0*.descene\0"));

		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const CString& path)
	{
		NewScene();

		SceneSerializer s(ActiveScene);
		s.Deserialize(path);
	}

	void EditorLayer::SaveScene()
	{
		CString filepath = FileDialogs::SaveFile(TEXT("Dragon Engine Scene (*.descene)\0*.descene\0"));

		if (!filepath.empty())
			SaveScene(filepath);
	}

	void EditorLayer::SaveScene(const CString& path)
	{
		SceneSerializer s(ActiveScene);
		s.Serialize(path);
	}

	void EditorLayer::OnActiveSceneChange(Scene* scene)
	{
		ActiveScene->OnSceneEnd();

		ActiveScene = (EditorScene*)scene;

		ActiveScene->OnSceneBegin();
	}
}