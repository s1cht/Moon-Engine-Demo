#pragma once

#include "Core/Layer/Layer.h"

namespace Pawn
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

	public:
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;

		void OnEvent(Event& event) override;

	};

}