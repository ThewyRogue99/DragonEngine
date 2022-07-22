#include "depch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

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
	}

	void ContentBrowserPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		if (CurrentDirectory != std::filesystem::path(AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				CurrentDirectory = CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.f;
		static float thumbnailSize = 100.f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = GetWidth();
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

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

			if (ImGui::BeginDragDropSource())
			{
				CString itemPath = (AssetPath / relativePath);

				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath.c_str(), (itemPath.size() + 1) * sizeof(wchar_t));

				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor(3);

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
}