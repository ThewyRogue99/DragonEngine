#include "depch.h"
#include "ContentBrowserPanel.h"

#pragma warning(disable : 4312)

#include "PanelDragPayload.h"

#include "Engine/Renderer/Texture.h"
#include "Editor/Scene/EditorSceneManager.h"
#include "Engine/Asset/AssetUtils.h"
#include "Engine/Asset/AssetMetadata.h"

#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/TextureManager.h"
#include "Editor/Asset/Serializer/Serializer.h"

#include "Engine/Scripting/ScriptEngine.h"

#include "Editor/Tools/EditorTool.h"
#include "Editor/Tools/ResourceTool.h"

#include "Editor/Project/ProjectManager.h"
#include "Editor/Project/ProjectTools.h"

#include "IconsFontAwesome6.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <nfd.h>

namespace Engine
{
	static Ref<Texture2D> DirectoryIcon = nullptr;
	static Ref<Texture2D> FileIcon = nullptr;
	static Ref<Texture2D> SceneFileIcon = nullptr;
	static Ref<Texture2D> ScriptFileIcon = nullptr;
	static Ref<Texture2D> AudioFileIcon = nullptr;

	const ImVec2 ContentBrowserPanel::BrowserContent::Size = ImVec2(110.f, 150.f);

	ContentBrowserPanel::ContentBrowserPanel() : EditorPanel(ICON_FA_FOLDER_OPEN "  Content Browser")
	{
		ContentList = new std::vector<BrowserContent>();
	}

	void ContentBrowserPanel::OnCreate()
	{
		EditorPanel::OnCreate();

		DirectoryIcon = ResourceTool::GetImageIcon(TEXT("DirectoryIcon"));
		FileIcon = ResourceTool::GetImageIcon(TEXT("FileIcon"));
		SceneFileIcon = ResourceTool::GetImageIcon(TEXT("SceneFileIcon"));
		ScriptFileIcon = ResourceTool::GetImageIcon(TEXT("ScriptFileIcon"));
		AudioFileIcon = ResourceTool::GetImageIcon(TEXT("AudioFileIcon"));

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		SetWindowFlag(ImGuiWindowFlags_NoScrollbar);

		ProjectManager::OnLoadProject().AddCallback(BIND_EVENT_FN(ContentBrowserPanel::OnLoadProject));
	}

	void ContentBrowserPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		DrawTopBar();

		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX;

		if (ImGui::BeginTable("ContentBrowser", 2, tableFlags))
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			
			DrawDirectoryBrowser();

			ImGui::TableSetColumnIndex(1);
			
			DrawAssetBrowser();

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
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

				const WString& PathName = it.GetPathName();

				if (PathName == CurrentDirectory)
					flags |= ImGuiTreeNodeFlags_Selected;

				ImGui::PushID(idx++);

				CString name = ICON_FA_FOLDER "  ";
				name += TypeUtils::FromUTF16(it.GetName());

				bool isOpen = ImGui::TreeNodeEx(&idx, flags, name.c_str());

				if (ImGui::BeginDragDropTarget())
				{
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery);

					if (payload && payload->IsDelivery())
					{
						PanelDragPayload::ContentBrowserItem Item;
						Item.FromData((uint8_t*)payload->Data);

						if (Item.ItemType != AssetType::Folder)
						{
							AssetManager::MoveAsset(AssetManager::GetAsset(Item.GetID()), it.GetPathName());
							Reload();
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
				{
					ChangeCurrentDirectory(PathName);
				}

				if (isOpen)
				{
					DrawDirectoryTree(PathName);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}
		}
	}

	void ContentBrowserPanel::DrawDirectoryBrowser()
	{
		ImGui::BeginChild("##directory_browser");
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow;

			if (CurrentDirectory.empty())
				flags |= ImGuiTreeNodeFlags_Selected;

			CString ProjectName = LoadedProject ? LoadedProject->Name : "Project";
			CString NodeName = ICON_FA_FOLDER_PLUS "  ";
			NodeName += ProjectName;

			bool isOpen = ImGui::TreeNodeEx(NodeName.c_str(), flags);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery);

