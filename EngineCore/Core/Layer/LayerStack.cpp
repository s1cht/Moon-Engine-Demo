#include "LayerStack.hpp"
#include "Core/Containers/Algorithm.hpp"

namespace Pawn::Core
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
		auto it = Pawn::Core::Algorithm::Find(m_Layers.Begin(), m_Layers.End(), layer);
		if (it != m_Layers.End())
		{
			m_Layers.Erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = Pawn::Core::Algorithm::Find(m_Layers.Begin(), m_Layers.End(), overlay);
		if (it != m_Layers.End())
			m_Layers.Erase(it);
	}
}