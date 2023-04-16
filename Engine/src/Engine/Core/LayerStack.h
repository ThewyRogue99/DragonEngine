#pragma once
#include "depch.h"

#include "Engine/Core/Core.h"
#include "Layer.h"

namespace Engine
{
	class LayerStack
	{
	public:
		ENGINE_API LayerStack();
		ENGINE_API ~LayerStack();

		ENGINE_API void PushLayer(Layer* layer);
		ENGINE_API void PushOverlay(Layer* overlay);
		ENGINE_API void PopLayer(Layer* layer);
		ENGINE_API void PopOverlay(Layer* overlay);

		ENGINE_API void Clear();

		ENGINE_API std::vector<Layer*>::iterator begin();
		ENGINE_API std::vector<Layer*>::iterator end();

	private:
		std::vector<Layer*> Layers;
		unsigned int LayerInsertIndex;
	};
}
