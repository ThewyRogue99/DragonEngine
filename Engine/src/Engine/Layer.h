#pragma once

namespace Engine
{
	class ENGINE_API Layer
	{
	public:
		Layer(const std::string& Name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate() { }
		virtual void OnImGuiRender() { }
		virtual void OnEvent(Event& event) { }

		inline const std::string& GetName() const { return DebugName; }

	protected:
		std::string DebugName;
	};
}
