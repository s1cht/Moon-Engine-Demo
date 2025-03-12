#include "LayerStack.h"
#include "Core/Containers/Algorithm.h"

#include <vector>

namespace Pawn
{
	LayerStack::LayerStack()
	{
		m_LayerInsertIndex = 0;
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer)
	{	
		m_Layers.Emplace(m_Layers.Begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.EmplaceBack(overlay);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = Pawn::Algorithm::Find(m_Layers.Begin(), m_Layers.End(), layer);
		if (it != m_Layers.End())
		{
			m_Layers.Erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = Pawn::Algorithm::Find(m_Layers.Begin(), m_Layers.End(), overlay);
		if (it != m_Layers.End())
			m_Layers.Erase(it);
	}
}