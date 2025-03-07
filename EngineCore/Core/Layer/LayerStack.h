#pragma once

#include "Layer.h"
#include "Core.h"
#include "Core/Containers/Array.h"

namespace Pawn
{
	class CORE_API LayerStack
	{
	private:
		using Iterator = Array<Layer*>::Iterator;

	public:
		LayerStack();
		~LayerStack();

	public:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		Iterator begin() { return Begin(); }
		Iterator Begin() { return m_Layers.Begin(); }

		Iterator end() { return End(); }
		Iterator End() { return m_Layers.End(); }

	private:
		Array<Layer*> m_Layers;
		Iterator m_LayerInsert;

	};
}