				if (payload && payload->IsDelivery())
				{
					PanelDragPayload::ContentBrowserItem Item;
					Item.FromData((uint8_t*)payload->Data);

					if (Item.ItemType != AssetType::Folder)
					{
						AssetManager::MoveAsset(AssetManager::GetAsset(Item.GetID()), L"");
						Reload();
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked())
			{
				ChangeCurrentDirectory(L"");
			}

			if (isOpen)
			{
				DrawDirectoryTree(L"");
				ImGui::TreePop();
			}
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::DrawAssetBrowser()
	{
		ImGui::BeginChild("##asset_browser", ImGui::GetContentRegionAvail());
		{
			ImGui::SameLine(0.f, 5.f);

			m_TextFilter.Draw(ICON_FA_MAGNIFYING_GLASS "  Search", ImGui::GetWindowWidth() * 0.25f);

			ImGui::Separator();
			ImGui::Spacing();

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
				if (ImGui::MenuItem("New Scene"))
				{
					CreateContent(AssetType::Scene);
				}
				if (ImGui::MenuItem("New C# Script"))
				{
					CreateContent(AssetType::Script);
				}

				ImGui::EndPopup();
			}

			static const float padding = 10.f;
			static const float cellWidth = BrowserContent::GetSize().x + padding;

			const float browserWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = (int)(browserWidth / cellWidth);

			if (columnCount < 1)
				columnCount = 1;

			bShouldReload = false;

			std::vector<BrowserContent>* DefaultContentList = ContentList;

			ImGui::Columns(columnCount, "##asset", false);

			ImGuiWindow* window = GImGui->CurrentWindow;

			for (size_t idx = 0; idx < DefaultContentList->size(); idx++)
			{
				BrowserContent& content = (*DefaultContentList)[idx];

				if (!content.bShouldRename && content.bIsCreated)
				{
					CString cName = TypeUtils::FromUTF16(content.Entry.GetName());

					if (m_TextFilter.PassFilter(cName.c_str()))
					{
						content.Draw(idx);
						ImGui::ItemSize(ImVec2(0.f, 20.f));

						ImGui::NextColumn();
					}
				}
				else
				{
					content.Draw(idx);
					ImGui::ItemSize(ImVec2(0.f, 20.f));

					ImGui::NextColumn();
				}
			}

			ImGui::Columns(1);

			if (bShouldReload)
			{
				delete DefaultContentList;
				bShouldReload = false;
			}
		}
		ImGui::EndChild();
	}

	void ContentBrowserPanel::DrawTopBar()
	{
		ImGui::Spacing();

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::SetCursorPosX(5.f);

		if (ImGui::Button(ICON_FA_ROTATE_LEFT))
		{
			if (!CurrentDirectory.empty())
			{
				ChangeCurrentDirectory(CurrentDirectory.parent_path());
			}
		}

		ImGui::SameLine(0.f, 5.f);
		if (ImGui::Button(ICON_FA_ROTATE_RIGHT))
		{
			ChangeCurrentDirectory(LastDirectory);
		}
		ImGui::SameLine(0.f, 15.f);

		if (ImGui::Button(ICON_FA_FILE_IMPORT "  Import"))
		{
			nfdchar_t* outPath = nullptr;
			nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

			if (result == NFD_OKAY)
			{
				std::filesystem::path FPath = outPath;
				free(outPath);

				if (!std::filesystem::is_directory(FPath))
				{
					Serializer::CreateAssetFromFile(FPath, CurrentDirectory);
					Reload();
				}
			}
			else if (result == NFD_CANCEL)
			{
				DE_INFO(FileDialog, "User cancelled file dialog");
			}
			else
			{
				DE_INFO(FileDialog, "Error: {0}", NFD_GetError());
			}
		}

		ImGui::SameLine(0.f, 5.f);

		if (ImGui::Button(ICON_FA_GEARS "  Compile"))
		{
			ProjectTools::CompileScriptProject();
		}

		ImGui::SameLine(0.f, 5.f);

		if (ImGui::Button(ICON_FA_FLOPPY_DISK "  Save"))
		{
			Scene* ActiveScene = EditorSceneManager::GetEditorScene();
			if (ActiveScene && !EditorTool::IsSimulating())
			{
				Ref<AssetMetadata> data = AssetMetadata::Create();

				SceneSerializer::Serialize(ActiveScene, data);

				if (AssetManager::AssetExists(CurrentDirectory, ActiveScene->GetName()))
				{
					Ref<Asset> SceneAsset = AssetManager::LoadAsset(CurrentDirectory, ActiveScene->GetName());
					SceneAsset->Metadata = data;

					AssetManager::SaveAsset(SceneAsset);
				}
				else
				{
					AssetManager::CreateAsset(CurrentDirectory, ActiveScene->GetName(), AssetType::Scene, data);
				}

				Reload();
			}
		}

		ImGui::Separator();
	}

	static bool ContentButton(int ContentIndex, ImTextureID TextureID, const ImVec2& Size, std::function<void()> ContentRender, bool ButtonColorBehavior = true)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		const ImGuiID& id = window->GetID(ContentIndex);

