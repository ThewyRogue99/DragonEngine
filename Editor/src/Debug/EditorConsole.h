#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Core/Log.h"

namespace Engine
{
	class EditorConsole : public Console
	{
	public:
		EditorConsole() : Console() { }

		virtual void OnAttach();

		virtual void OnDetach();

	protected:
		virtual Ref<CoreLogger> GetCoreLogger() override;

		virtual Ref<ClientLogger> GetClientLogger() override;

	private:
		Ref<CoreLogger> m_CoreLogger = nullptr;

		Ref<ClientLogger> m_ClientLogger = nullptr;
	};
}