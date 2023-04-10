#include "SceneEditorPanel.h"

#include "SceneEditor/Panels.h"
#include "Engine/Debug/Debug.h"

#include "Editor/Project/ProjectManager.h"
#include "Editor/Tools/EditorTool.h"
#include "Editor/EditorLayer.h"

#include <nfd.h>
#include <imgui_internal.h>

#include "IconsFontAwesome6.h"

namespace Engine
{
	SceneEditorPanel::SceneEditorPanel() : EditorPanel(ICON_FA_CUBES "  Scene Editor")
	{

	}

	void SceneEditorPanel::OnCreate()
	{
		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		SetWindowFlag(window_flags);

		PManager.AddPanels({
			{ TEXT("Viewport"), new ViewportPanel()},
			{ TEXT("SceneHierarchy"), new SceneHierarchyPanel() },
			{ TEXT("Toolbar"), new ToolbarPanel() },
			{ TEXT("Console"), new ConsolePanel() },
			{ TEXT("ContentBrowser"), new ContentBrowserPanel() },
			{ TEXT("Performance"), new PerformancePanel() }
		});

		PManager.GetPanel(TEXT("Performance"))->ClosePanel();

		if (!ProjectManager::IsProjectLoaded())
			PManager.DisableAllPanels();

		ProjectManager::OnLoadProject().AddCallback(BIND_CLASS_FN(OnProjectLoad));
	}

	void SceneEditorPanel::OnUpdate(float DeltaTime)
	{
		PManager.Update(DeltaTime);
	}

	void SceneEditorPanel::OnRender(float DeltaTime)
	{
		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("SceneEditorDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem(ICON_FA_GEAR "  Settings"))
				{
					EditorTool::GetEditorLayer()->OpenSettingsPanel();
				}
				if (ImGui::MenuItem(ICON_FA_DOOR_OPEN "  Exit"))
				{
					Engine::Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FOLDER_PLUS "  New Project", "Ctrl+N"))
				{
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
						DE_WARN(FileDialog, "User cancelled file dialog");
					}
					else
					{
						DE_ERROR(FileDialog, "Error: {0}", NFD_GetError());
					}
				}
				if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Load Project", "Ctrl+L"))
				{
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
						DE_WARN(FileDialog, "User cancelled file dialog");
					}
					else
					{
						DE_ERROR(FileDialog, "Error: {0}", NFD_GetError());
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Panels"))
				{
					if (ImGui::MenuItem("Scene Hierarchy", nullptr, &(Data.bSceneHierarchyOpen)))
					{
						if (Data.bSceneHierarchyOpen)
						{
							PManager.GetPanel(TEXT("SceneHierarchy"))->OpenPanel();
						}
						else
						{
							PManager.GetPanel(TEXT("SceneHierarchy"))->ClosePanel();
						}
					}
					if (ImGui::MenuItem("Console", nullptr, &(Data.bConsoleOpen)))
					{
						if (Data.bConsoleOpen)
						{
							PManager.GetPanel(TEXT("Console"))->OpenPanel();
						}
						else
						{
							PManager.GetPanel(TEXT("Console"))->ClosePanel();
						}
					}
					if (ImGui::MenuItem("Content Browser", nullptr, &(Data.bContentBrowserOpen)))
					{
						if (Data.bContentBrowserOpen)
						{
							PManager.GetPanel(TEXT("ContentBrowser"))->OpenPanel();
						}
						else
						{
							PManager.GetPanel(TEXT("ContentBrowser"))->ClosePanel();
						}
					}

					if (ImGui::MenuItem("Performance", nullptr, &(Data.bPerformanceOpen)))
					{
						if (Data.bPerformanceOpen)
						{
							PManager.GetPanel(TEXT("Performance"))->OpenPanel();
						}
						else
						{
							PManager.GetPanel(TEXT("Performance"))->ClosePanel();
						}
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		PManager.Render(DeltaTime);
	}

	void SceneEditorPanel::OnEvent(Event& event)
	{
		PManager.OnEvent(event);
	}

	void SceneEditorPanel::OnProjectLoad(const Project&)
	{
		PManager.ActivateAllPanels();
	}
}