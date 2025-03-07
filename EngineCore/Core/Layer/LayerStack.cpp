#include "pch.h"
#include "LayerStack.h"
#include "Core/Containers/Algorithm.h"

namespace Pawn
{
	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_LayerInsert = m_Layers.Emplace(m_LayerInsert, layer);
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
			m_LayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay)
	{
		auto it = Pawn::Algorithm::Find(m_Layers.Begin(), m_Layers.End(), overlay);
		if (it != m_Layers.End())
			m_Layers.Erase(it);
	}
}