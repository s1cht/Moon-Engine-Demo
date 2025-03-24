#pragma once

#include "Core.h"
#include "Core/Containers/String.h"
#include "Core/Events/Event.h"

namespace Pawn
{
	class CORE_API Layer
	{
	public:
		Layer(const String& name = TEXT("Layer"));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float64 deltaTime) {}
		virtual void OnRender(float64 deltaTime) {}
		virtual void OnImGuiRender(float64 deltaTime) {}
		virtual void OnEvent(Event& event) {}

		inline const String& GetName() const { return m_DebugName; }

	protected:
		String m_DebugName;

	};

}