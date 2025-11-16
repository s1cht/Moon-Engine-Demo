#pragma once
#include <Core.hpp>
#include <Core/Layer/Layer.hpp>

#include "ImGuiReferences.hpp"
#include "Renderer/Base/RingBuffer.hpp"
#include "Renderer/RenderCommand.hpp"

namespace ME::Render::ImGui
{
	class MEAPI ImGuiLayer : public ME::Core::Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

	public:
		void OnAttach() override;
		void OnDetach() override;

		void OnEvent(ME::Core::Event& event) override;
		void OnImGuiRender(float64 deltaTime, ImGuiContext* context) override;

	public:
		void BeginRender();
		void EndRender(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer);
		void PostRender();

		inline ImGuiContext* GetContext() { return m_ImGuiContext; }

		void Shutdown();
		void EnableEvents(bool enabled) { m_EnabledEvents = enabled; }

	private:
		void Render(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer);
		void CreateRenderResources();


	private:
		ImGuiContext* m_ImGuiContext;

	private:
		ME::Core::Memory::Reference<ME::Render::RFramebuffer> m_Framebuffer;
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_RenderPass;
		ME::Core::Memory::Reference<ME::Render::ResourceHandler> m_ResourceHandler;

	private:
		bool m_Disabled = false;
		bool m_EnabledEvents = true;
	};
}
