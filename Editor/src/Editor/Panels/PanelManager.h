#pragma once

#include "Engine/Types/Types.h"
#include "EditorPanel.h"
#include "Engine/Events/Event.h"

#include <unordered_map>

namespace Engine
{
	class PanelManager
	{
	public:
		PanelManager() = default;

		~PanelManager();

		void AddPanel(const CString& PanelName, EditorPanel* Panel);
		void AddPanels(std::initializer_list<std::pair<CString, EditorPanel*>> Panels);

		EditorPanel* GetPanel(const CString& PanelName);

		bool RemovePanel(const CString& PanelName);
		void RemoveAllPanels();

		bool DisablePanel(const CString& PanelName);
		void DisableAllPanels();

		bool ActivatePanel(const CString& PanelName);
		void ActivateAllPanels();

		void Update(float DeltaTime);
		void Render(float DeltaTime);

		void AddData(const CString& Name, void* Data, size_t size);

		void OnEvent(Event& event);

	private:
		std::unordered_map<CString, EditorPanel*> PanelMap;
	};
}
