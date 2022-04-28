#pragma once

#include "Core/Timestep.h"

namespace Engine
{
	class ENGINE_API Layer
	{
	public:
		Layer(const std::string& Name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate(Timestep DeltaTime) { }
		virtual void OnImGuiRender(Timestep DeltaTime) { }
		virtual void OnEvent(Event& event) { }

		inline const std::string& GetName() const { return DebugName; }

	protected:
		std::string DebugName;
	};
}
