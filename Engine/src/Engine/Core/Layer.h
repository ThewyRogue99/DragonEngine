#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	class Layer
	{
	public:
		ENGINE_API Layer(const WString& Name = TEXT("Layer"));
		ENGINE_API virtual ~Layer();
		
		ENGINE_API virtual void OnAttach();
		ENGINE_API virtual void OnDetach();
		ENGINE_API virtual void OnUpdate(float DeltaTime);
		ENGINE_API virtual void OnEvent(Event& event);
		
		ENGINE_API const WString& GetName() const;

	protected:
		WString DebugName;
	};
}
