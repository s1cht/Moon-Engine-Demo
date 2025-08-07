#include "Application.h"
#include "Events/MouseEvents.h"
#include "Events/KeyEvents.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Utils/MemWatch/MemWatch.hpp>
#include <Core/Utils/Benchmark/Benchmark.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Misc/Time.hpp>

#include "Assets/ShaderManager.hpp"
#include "Renderer/RenderResourcesTracker.hpp"
#include "Utility/AssetLoader.h"

namespace ME {

	using namespace std::chrono_literals;

	Application* Application::s_Instance;
	bool Application::s_ShutdownRequested;

	Application::Application(ApplicationProperties props)
		: m_WindowUpdateX(0), m_WindowUpdateY(0), m_AppData(props)
	{	
		ME_ASSERT(!s_Instance, TEXT("Application is already created!"));
		s_Instance = this;
		s_ShutdownRequested = false;

		Input::InputController::Get().SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		ME::WindowProperties windowProps{};
		windowProps.WindowTitle = m_AppData.ApplicationName;
		windowProps.WindowSize = ME::Core::Math::Vector2D32(1920.f, 1050.f);

		m_Window = ME::Core::Memory::Scope<Window>(Window::Create(windowProps));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Render::Renderer::SetRenderAPI(ME::Render::RendererAPI::API::Vulkan);
		Render::Renderer::Init();
		Assets::ShaderManager::Get();

		//m_ImGuiLayer = new Render::Imgui::ImGuiLayer();
		//PushOverlay(m_ImGuiLayer);

		ME::Core::Containers::String str = ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath"));

		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("Assets"), (str + TEXT("assets\\"))));
		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("Shaders"), (str + TEXT("assets\\Shaders\\Source\\"))));
		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("CompiledShaders"), (str + TEXT("assets\\Shaders\\Compiled\\"))));

		m_Runs = true;
	}

	Application::~Application()
	{
		//m_ImGuiLayer->Shutdown();
		ME::Assets::ShaderManager::Get().Shutdown();
		Render::RenderResourcesTracker::Get().ShutdownAll();
		Render::Renderer::Shutdown();
		s_ShutdownRequested = false;
	}

	void Application::Run()
	{
		while (m_Runs && !s_ShutdownRequested)
		{
			float64 delta = ME::Core::Clock::Time::Update().AsMilliseconds();
			
			m_Window->OnUpdate(delta);

			//Render::RenderCommand::Clear(Pawn::Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f));

			for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); layer++)
			{
				(*layer)->OnUpdate(delta);
 			}

			//Render::Renderer::EndScene();
			
			//m_ImGuiLayer->BeginRender();

			//for (auto layer : m_LayerStack)
			//	layer->OnImGuiRender(delta, m_ImGuiLayer->GetContext());

			//m_ImGuiLayer->EndRender();

			//m_ImGuiLayer->PostRender();

			if (m_WindowUpdateX != 0 && m_WindowUpdateY != 0)
			{
				Render::RenderCommand::OnWindowUpdate(m_WindowUpdateX, m_WindowUpdateY);
				m_WindowUpdateX = m_WindowUpdateY = 0;
			}

			//Render::RenderCommand::Present();
		}

		if (s_ShutdownRequested)
		{
			this->~Application();
		}
	}

	void Application::PushLayer(ME::Core::Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(ME::Core::Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(ME::Core::Event& event)
	{
		ME::Core::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<ME::Events::WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowSizeEvent));
		dispatcher.Dispatch<ME::Events::WindowClosedEvent>(BIND_EVENT_FN(Application::OnClosedEvent));

		for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();)
		{
			(*--it)->OnEvent(event);
			if (event.IsHandled)
				break;
		}

	}

	bool Application::OnClosedEvent(ME::Events::WindowClosedEvent& event)
	{
		m_Runs = false;

		return false;
	}

	bool Application::OnWindowSizeEvent(ME::Events::WindowResizedEvent& event)
	{
		m_WindowUpdateX = (int32)event.GetSizeX();
		m_WindowUpdateY = (int32)event.GetSizeY();
		return false;
	}

}
