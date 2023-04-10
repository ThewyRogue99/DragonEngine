#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Renderer/Texture.h"

#include "Editor/Project/ProjectManager.h"
#include "Editor/Panels/EditorPanel.h"
#include "Engine/Asset/AssetManager.h"

#include <filesystem>
#include <vector>

namespace Engine
{
	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnCreate() override;

		virtual void OnRender(float DeltaTime) override;

	private:
		void DrawDirectoryTree(const std::filesystem::path& DirectoryPath);

		void DrawDirectoryBrowser();
		void DrawAssetBrowser();

		void DrawTopBar();

		void Reload();

		void OnLoadProject(const Project& project);

		void CreateContent(AssetType type);

		void ChangeCurrentDirectory(const std::filesystem::path& NewDirectory);

	private:
		class BrowserContent
		{
		public:
			BrowserContent() = default;

			BrowserContent(const DirectoryEntry& entry)
				: Entry(entry), Type(entry.GetType()) { }

			void Draw(int ContentIndex);

			void StartRename();
			void StopRename();

			enum class ContentEvent;

			inline void OnEvent(std::function<bool(BrowserContent&, ContentEvent)> Callback)
				{ OnEventCallback = Callback; }

			static ImVec2 GetSize() { return Size; }

			friend class ContentBrowserPanel;

		public:
			enum class ContentEvent
			{
				Create,
				Delete,
				Rename,
				Open,
				Move
			};

		private:
			void DrawUncreatedContent();
			void DrawCreatedContent();

			bool RunEvent(ContentEvent CEvent);

		private:
			DirectoryEntry Entry;

			AssetType Type = AssetType::Undefined;

			bool bShouldRename = false;
			bool bIsCreated = true;
			bool bIsError = false;

			static const ImVec2 Size;

			char* RenameBuffer = nullptr;
			static const size_t RenameBufferSize = 256;

			Ref<Texture2D> ContentIcon = nullptr;

			std::function<bool(BrowserContent&, ContentEvent)> OnEventCallback = nullptr;
		};

	private:
		bool OnContentEvent(BrowserContent& Content, BrowserContent::ContentEvent CEvent);

		bool OnCreateContent(BrowserContent* Content);
		bool OnRenameContent(BrowserContent* Content);

	private:
		bool bShouldReload = false;

		std::filesystem::path CurrentDirectory = L"";
		std::filesystem::path LastDirectory = L"";

		const Project* LoadedProject = nullptr;

		std::vector<BrowserContent>* ContentList = nullptr;

		ImGuiTextFilter m_TextFilter;
	};
}
