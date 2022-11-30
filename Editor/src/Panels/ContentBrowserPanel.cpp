#include "depch.h"
#include "ContentBrowserPanel.h"

#pragma warning(disable : 4312)

#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetUtils.h"
#include "Engine/Asset/AssetMetadata.h"
#include "Engine/Asset/Serializer/Serializer.h"

#include "../Project/ProjectManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Engine
{
	static Ref<Texture2D> DirectoryIcon = nullptr;
	static Ref<Texture2D> FileIcon = nullptr;
	static Ref<Texture2D> SceneFileIcon = nullptr;
	static Ref<Texture2D> ScriptFileIcon = nullptr;

	ContentBrowserPanel::ContentBrowserPanel() : EditorPanel("Content Browser")
	{
		DirectoryIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/DirectoryIcon.png"));
		FileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/FileIcon.png"));
		SceneFileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/SceneFileIcon.png"));
		ScriptFileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/ScriptFileIcon.png"));
		
		ProjectManager::OnLoadProject().AddCallback(BIND_EVENT_FN(ContentBrowserPanel::OnLoadProject));
	}

	void ContentBrowserPanel::OnCreate()
	{
		EditorPanel::OnCreate();

		if(!DirectoryIcon)
			DirectoryIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/DirectoryIcon.png"));

		if(!FileIcon)
			FileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/FileIcon.png"));

		if (!SceneFileIcon)
			SceneFileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/SceneFileIcon.png"));

		if (!ScriptFileIcon)
			ScriptFileIcon = Texture2D::Create(TEXT("Resource/Icon/ContentBrowser/ScriptFileIcon.png"));

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	}

	void ContentBrowserPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		DrawTopBar();

		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX;

		if (ImGui::BeginTable("ContentBrowser", 2, tableFlags, ImVec2(0.0f, 0.0f)))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGuiStyle& style = ImGui::GetStyle();
			
			DrawDirectoryBrowser();

			ImGui::TableSetColumnIndex(1);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
			
			DrawAssetBrowser();

			ImGui::PopStyleColor();

			ImGui::EndTable();
		}
	}

	void ContentBrowserPanel::DrawDirectoryTree(const std::filesystem::path& DirectoryPath)
	{
		int idx = 0;
		for (auto& it : AssetIterator(DirectoryPath))
		{
			if (it.GetType() == AssetType::Folder)
			{
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

				const CString& PathName = it.GetPathName();

				if (PathName == CurrentDirectory)
					flags |= ImGuiTreeNodeFlags_Selected;

				ImGui::PushID(idx++);

				std::string name = TypeUtils::FromUTF16(it.GetName());

				if (ImGui::TreeNodeEx(&idx, flags, name.c_str()))
				{
					DrawDirectoryTree(PathName);

					ImGui::TreePop();
				}

				ImGui::PopID();

				if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
				{
					CurrentDirectory = PathName;
					Reload();
				}
			}
		}
	}

	void ContentBrowserPanel::DrawDirectoryBrowser()
	{
		ImGui::BeginChild("##directory_browser");
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

			if (CurrentDirectory.empty())
				flags |= ImGuiTreeNodeFlags_Selected;

			std::string ProjectName = LoadedProject ? LoadedProject->Name : "Project";

			if (ImGui::TreeNodeEx(ProjectName.c_str(), flags))
			{
				DrawDirectoryTree(L"");
				ImGui::TreePop();
			}

			if (ImGui::IsItemHovered() && ImGui::IsItemClicked())
				CurrentDirectory = L"";
		}
		ImGui::EndChild();
	}

	static std::string _labelPrefix(const char* const label)
	{
		float width = ImGui::CalcItemWidth();

		float x = ImGui::GetCursorPosX();
		ImGui::Text(label);
		ImGui::SameLine();
		ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
		ImGui::SetNextItemWidth(-1);

		std::string labelID = "##";
		labelID += label;

		return labelID;
	}

	void ContentBrowserPanel::DrawAssetBrowser()
	{
		ImGui::BeginChild("##asset_browser");
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Create Folder"))
				{
					CreateContent(AssetType::Folder);
				}
				if (ImGui::MenuItem("Reload"))
				{
					Reload();
				}
				if (ImGui::MenuItem("New C# Script"))
				{
					CreateContent(AssetType::Script);
				}

				ImGui::EndPopup();
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

			for (auto& it : ContentList)
			{
				ImGui::PushID(i++);

				it.Draw(thumbnailSize);

				ImGui::PopID();
			}

			ImGui::Columns(1);
		}
		ImGui::EndChild();
	}

	namespace TopBarUtils
	{
		bool DrawTextButton(const char* label, const ImVec2& size = ImVec2(0, 0))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

			bool result = ImGui::Button(label, size);

			ImGui::PopStyleColor(3);

			return result;
		}
	}

	void ContentBrowserPanel::DrawTopBar()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_Tab]);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);

		ImGui::BeginChild("##top_bar", ImVec2((float)GetPanelAvailableWidth(), 20), false, ImGuiWindowFlags_NoDecoration);
		{
			if (TopBarUtils::DrawTextButton("<-"))
			{
				if (!CurrentDirectory.empty())
				{
					CurrentDirectory = CurrentDirectory.parent_path();
					Reload();
				}
			}

			ImGui::SameLine(0.f, 5.f);
			TopBarUtils::DrawTextButton("->");
			ImGui::SameLine(0.f, 15.f);

			if (TopBarUtils::DrawTextButton("Import Asset"))
			{
				std::filesystem::path FPath = FileDialogs::OpenFile(L"All Files (*.*) \0*.*\0");

				if (!std::filesystem::is_directory(FPath))
				{
					AssetManager::CreateAssetFromFile(FPath, CurrentDirectory);
					Reload();
				}
			}

			ImGui::SameLine(0.f, 5.f);

			if (TopBarUtils::DrawTextButton("Save"))
			{
				Scene* ActiveScene = SceneManager::GetActiveScene();
				AssetMetadata data;

				SceneSerializer::Serialize(ActiveScene, data);

				AssetManager::CreateAsset(CurrentDirectory, ActiveScene->GetName(), data, AssetType::Scene, true);
				Reload();
			}

			ImGui::SameLine(0.f, 5.f);
		}
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);
	}

	void ContentBrowserPanel::BrowserContent::Draw(float ThumbnailSize)
	{
		Ref<Texture2D> icon = nullptr;
		switch (Type)
		{
		case AssetType::Folder:
			icon = DirectoryIcon;
			break;
		case AssetType::Scene:
			icon = SceneFileIcon;
			break;
		case AssetType::Script:
			icon = ScriptFileIcon;
			break;
		default:
			icon = FileIcon;
			break;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

		ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { ThumbnailSize, ThumbnailSize }, { 0.f, 1.f }, { 1.f, 0.f });

		ImGui::PopStyleColor(3);

		if (bIsCreated)
		{
			DrawCreatedContent();
		}
		else
		{
			DrawUncreatedContent();
		}
	}

	void ContentBrowserPanel::BrowserContent::DrawUncreatedContent()
	{
		bool isDirectory = Type == AssetType::Folder;

		if (!RenameBuffer)
		{
			RenameBuffer = new char[RenameBufferSize];
			memset(RenameBuffer, 0, RenameBufferSize);
		}

		if (bIsError)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.f, 0.f, 1.f));
		}

		ImGui::InputText("##name", RenameBuffer, RenameBufferSize);

		if (bIsError)
		{
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		ImGui::SetKeyboardFocusHere(-1);

		if (
			!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Enter)
		){
			bIsError = !Panel->OnCreateContent(this);

			if (!bIsError)
			{
				bIsCreated = true;

				delete[] RenameBuffer;
				RenameBuffer = nullptr;

				Panel->Reload();
			}
		}
	}

	void ContentBrowserPanel::BrowserContent::DrawCreatedContent()
	{
		bool isDirectory = Type == AssetType::Folder;
		bool lastRenameState = bShouldRename;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
			{
				StartRename();
			}
			if (ImGui::MenuItem("Remove"))
			{
				if (isDirectory)
				{
					AssetManager::RemoveFolder(Entry.GetPath(), Entry.GetName());
					Panel->Reload();
				}
				else
				{
					AssetManager::RemoveAsset(Entry.GetPath(), Entry.GetName());

					Panel->Reload();
				}
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			if (!isDirectory)
			{
				std::string id = Entry.GetID();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", id.c_str(), id.size() + 1);
			}

			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (isDirectory)
			{
				Panel->CurrentDirectory /= Entry.GetName();
				Panel->Reload();
			}
		}

		std::string cName = TypeUtils::FromUTF16(Entry.GetName());

		if (bShouldRename)
		{
			ImGui::InputText("##name", RenameBuffer, RenameBufferSize, ImGuiInputTextFlags_AutoSelectAll);

			if (!lastRenameState)
				ImGui::SetKeyboardFocusHere(-1);

			if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				StopRename();
			}
			else if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Enter))
			{
				StopRename();
			}
		}
		else
		{
			ImGui::TextWrapped(cName.c_str());
		}

		ImGui::NextColumn();
	}

	void ContentBrowserPanel::BrowserContent::StartRename()
	{
		if (!bShouldRename)
		{
			bShouldRename = true;
			size_t BufferSize = RenameBufferSize + 1;

			RenameBuffer = new char[BufferSize];

			memset(RenameBuffer, 0, BufferSize);

			std::string str = TypeUtils::FromUTF16(Entry.GetName());

			strcpy(RenameBuffer, str.c_str());
		}
	}

	void ContentBrowserPanel::BrowserContent::StopRename()
	{
		if (bShouldRename)
		{
			bShouldRename = false;

			CString NewName = TypeUtils::FromUTF8(RenameBuffer);

			if (Type == AssetType::Folder)
			{
				AssetManager::RenameFolder(Entry.GetPath(), Entry.GetName(), NewName);
			}
			else
			{
				AssetManager::RenameAsset(Entry.GetID(), NewName);
			}

			Panel->Reload();

			delete[] RenameBuffer;
		}
	}

	void ContentBrowserPanel::CreateContent(AssetType type)
	{
		BrowserContent content = BrowserContent();
		content.bIsCreated = false;
		content.Type = type;
		content.Panel = this;

		ContentList.push_back(content);
	}

	bool ContentBrowserPanel::OnCreateContent(BrowserContent* Content)
	{
		switch (Content->Type)
		{
		case AssetType::Folder:
		{
			CString FolderName = TypeUtils::FromUTF8(Content->RenameBuffer);
			return AssetManager::CreateFolder(CurrentDirectory, FolderName);
		}
		case AssetType::Script:
		{
			std::string ScriptName = Content->RenameBuffer;
			if (ProjectManager::CreateScript(ScriptName))
			{
				AssetMetadata data;
				data.SetStringField("Name", ScriptName);

				return AssetManager::CreateAsset(
					CurrentDirectory,
					TypeUtils::FromUTF8(ScriptName),
					data,
					AssetType::Script
				);
			}

			return false;
		}
		default:
			return false;
		}
	}

	void ContentBrowserPanel::Reload()
	{
		AssetIterator it(CurrentDirectory);

		ContentList.clear();

		for (auto& content : it)
		{
			BrowserContent data(this, content);

			ContentList.push_back(data);
		}
	}

	void ContentBrowserPanel::OnLoadProject(const Project& project)
	{
		LoadedProject = &project;
		Reload();
	}
}