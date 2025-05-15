#pragma once

#include "Core.hpp"

struct ImGuiContext;

namespace Pawn::Core
{
	class CORE_API Layer
	{
	public:
		Layer(const Pawn::Core::Containers::String& name = TEXT("Layer"));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float64 deltaTime) {}
		virtual void OnRender(float64 deltaTime) {}
		virtual void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) {}
		virtual void OnEvent(Pawn::Core::Event& event) {}

		inline const Pawn::Core::Containers::String& GetName() const { return m_DebugName; }

	protected:
		Pawn::Core::Containers::String m_DebugName;

	};

}