#pragma once

#include "Core.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Containers/String.hpp"

namespace ME::Render
{
	class CommandBuffer;
}

struct ImGuiContext;

namespace ME::Core
{
	class COREAPI Layer
	{
	public:
		Layer(const ME::Core::String& name = TEXT("Layer"));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float64 deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) {}
		virtual void OnEvent(ME::Core::Event& event) {}

		inline const ME::Core::String& GetName() const { return m_DebugName; }

	protected:
		ME::Core::String m_DebugName;
	};

}