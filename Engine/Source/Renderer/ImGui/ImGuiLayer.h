#pragma once

#include <Core.h>
#include <Core/Layer/Layer.h>

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
		void OnImGuiRender(float64 deltaTime) override;


	public:
		void BeginRender();
		void EndRender();
		void PostRender();

		void Shutdown();
		void EnableEvents(bool enabled) { m_EnabledEvents = enabled; }

	private:
		bool m_Disabled = false;
		bool m_EnabledEvents = true;

	};

}