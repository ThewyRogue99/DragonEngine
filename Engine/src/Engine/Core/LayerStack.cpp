#include "depch.h"
#include "LayerStack.h"

namespace Engine
{
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		Clear();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		Layers.emplace(Layers.begin() + LayerInsertIndex, layer);

		layer->OnAttach();

		LayerInsertIndex++;
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

			layer->OnDetach();

			LayerInsertIndex--;
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

	void LayerStack::Clear()
	{
		if (Layers.size() > 0)
		{
			for (auto it = end(); it != begin();)
			{
				(*--it)->OnDetach();
				delete (*it);
			}

			Layers.clear();
		}
	}
}