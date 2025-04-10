#pragma once

#include <Core.h>
#include <Core/Layer/Layer.h>
#include "ImGuiReferences.h"

namespace Pawn::Render::Imgui
{

	class PAWN_API ImGuiLayer : public Pawn::Core::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

	public:
		void OnAttach() override;
		void OnDetach() override;

		void OnEvent(Pawn::Core::Event& event) override;
		void OnImGuiRender(float64 deltaTime, ImGuiContext* context) override;

	public:
		void BeginRender();
		void EndRender();
		void PostRender();

		inline ImGuiContext* GetContext();

		void Shutdown();
		void EnableEvents(bool enabled) { m_EnabledEvents = enabled; }

	private:
		ImGuiContext* m_ImGuiContext;
		bool m_Disabled = false;
		bool m_EnabledEvents = true;

	};

}