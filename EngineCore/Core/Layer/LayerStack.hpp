#pragma once

#include "Core.hpp"
#include "Layer.hpp"

namespace Pawn::Core
{
	class CORE_API LayerStack
	{
	private:
		using Iterator = Pawn::Core::Containers::Array<Layer*>::Iterator;

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
		Pawn::Core::Containers::Array<Layer*> m_Layers;
		SIZE_T m_LayerInsertIndex;

	};
}