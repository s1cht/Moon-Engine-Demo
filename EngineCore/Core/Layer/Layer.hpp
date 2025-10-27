#pragma once

#include "Core.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Containers/String/String.hpp"

namespace ME::Render
{
	class CommandBuffer;
}

struct ImGuiContext;

namespace ME::Core
{
	class CORE_API Layer
	{
	public:
		Layer(const ME::Core::Containers::String& name = TEXT("Layer"));
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float64 deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) {}
		virtual void OnEvent(ME::Core::Event& event) {}

		inline const ME::Core::Containers::String& GetName() const { return m_DebugName; }

	protected:
		ME::Core::Containers::String m_DebugName;

	};

}