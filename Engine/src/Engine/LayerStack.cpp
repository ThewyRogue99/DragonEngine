#include "depch.h"

#include "LayerStack.h"

namespace Engine
{
	LayerStack::LayerStack()
	{
		LayerInsert = Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		LayerInsert = Layers.emplace(LayerInsert, layer);
		layer->OnAttach();
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		Layers.emplace_back(overlay);
		overlay->OnAttach();
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(Layers.begin(), Layers.end(), layer);

		if (it != Layers.end())
		{
			Layers.erase(it);
			LayerInsert--;
			layer->OnDetach();
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = std::find(Layers.begin(), Layers.end(), overlay);

		if (it != Layers.end())
		{
			Layers.erase(it);
			overlay->OnDetach();
		}
	}
}