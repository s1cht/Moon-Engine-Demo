#include "Application.h"
#include "Events/MouseEvents.h"
#include "Events/KeyEvents.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include <Core.h>
#include <Core/Utils/Logging/Logger.h>
#include <Core/Utils/MemWatch/MemWatch.h>

#include <Core/Misc/Assertion.h>

import Pawn.Core.IO;
import Pawn.Core.Clock;
import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;
import Pawn.Core.Utils.Benchmark;

namespace Pawn {

	using namespace std::chrono_literals;

	Application* Application::s_Instance;

	Application::Application(ApplicationProperties props)
		: m_WindowUpdateX(0), m_WindowUpdateY(0), m_AppData(props)
	{	
		PE_ASSERT(!s_Instance, TEXT("Application is already created!"));
		s_Instance = this;

		Input::InputController::Get().SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Pawn::WindowProperties windowProps{};
		windowProps.WindowTitle = m_AppData.ApplicationName;
		windowProps.WindowSize = Pawn::Core::Math::Vector2D32(1920.f, 1050.f);

		m_Window = Pawn::Core::Memory::Scope<Window>(Window::Create(windowProps));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Render::Renderer::SetRenderAPI(Pawn::Render::RendererAPI::API::Vulkan);
		Render::Renderer::Init();

		//m_ImGuiLayer = new Render::Imgui::ImGuiLayer();
		//PushOverlay(m_ImGuiLayer);

		Pawn::Core::Containers::String str = Pawn::Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath"));

		Pawn::Core::IO::DirectoryStorage::StoreDirectory(Pawn::Core::IO::DirectoryStorage::Directory(TEXT("Assets"), (str + TEXT("assets\\"))));
		Pawn::Core::IO::DirectoryStorage::StoreDirectory(Pawn::Core::IO::DirectoryStorage::Directory(TEXT("Shaders"), (str + TEXT("assets\\Shaders\\Source\\"))));
		Pawn::Core::IO::DirectoryStorage::StoreDirectory(Pawn::Core::IO::DirectoryStorage::Directory(TEXT("CompiledShaders"), (str + TEXT("assets\\Shaders\\Compiled\\"))));

		m_Runs = true;
	}

	Application::~Application()
	{
		//m_ImGuiLayer->Shutdown();
		Render::Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (m_Runs)
		{
			float64 delta = Pawn::Core::Clock::Time::Update().AsMilliseconds();
			
			m_Window->OnUpdate(delta);

			//Render::RenderCommand::Clear(Pawn::Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f));


			//for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); layer++)
			//{
			//	(*layer)->OnUpdate(delta);
			//}

			//Render::Renderer::EndScene();
			//
			//m_ImGuiLayer->BeginRender();

			//for (auto layer : m_LayerStack)
			//	layer->OnImGuiRender(delta, m_ImGuiLayer->GetContext());

			//m_ImGuiLayer->EndRender();

			//m_ImGuiLayer->PostRender();

			//if (m_WindowUpdateX != 0 && m_WindowUpdateY != 0)
			//{
			//	Render::RenderCommand::OnWindowUpdate(m_WindowUpdateX, m_WindowUpdateY);
			//	m_WindowUpdateX = m_WindowUpdateY = 0;
			//}

			//Render::RenderCommand::Present();
		}
	}

	void Application::PushLayer(Pawn::Core::Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Pawn::Core::Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Pawn::Core::Event& event)
	{
		Pawn::Core::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Pawn::Events::WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowSizeEvent));
		dispatcher.Dispatch<Pawn::Events::WindowClosedEvent>(BIND_EVENT_FN(Application::OnClosedEvent));

		for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();)
		{
			(*--it)->OnEvent(event);
			if (event.IsHandled)
				break;
		}

	}

	bool Application::OnClosedEvent(Pawn::Events::WindowClosedEvent& event)
	{
		m_Runs = false;

		return false;
	}

	bool Application::OnWindowSizeEvent(Pawn::Events::WindowResizedEvent& event)
	{
		m_WindowUpdateX = (int32)event.GetSizeX();
		m_WindowUpdateY = (int32)event.GetSizeY();
		return false;
	}

}