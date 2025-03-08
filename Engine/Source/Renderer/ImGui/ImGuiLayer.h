#pragma once

#include "Core/Layer/Layer.h"

namespace Pawn::Render::Imgui
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

	public:
		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;

	public:
		void BeginRender();
		void EndRender();

	private:
		bool m_Disabled = false;

	};

}