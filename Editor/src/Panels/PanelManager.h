#pragma once

#include "Engine/Types/Types.h"
#include "EditorPanel.h"
#include "Engine/Events/Event.h"

#include <vector>

namespace Engine
{
	class PanelManager
	{
	public:
		PanelManager() = default;

		~PanelManager();

		void AddPanel(EditorPanel* panel);
		void AddPanels(std::initializer_list<EditorPanel*> panels);

		void RemovePanel(const std::string& PanelName);

		void RemoveAllPanels();

		void Update(float DeltaTime);
		void Render(float DeltaTime);

		void AddData(const CString& Name, void* Data, size_t size);
		void RemoveData(const CString& name);
		void ClearData();

		void OnEvent(Event& event);

		struct PanelData
		{
			CString name = CString();
			void* Data = nullptr;
			size_t size = 0;
		};

		PanelData& GetData(const CString& name);

	private:
		std::vector<EditorPanel*> Panels;

		std::vector<PanelData> DataList;
	};
}
