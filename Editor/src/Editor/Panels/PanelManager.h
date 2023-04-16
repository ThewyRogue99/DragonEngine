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

		void AddPanel(const WString& PanelName, EditorPanel* Panel);
		void AddPanels(std::initializer_list<std::pair<WString, EditorPanel*>> Panels);

		EditorPanel* GetPanel(const WString& PanelName);

		bool RemovePanel(const WString& PanelName);
		void RemoveAllPanels();

		bool DisablePanel(const WString& PanelName);
		void DisableAllPanels();

		bool ActivatePanel(const WString& PanelName);
		void ActivateAllPanels();

		void Update(float DeltaTime);
		void Render(float DeltaTime);

		void AddData(const WString& Name, void* Data, size_t size);

		void OnEvent(Event& event);

	private:
		std::unordered_map<WString, EditorPanel*> PanelMap;
	};
}
