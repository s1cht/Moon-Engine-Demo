#include "Application.h"
#include "Events/MouseEvents.h"
#include "Events/KeyEvents.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include <Core.h>
#include <Core/Utils/Logging/Logger.h>
#include <Core/Utils/Benchmark/Benchmark.h>
#include <Core/Utils/MemWatch/MemWatch.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Array.h>

namespace Pawn {

	using namespace std::chrono_literals;
	using namespace Math;

	Application* Application::s_Instance;

	Application::Application()
		: m_WindowUpdateX(0), m_WindowUpdateY(0)
	{	
		PE_ASSERT(!s_Instance, TEXT("Application is already created!"));
		s_Instance = this;

		Input::Get().SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window = Pawn::Memory::Scope<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Render::Renderer::SetRenderAPI(Pawn::Render::RendererAPI::API::DirectX11);
		Render::Renderer::Init();

		m_ImGuiLayer = new Render::Imgui::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_Runs = true;
	}

	Application::~Application()
	{
		m_ImGuiLayer->Shutdown();
		Render::Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (m_Runs)
		{
			m_Window->OnUpdate();

			Render::RenderCommand::Clear(Math::Vector4D32(0.f, 0.f, 0.f, 1.f));

			m_ImGuiLayer->BeginRender();
			for (auto layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->EndRender();

			Render::Renderer::BeginScene();

			for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); layer++)
			{
				(*layer)->OnUpdate();
			}

			Render::Renderer::EndScene();

			m_ImGuiLayer->PostRender();

			if (m_WindowUpdateX != 0 && m_WindowUpdateY != 0)
			{
				Render::RenderCommand::OnWindowUpdate(m_WindowUpdateX, m_WindowUpdateY);
				m_WindowUpdateX = m_WindowUpdateY = 0;
			}

			Render::RenderCommand::Present();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowSizeEvent));

		for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();)
		{
			(*--it)->OnEvent(event);
			if (event.IsHandled)
				break;
		}

	}

	bool Application::OnWindowSizeEvent(WindowResizedEvent& event)
	{
		m_WindowUpdateX = (int32)event.GetSizeX();
		m_WindowUpdateY = (int32)event.GetSizeY();
		return false;
	}

}