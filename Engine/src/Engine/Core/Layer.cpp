#include "depch.h"

#include "Layer.h"

namespace Engine
{
	Layer::Layer(const CString& Name) : DebugName(Name)
	{

	}

	Layer::~Layer()
	{

	}

	void Layer::OnAttach()
	{
		DE_LOG(Layer, "Attaching Layer: {0}", TypeUtils::FromUTF16(DebugName).c_str());
	}

	void Layer::OnDetach()
	{
		DE_LOG(Layer, "Detaching Layer: {0}", TypeUtils::FromUTF16(DebugName).c_str());
	}

	void Layer::OnUpdate(float DeltaTime)
	{

	}

	void Layer::OnEvent(Event& event)
	{

	}
}