#pragma once

#include <Core.hpp>
#include <Core/Layer/Layer.hpp>
#include "ImGuiReferences.h"

namespace ME::Render::Imgui
{
	class MOON_API ImGuiLayer : public ME::Core::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

	public:
		void OnAttach() override;
		void OnDetach() override;

		void OnEvent(ME::Core::Event& event) override;
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

	private:
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_RenderPass;
		ME::Core::Memory::Reference<ME::Render::ResourceHandler> m_ResourceHandler;

	private:
		bool m_Disabled = false;
		bool m_EnabledEvents = true;
	};

}