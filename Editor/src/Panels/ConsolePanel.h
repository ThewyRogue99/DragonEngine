#pragma once

#include "EditorPanel.h"

namespace Engine
{
	class ConsolePanel : public EditorPanel
	{
	public:
		ConsolePanel();

		virtual void OnRender(float DeltaTime) override;

	private:
		char buffer[2048] = { };
	};
}