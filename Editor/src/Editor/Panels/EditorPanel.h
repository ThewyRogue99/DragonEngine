#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"

#include <imgui.h>
#include <vector>

namespace Engine
{
	class PanelManager;

	class EditorPanel
	{
	public:
		EditorPanel(const std::string& Name = "Panel", uint32_t Width = 1280, uint32_t Height = 720);
		~EditorPanel();

		virtual void OnCreate();

		virtual void OnUpdate(float DeltaTime);

		virtual void OnBeginRender();

		virtual void OnRender(float DeltaTime);

		virtual void OnDelete();

		virtual void OnEvent(Event& event);

		void SetShowCloseButton(bool state) { bShowCloseButton = state; }

		void ClosePanel();
		void OpenPanel();

		void SetFocus();

		ImVec2 GetPanelAvailableSize() const { return PanelAvailableSize; }
		uint32_t GetPanelAvailableWidth() const { return (uint32_t)PanelAvailableSize.x; }
		uint32_t GetPanelAvailableHeight() const { return (uint32_t)PanelAvailableSize.y; }

		ImVec2 GetSize() const { return PanelSize; }
		uint32_t GetWidth() const { return (uint32_t)PanelSize.x; }
		uint32_t GetHeight() const { return (uint32_t)PanelSize.y; }

		ImVec2 GetPanelRelativeMousePos();

		const std::string& GetName() const { return PanelName; }

		friend class PanelManager;

	protected:
		void* GetData(const CString& name);
		void AddData(const CString& name, void* Data, size_t size);

		virtual void OnData(const CString& Name, void* Data, size_t size);

		virtual void OnResize(uint32_t width, uint32_t height);

		void SetPanelStyleVar(ImGuiStyleVar style, ImVec2 value);

		void SetPanelStyleColor(ImGuiCol style, ImVec4 value);

		inline void SetWindowFlag(ImGuiWindowFlags flags) { WindowFlag = flags; }

		ImVec2 GetPanelBound(uint8_t index);

	private:
		virtual void Render(float DeltaTime);

	private:
		struct StyleVar
		{
			ImGuiStyleVar style;
			ImVec2 value;
		};

		struct StyleColor
		{
			ImGuiCol style;
			ImVec4 value;
		};

	protected:
		std::string PanelName;

		bool IsDisabled = false;
		bool bIsOpen = true;

	private:
		bool bBeginRender = true;
		bool bShowCloseButton = false;

		ImVec2 PanelPosition;
		ImVec2 PanelSize;

		ImVec2 PanelAvailableSize;

		ImVec2 PanelBounds[2];

		ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_None;
		
		std::vector<StyleVar> PanelStyles;
		std::vector<StyleColor> PanelColors;

		PanelManager* manager = nullptr;
	};
}