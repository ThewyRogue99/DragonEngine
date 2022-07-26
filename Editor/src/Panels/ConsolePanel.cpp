#include "depch.h"
#include "ConsolePanel.h"

#include "../Debug/EditorConsole.h"
#include "Engine/Core/Log.h"

namespace Engine
{
	ConsolePanel::ConsolePanel() : EditorPanel("Console")
	{

	}

	void ConsolePanel::OnRender(float DeltaTime)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

		Ref<EditorConsole> console = std::static_pointer_cast<EditorConsole>(Log::GetConsole());

		ImVec2 size = GetPanelAvailableSize();
		size = { size.x, size.y };

		std::string str = console->GetString();

		ImGui::InputTextMultiline("##Console", (char*)str.c_str(), str.length(), size, flags);
	}
}