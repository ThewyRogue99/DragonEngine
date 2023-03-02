#include "depch.h"
#include "EditorPanel.h"

#include "PanelManager.h"

#pragma warning(disable : 4244)

namespace Engine
{
	EditorPanel::EditorPanel(const std::string& Name, uint32_t Width, uint32_t Height)
		: PanelName(Name), PanelSize(Width, Height)
	{

	}

	EditorPanel::~EditorPanel()
	{
		
	}

	void EditorPanel::OnCreate()
	{

	}

	void EditorPanel::OnDelete()
	{

	}

	void EditorPanel::OnEvent(Event& event)
	{

	}

	void EditorPanel::ClosePanel()
	{
		bIsOpen = false;
	}

	void EditorPanel::OpenPanel()
	{
		bIsOpen = true;
	}

	void EditorPanel::SetFocus()
	{
		ImGui::SetWindowFocus(PanelName.c_str());
	}

	void EditorPanel::OnUpdate(float DeltaTime)
	{

	}

	void EditorPanel::OnBeginRender()
	{

	}

	void EditorPanel::OnRender(float DeltaTime)
	{
		
	}

	ImVec2 EditorPanel::GetPanelRelativeMousePos()
	{
		auto [mx, my] = ImGui::GetMousePos();

		auto firstBound = GetPanelBound(0);
		auto secondBound = GetPanelBound(1);

		mx -= firstBound.x;
		my -= firstBound.y;

		ImVec2 size = { secondBound.x - firstBound.x, secondBound.y - firstBound.y };

		my = size.y - my;

		int MouseX = (int)mx;
		int MouseY = (int)my;

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)size.x && MouseY < (int)size.y)
		{
			return ImVec2((float)MouseX, (float)MouseY);
		}
		
		return ImVec2(-1.f, -1.f);
	}

	void* EditorPanel::GetData(const CString& name)
	{
		return manager->GetData(name).Data;
	}

	void EditorPanel::AddData(const CString& name, void* Data, size_t size)
	{
		manager->AddData(name, Data, size);
	}

	void EditorPanel::OnData(const CString& Name, void* Data, size_t size)
	{

	}

	void EditorPanel::OnResize(uint32_t width, uint32_t height)
	{

	}

	void EditorPanel::SetPanelStyleVar(ImGuiStyleVar style, ImVec2 value)
	{
		PanelStyles.push_back({ style, value });
	}

	void EditorPanel::SetPanelStyleColor(ImGuiCol style, ImVec4 value)
	{
		PanelColors.push_back({ style, value });
	}

	ImVec2 EditorPanel::GetPanelBound(uint8_t index)
	{
		if (index > 1 || index < 0)
			return ImVec2(-1.f, -1.f);

		return PanelBounds[index];
	}

	void EditorPanel::Render(float DeltaTime)
	{
		if (bIsOpen)
		{
			for (auto& style : PanelStyles)
				ImGui::PushStyleVar(style.style, style.value);

			for (auto& color : PanelColors)
				ImGui::PushStyleColor(color.style, color.value);

			ImGui::BeginDisabled(IsDisabled);
			if (ImGui::Begin(PanelName.c_str(), bShowCloseButton ? &bIsOpen : nullptr, WindowFlag))
			{
				if (bBeginRender)
				{
					OnBeginRender();
					bBeginRender = false;
				}

				PanelPosition = ImGui::GetWindowPos();

				ImVec2 PanelMinRegion = ImGui::GetWindowContentRegionMin();
				ImVec2 PanelMaxRegion = ImGui::GetWindowContentRegionMax();

				PanelBounds[0] = { PanelMinRegion.x + PanelPosition.x, PanelMinRegion.y + PanelPosition.y };
				PanelBounds[1] = { PanelMaxRegion.x + PanelPosition.x, PanelMaxRegion.y + PanelPosition.y };

				ImVec2 CurrentSize = ImGui::GetWindowSize();

				if (CurrentSize.x != PanelSize.x || CurrentSize.y != PanelSize.y)
					OnResize((uint32_t)CurrentSize.x, (uint32_t)CurrentSize.y);

				PanelSize = CurrentSize;
				PanelAvailableSize = ImGui::GetContentRegionAvail();

				OnRender(DeltaTime);
			}
			else
			{
				bBeginRender = true;
			}
			ImGui::End();
			ImGui::EndDisabled();

			ImGui::PopStyleVar((int)PanelStyles.size());
			ImGui::PopStyleColor((int)PanelColors.size());
		}
		else
		{
			bBeginRender = true;
		}
	}
}