		const ImGuiStyle& style = ImGui::GetStyle();

		ImVec2& CursorPos = window->DC.CursorPos;

		ImVec2 max(CursorPos.x + Size.x, CursorPos.y + Size.y);

		const ImRect ButtonRect(CursorPos, max);

		ImVec4 color = style.Colors[ImGuiCol_Button];

		bool pressed = false;

		if (ButtonColorBehavior)
		{
			bool hovered = false, held = false;
			pressed = ImGui::ButtonBehavior(ButtonRect, id, &hovered, &held);

			if (held)
			{
				color = style.Colors[ImGuiCol_ButtonActive];
			}
			else if (hovered)
			{
				color = style.Colors[ImGuiCol_ButtonHovered];
			}
		}

		ImDrawList* draw_list = window->DrawList;

		draw_list->AddRectFilled(ButtonRect.Min, ButtonRect.Max, ImGui::GetColorU32(color), 5.f, ImDrawFlags_RoundCornersAll);

		// Draw Icon
		{
			ImVec2 ImageSize = ImVec2(Size.x - 10.f, (Size.y * 2.f / 3.f) - 10.f);

			ImVec2 p_min = ImVec2(ButtonRect.Min.x + 5.f, ButtonRect.Min.y + 5.f);
			ImVec2 p_max = ImVec2(p_min.x + ImageSize.x, p_min.y + ImageSize.y);

			draw_list->AddImage(
				TextureID,
				p_min,
				p_max,
				{ 0.f, 1.f }, { 1.f, 0.f }
			);

			CursorPos = ImVec2(p_min.x, p_max.y + 5.f);
		}

		// Draw Separator
		{
			ImVec2 p1 = ImVec2(ButtonRect.Min.x, CursorPos.y + 5.f);
			ImVec2 p2 = ImVec2(ButtonRect.Max.x, p1.y);

			draw_list->AddLine(
				p1,
				p2,
				ImGui::GetColorU32(style.Colors[ImGuiCol_Separator])
			);

			CursorPos = ImVec2(p1.x + 5.f, p1.y + 5.f);
		}

		// Draw Contents
		ContentRender();

		ImGui::ItemSize(ButtonRect);
		ImGui::ItemAdd(ButtonRect, id);

		CursorPos = ImVec2(ButtonRect.Min.x, ButtonRect.Max.y);

