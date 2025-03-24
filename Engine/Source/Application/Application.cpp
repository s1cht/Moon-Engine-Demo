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
#include <Core/Platform/Base/IO.h>
#include <Core/Misc/Time.h>

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

		String str = IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath"));

		IO::DirectoryStorage::StoreDirectory(IO::DirectoryStorage::Directory(TEXT("Assets"), (str + TEXT("assets\\"))));
		IO::DirectoryStorage::StoreDirectory(IO::DirectoryStorage::Directory(TEXT("Shaders"), (str + TEXT("assets\\Shaders\\Source\\"))));
		IO::DirectoryStorage::StoreDirectory(IO::DirectoryStorage::Directory(TEXT("CompiledShaders"), (str + TEXT("assets\\Shaders\\Compiled\\"))));

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
			float64 delta = Pawn::Time::Time::Update().AsMilliseconds();
			
			m_Window->OnUpdate(delta);

			Render::RenderCommand::Clear(Math::Vector4D32(0.f, 0.f, 0.f, 1.f));

			Render::Renderer::BeginScene();

			for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); layer++)
			{
				(*layer)->OnUpdate(delta);
			}

			Render::Renderer::EndScene();
			
			m_ImGuiLayer->BeginRender();

			for (auto layer : m_LayerStack)
				layer->OnImGuiRender(delta);

			m_ImGuiLayer->EndRender();

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