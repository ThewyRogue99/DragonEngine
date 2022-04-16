#pragma once

#include "Layer.h"

namespace Engine
{
	class ENGINE_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		void Clear();

		inline std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		inline std::vector<Layer*>::iterator end() { return Layers.end(); }

	private:
		std::vector<Layer*> Layers;
		std::vector<Layer*>::iterator LayerInsert;
	};
}