		return pressed;
	}

	static void ContentDrag(const char* Label, ImTextureID TextureID, const ImVec2& Size)
	{
		ImGui::BeginChild("##content_drag", Size);

		ImVec2 ImageSize = ImVec2(Size.x - 10.f, (Size.y * 2.f / 3.f) - 10.f);

		// Draw Icon
		{
			ImGui::SetCursorPos(ImVec2(5.f, 5.f));

			ImGui::Image(
				TextureID,
				ImageSize,
				{ 0.f, 1.f }, { 1.f, 0.f }
			);
		}
		
		// Draw Text
		{
			ImGui::SetCursorPos(ImVec2(5.f, ImageSize.y + 10.f));

			ImGui::Text(Label);
		}

		ImGui::EndChild();
	}

	void ContentBrowserPanel::BrowserContent::Draw(int ContentIndex)
	{
		if (!ContentIcon)
		{
			switch (Type)
			{
			case AssetType::Folder:
				ContentIcon = DirectoryIcon;
				break;
			case AssetType::Scene:
				ContentIcon = SceneFileIcon;
				break;
			case AssetType::Script:
				ContentIcon = ScriptFileIcon;
				break;
			case AssetType::Audio:
				ContentIcon = AudioFileIcon;
				break;
			case AssetType::Texture:
			{
				ContentIcon = TextureManager::LoadTexture(Entry.GetID());
			} break;
			default:
				ContentIcon = FileIcon;
				break;
			}
		}

		if (bIsCreated)
		{
			ContentButton(ContentIndex, (ImTextureID)ContentIcon->GetRendererID(), Size, [&]() {
				DrawCreatedContent();
			}, !bShouldRename);

			if (!bShouldRename)
			{
				if (ImGui::BeginPopupContextItem())
				{
					if (Type != AssetType::Script)
					{
						if (ImGui::MenuItem("Rename"))
						{
							StartRename();
						}
					}
					if (ImGui::MenuItem("Remove"))
					{
						RunEvent(ContentEvent::Delete);
						ImGui::EndPopup();
						return;
					}

					ImGui::EndPopup();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					RunEvent(ContentEvent::Open);
				}

				if (Type == AssetType::Folder)
				{
					if (ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery);

						if (payload)
						{
							if (payload->IsPreview())
							{
								ImGui::GetForegroundDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
							}
							if (payload->IsDelivery())
							{
								PanelDragPayload::ContentBrowserItem Item;
								Item.FromData((uint8_t*)payload->Data);

								if (Item.ItemType != AssetType::Folder)
								{
									AssetManager::MoveAsset(AssetManager::GetAsset(Item.GetID()), Entry.GetPathName());
									RunEvent(ContentEvent::Move);
								}
							}
						}

						ImGui::EndDragDropTarget();
					}
				}
				else
				{
					if (ImGui::BeginDragDropSource())
					{
						PanelDragPayload::ContentBrowserItem Item;

						if (Type == AssetType::Folder)
						{
							Item.SetFolder(Entry.GetPath().c_str(), Entry.GetName().c_str());
						}
						else
						{
							Item.SetAsset(Entry.GetID().c_str());
							Item.ItemType = Entry.GetType();
						}

						size_t size = 0;
						uint8_t* data = Item.Data(size);

						ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", data, size);

						CString cName = TypeUtils::FromUTF16(Entry.GetName());
						ContentDrag(cName.c_str(), (ImTextureID)ContentIcon->GetRendererID(), Size);

						delete[] data;

						ImGui::EndDragDropSource();
					}
				}
			}
		}
		else
		{
			ContentButton(ContentIndex, (ImTextureID)ContentIcon->GetRendererID(), Size, [&]() {
				DrawUncreatedContent();
			}, false);
		}
	}

	void ContentBrowserPanel::BrowserContent::DrawUncreatedContent()
	{
		bool isDirectory = (Type == AssetType::Folder);

		if (!RenameBuffer)
		{
			RenameBuffer = new char[RenameBufferSize];
			memset(RenameBuffer, 0, RenameBufferSize);
		}

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::PushItemWidth(Size.x - 10.f);

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
		ImGui::PushStyleColor(ImGuiCol_Border, bIsError ? ImVec4(1.f, 0.f, 0.f, 1.f) : style.Colors[ImGuiCol_ButtonHovered]);

		ImGui::InputText("##name", RenameBuffer, RenameBufferSize);

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGui::PopItemWidth();

		ImGui::SetKeyboardFocusHere(-1);

		if (
			!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_Enter)
		){
			bIsError = !RunEvent(ContentEvent::Create);

			if (!bIsError)
			{
				bIsCreated = true;

				delete[] RenameBuffer;
				RenameBuffer = nullptr;
			}
		}
	}

	void ContentBrowserPanel::BrowserContent::DrawCreatedContent()
	{
		CString cName = TypeUtils::FromUTF16(Entry.GetName());
		bool lastRenameState = bShouldRename;

		if (bShouldRename)
		{
			ImGuiStyle& style = ImGui::GetStyle();

			ImGui::PushItemWidth(Size.x - 10.f);

			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
			ImGui::PushStyleColor(ImGuiCol_Border, bIsError ? ImVec4(1.f, 0.f, 0.f, 1.f) : style.Colors[ImGuiCol_ButtonHovered]);

			ImGui::InputText("##name", RenameBuffer, RenameBufferSize, ImGuiInputTextFlags_AutoSelectAll);

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			ImGui::PopItemWidth();

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
	}

	bool ContentBrowserPanel::BrowserContent::RunEvent(ContentEvent CEvent)
	{
		return OnEventCallback(*this, CEvent);
	}

	void ContentBrowserPanel::BrowserContent::StartRename()
	{
		if (!bShouldRename)
		{
			bShouldRename = true;
			size_t BufferSize = RenameBufferSize + 1;

			RenameBuffer = new char[BufferSize];

			memset(RenameBuffer, 0, BufferSize);

			CString str = TypeUtils::FromUTF16(Entry.GetName());

			strcpy(RenameBuffer, str.c_str());
		}
	}

	void ContentBrowserPanel::BrowserContent::StopRename()
	{
		if (bShouldRename)
		{
			bShouldRename = false;

			RunEvent(ContentEvent::Rename);

			delete[] RenameBuffer;
		}
	}

	void ContentBrowserPanel::CreateContent(AssetType type)
	{
		BrowserContent content = BrowserContent();
		content.bIsCreated = false;
		content.Type = type;
		content.OnEvent(BIND_CLASS_FN(OnContentEvent));

		ContentList->push_back(content);
	}

	void ContentBrowserPanel::ChangeCurrentDirectory(const std::filesystem::path& NewDirectory)
	{
		std::filesystem::path path = CurrentDirectory;
		CurrentDirectory = NewDirectory;
		LastDirectory = path;

		Reload();
	}

	bool ContentBrowserPanel::OnContentEvent(BrowserContent& Content, BrowserContent::ContentEvent CEvent)
	{
		switch (CEvent)
		{
		case BrowserContent::ContentEvent::Create:
		{
			if (OnCreateContent(&Content))
			{
				Reload();
				return true;
			}

			return false;
		} break;
		case BrowserContent::ContentEvent::Delete:
		{
			DirectoryEntry& Entry = Content.Entry;
			bool isDirectory = Content.Type == AssetType::Folder;

			if (isDirectory)
			{
				AssetManager::RemoveFolder(Entry.GetPath(), Entry.GetName());
				Reload();
			}
			else
			{
				AssetManager::RemoveAsset(AssetManager::GetAsset(Entry.GetPath(), Entry.GetName()));
				Reload();
			}

			return true;
		} break;
		case BrowserContent::ContentEvent::Rename:
		{
			if (OnRenameContent(&Content))
			{
				Reload();
				return true;
			}

			return false;
		} break;
		case BrowserContent::ContentEvent::Open:
		{
			switch (Content.Type)
			{
			case AssetType::Folder:
			{
				ChangeCurrentDirectory(CurrentDirectory / Content.Entry.GetName());
				return true;
			} break;
			case AssetType::Script:
			{
				ProjectTools::OpenScriptProject();
				return true;
			} break;
			default:
				return false;
			}
		} break;
		case BrowserContent::ContentEvent::Move:
		{
			Reload();
			return true;
		} break;
		default:
			break;
		}

		return false;
	}

	bool ContentBrowserPanel::OnRenameContent(BrowserContent* Content)
	{
		DirectoryEntry& Entry = Content->Entry;
		WString NewName = TypeUtils::FromUTF8(Content->RenameBuffer);

		switch (Content->Type)
		{
		case AssetType::Folder:
		{
			return AssetManager::RenameFolder(Entry.GetPath(), Entry.GetName(), NewName);
		} break;
		case AssetType::Scene:
		{
			Ref<Asset> SceneAsset = AssetManager::LoadAsset(Entry.GetID());

			if (AssetManager::RenameAsset(SceneAsset, NewName))
			{
				// Rename Scene
				EditorScene* NewScene = new EditorScene(NewName);
				SceneSerializer::Deserialize(NewScene, SceneAsset->Metadata);
				NewScene->SetName(NewName);

				Ref<AssetMetadata> SceneData = AssetMetadata::Create();
				SceneSerializer::Serialize(NewScene, SceneData);

				delete NewScene;

				SceneAsset->Metadata = SceneData;
				return AssetManager::SaveAsset(SceneAsset);
			}

			return false;
		} break;
		default:
			return AssetManager::RenameAsset(AssetManager::GetAsset(Entry.GetID()), NewName);
		}

		return false;
	}

	bool ContentBrowserPanel::OnCreateContent(BrowserContent* Content)
	{
		switch (Content->Type)
		{
		case AssetType::Folder:
		{
			WString FolderName = TypeUtils::FromUTF8(Content->RenameBuffer);
			return AssetManager::CreateFolder(CurrentDirectory, FolderName);
		}
		case AssetType::Script:
		{
			CString ScriptName = Content->RenameBuffer;
			if (ProjectManager::CreateScript(ScriptName))
			{
				Ref<AssetMetadata> data = AssetMetadata::Create();
				data->GetFields().SetStringField("Name", ScriptName);

				return AssetManager::CreateAsset(
					CurrentDirectory,
					TypeUtils::FromUTF8(ScriptName),
					AssetType::Script,
					data
				) != nullptr;
			}

			return false;
		}
		case AssetType::Scene:
		{
			WString SceneName = TypeUtils::FromUTF8(Content->RenameBuffer);
			EditorScene* NewScene = new EditorScene(SceneName);

			Ref<AssetMetadata> data = AssetMetadata::Create();
			SceneSerializer::Serialize(NewScene, data);

			Ref<Asset> result = AssetManager::CreateAsset(
				CurrentDirectory,
				SceneName,
				AssetType::Scene,
				data
			);

			delete NewScene;
			return result != nullptr;
		}
		default:
			return false;
		}
	}

	void ContentBrowserPanel::Reload()
	{
		AssetIterator it(CurrentDirectory);

		ContentList = new std::vector<BrowserContent>();

		for (auto& content : it)
		{
			BrowserContent data(content);
			data.OnEvent(BIND_CLASS_FN(OnContentEvent));

			ContentList->push_back(data);
		}

		bShouldReload = true;
	}

	void ContentBrowserPanel::OnLoadProject(const Project& project)
	{
		LoadedProject = &project;
		Reload();
	}
}