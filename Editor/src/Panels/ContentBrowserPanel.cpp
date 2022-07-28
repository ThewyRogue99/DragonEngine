#include "depch.h"
#include "ContentBrowserPanel.h"

#include "Engine/Core/UUID.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Engine
{
	Ref<Texture2D> ContentBrowserPanel::DirectoryIcon = nullptr;
	Ref<Texture2D> ContentBrowserPanel::FileIcon = nullptr;

	static const std::filesystem::path AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel() : EditorPanel("Content Browser"), CurrentDirectory(AssetPath)
	{
		DirectoryIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/DirectoryIcon.png"));
		FileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/FileIcon.png"));
	}

	void ContentBrowserPanel::OnCreate()
	{
		EditorPanel::OnCreate();

		if(!DirectoryIcon)
			DirectoryIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/DirectoryIcon.png"));

		if(!FileIcon)
			FileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/FileIcon.png"));

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	}

	void ContentBrowserPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX;

		if (ImGui::BeginTable("ContentBrowser", 2, tableFlags, ImVec2(0.0f, 0.0f)))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGuiStyle& style = ImGui::GetStyle();
			
			ImGui::BeginChild("##directory_browser");
			{
				static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

				if (CurrentDirectory == AssetPath)
					flags |= ImGuiTreeNodeFlags_Selected;

				if (ImGui::TreeNodeEx("Project", flags))
				{
					DrawDirectoryTree(AssetPath);
					ImGui::TreePop();
				}

				if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
					CurrentDirectory = AssetPath;
			}
			ImGui::EndChild();

			ImGui::TableSetColumnIndex(1);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
			ImGui::BeginChild("##asset_browser");
			{
				if (CurrentDirectory != std::filesystem::path(AssetPath))
				{
					if (ImGui::Button("<-"))
					{
						CurrentDirectory = CurrentDirectory.parent_path();
					}
				}

				static float padding = 10.f;
				static float thumbnailSize = 110.f;
				float cellSize = thumbnailSize + padding;

				float browserWidth = ImGui::GetContentRegionAvail().x;
				int columnCount = (int)(browserWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				ImGui::Columns(columnCount, "##asset", false);

				int i = 0;

				for (auto& it : std::filesystem::directory_iterator(CurrentDirectory))
				{
					ImGui::PushID(i++);

					const auto& path = it.path();
					auto relativePath = std::filesystem::relative(path, AssetPath);
					std::string pathString = relativePath.filename().string();

					Ref<Texture2D> icon = it.is_directory() ? DirectoryIcon : FileIcon;

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
					ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

					ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0.f, 1.f }, { 1.f, 0.f });

					ImGui::PopStyleColor(3);

					if (ImGui::BeginDragDropSource())
					{
						if (!it.is_directory())
						{
							CString itemPath = (AssetPath / relativePath);

							ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), (itemPath.size() + 1) * sizeof(wchar_t));
						}

						ImGui::EndDragDropSource();
					}

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if(it.is_directory())
							CurrentDirectory /= path.filename();
					}

					ImGui::TextWrapped(pathString.c_str());

					ImGui::NextColumn();

					ImGui::PopID();
				}

				ImGui::Columns(1);
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndTable();
		}
	}

	void ContentBrowserPanel::DrawDirectoryTree(const std::filesystem::path& DirectoryPath)
	{
		if (std::filesystem::is_directory(DirectoryPath))
		{
			int idx = 0;
			for (auto& it : std::filesystem::directory_iterator(DirectoryPath))
			{
				if (it.is_directory())
				{
					const auto& path = it.path();
					auto relativePath = std::filesystem::relative(path, AssetPath);
					std::string pathString = relativePath.filename().string();

					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

					if (path == CurrentDirectory)
						flags |= ImGuiTreeNodeFlags_Selected;

					ImGui::PushID(idx++);

					if (ImGui::TreeNodeEx((void*)idx, flags, pathString.c_str()))
					{
						DrawDirectoryTree(path);

						ImGui::TreePop();
					}

					ImGui::PopID();

					if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
						CurrentDirectory = path;
				}
			}
		}
	}
}