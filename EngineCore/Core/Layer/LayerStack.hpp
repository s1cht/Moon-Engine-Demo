#pragma once

#include "Core.hpp"
#include "Layer.hpp"
#include "Core/Containers/Array.hpp"

namespace ME::Core
{
	class COREAPI LayerStack
	{
	private:
		using Iterator = ME::Core::Array<Layer*>::Iterator;

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
		ME::Core::Array<Layer*> m_Layers;
		SIZE_T m_LayerInsertIndex;

	};
}