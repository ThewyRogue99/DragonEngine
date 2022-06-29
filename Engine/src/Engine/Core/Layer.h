#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	class ENGINE_API Layer
	{
	public:
		Layer(const CString& Name = TEXT("Layer"));
		virtual ~Layer();

		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate(float DeltaTime);
		virtual void OnImGuiRender(float DeltaTime);
		virtual void OnEvent(Event& event);

		inline const CString& GetName() const { return DebugName; }

	protected:
		CString DebugName;
	};
}
