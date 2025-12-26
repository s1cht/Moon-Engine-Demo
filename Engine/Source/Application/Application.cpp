#include "Application.hpp"
#include "EngineDefines.hpp"

#include <Core.hpp>
#include <Core/Utility/Logging/Logger.hpp>
#include <Core/Utility/MemWatch/MemWatch.hpp>
#include <Core/Utility/Benchmark/Benchmark.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Time.hpp>

#include "Framework/Components/AudioComponent.hpp"
#include "Framework/Components/InputComponent.hpp"
#include "Framework/Components/CameraComponent.hpp"
#include "Framework/Entities/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Managers/ShaderManager.hpp"
#include "Renderer/Managers/MeshManager.hpp"
#include "Renderer/RenderResourcesTracker.hpp"
#include "Renderer/Managers/AssetManager.h"
#include "Utility/AssetLoader.hpp"

namespace ME
{

    Application* Application::s_Instance;
	bool Application::s_ShutdownRequested;

	Application::Application(ApplicationProperties props)
		: m_AppData(props), m_WindowUpdateX(0), m_WindowUpdateY(0)
	{
		ME_ASSERT(!s_Instance, "Application is already created!");
		s_Instance = this;
		s_ShutdownRequested = false;

		Input::InputController::Get().SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		ME::Core::String str = ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath"));
		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("Assets"), (str + TEXT("assets\\"))));
		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("ShaderSource"), (str + TEXT("assets\\Shaders\\Source\\"))));
		ME::Core::IO::DirectoryStorage::StoreDirectory(ME::Core::IO::DirectoryStorage::Directory(TEXT("CompiledShaders"), (str + TEXT("assets\\Shaders\\Compiled\\"))));

		ME::WindowProperties windowProps{};
		windowProps.WindowTitle = m_AppData.ApplicationName;
		windowProps.WindowSize = ME::Core::Math::Vector2D32(1920.f, 1050.f);

		m_Window = ME::Core::Memory::Scope<Window>(Window::Create(windowProps));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		// Renderer initialization
		Render::Renderer::SetRenderAPI(ME::Render::RenderAPI::API::Vulkan);
		if (!Render::RenderCommand::Init())
		{
			RequestShutdown();
			return;
		}

		// Shader manager initialization
		Render::Manager::ShaderManager::Get();
		ME::Render::Manager::ShaderManager::Get().LoadCompiler();

		ME::Core::Array<Core::WideString> GlobalIncludePaths = 
		{
		    Core::StringToWideString(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("ShaderSource")))
		};

		Utility::ShaderCompiler::Get().SetGlobalIncludePaths(GlobalIncludePaths);
		ME::Render::Manager::ShaderManager::Get().SetShaderSourcePath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("ShaderSource")));
		ME::Render::Manager::ShaderManager::Get().SetCompiledShaderPath(ME::Core::IO::DirectoryStorage::GetDirectory(TEXT("CompiledShaders")));

		Render::ResourceBinding commonBinding = {};
		commonBinding.Stage = Render::ShaderStage::Vertex | 
			Render::ShaderStage::Mesh | 
			Render::ShaderStage::Task | 
			Render::ShaderStage::Compute | 
			Render::ShaderStage::Domain | 
			Render::ShaderStage::Hull;
		commonBinding.Type = Render::ResourceType::StorageBuffer;

		Render::Renderer::Init();

		m_World = ME::Core::Memory::MakeReference<ECS::World>();

		m_World->RegisterComponent<Components::MeshComponent>();
		m_World->RegisterComponent<Components::TransformComponent>();
		m_World->RegisterComponent<Components::CameraComponent>();
		m_World->RegisterComponent<Components::InputComponent>();
		m_World->RegisterComponent<Components::AudioComponent>();

		Render::Manager::MeshMemoryPoolInfo meshPoolInfo = {};
		meshPoolInfo.VertexMemoryPoolSize = ME_MESH_MNG_VERT_BUFFER_SIZE;
		meshPoolInfo.IndexMemoryPoolSize = ME_MESH_MNG_IND_BUFFER_SIZE;
		meshPoolInfo.MeshletMemoryPoolSize = ME_MESH_MNG_MESHLET_BUFFER_SIZE;
		meshPoolInfo.BindingInfo = commonBinding;

		Render::Manager::MeshManager::Get().Init(meshPoolInfo);

		m_AudioEngine = Audio::AudioEngine::Create();

		LoadAssets();

		m_ImGuiLayer = new Render::ImGui::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_Runs = true;
	}

	Application::~Application()
	{
		m_ImGuiLayer->Shutdown();
		ME::Render::Manager::ShaderManager::Get().Shutdown();
		Render::RenderResourcesTracker::Get().ShutdownAll();
		Render::Renderer::Shutdown();
		s_ShutdownRequested = false;
	}

    void Application::RunImpl()
	{
		while (m_Runs && !s_ShutdownRequested)
		{
			float64 delta = ME::Core::Clock::Time::Update().AsSeconds();
			m_Window->OnUpdate(delta);
			m_World->OnUpdate(static_cast<float32>(delta));

			if (m_WindowUpdateX != 0 && m_WindowUpdateY != 0)	
			{
				Render::RenderCommand::OnWindowUpdate(m_WindowUpdateX, m_WindowUpdateY);
				m_WindowUpdateX = m_WindowUpdateY = 0;
			}

			for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); ++layer)
				(*layer)->OnUpdate(delta);

			Render::Renderer::BeginFrame();

			ME::Core::Array<ME::Core::Memory::Reference<ECS::Entity>> cameras = m_World->GetEntitiesWhichAre<ME::EditorCamera>();
			if (!cameras.Empty())
			{
			    auto entsWithMesh = m_World->View<Components::TransformComponent, Components::MeshComponent>();
				ME::Core::Memory::Reference<ME::Render::Camera> camera = std::static_pointer_cast<ME::EditorCamera>(cameras[0]);

			    Render::Renderer::BeginScene(camera);
				for (const auto& entity : entsWithMesh)
				{
					Components::MeshComponent mesh = entity->GetComponent<Components::MeshComponent>();
					Components::TransformComponent transform = entity->GetComponent<Components::TransformComponent>();
					Render::Renderer::RenderMesh(mesh, transform);
				}
				Render::Renderer::EndScene();
			}

			for (auto layer = m_LayerStack.Begin(); layer != m_LayerStack.End(); ++layer)
				(*layer)->OnRender();

			m_ImGuiLayer->BeginRender();

			for (auto layer : m_LayerStack)
				layer->OnImGuiRender(delta, m_ImGuiLayer->GetContext());

			m_ImGuiLayer->EndRender(Render::RenderCommand::GetAvailableCommandBuffer());
			m_ImGuiLayer->PostRender();

			Render::Renderer::EndFrame();
		}

		if (s_ShutdownRequested)
			this->~Application();
	}

	void Application::PushLayer(ME::Core::Layer* layer)
	{
		Get().m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(ME::Core::Layer* overlay)
	{
		Get().m_LayerStack.PushOverlay(overlay);
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

    void Application::LoadAssets()
    {
		for (auto& asset : m_AppData.AssetPaths)
            if (!Manager::AssetManager::Load(asset, true))
				ME_ERROR("Failed to load asset: \"{}\"!", asset);
		for (auto& shader : m_AppData.ShaderPaths)
		{
		    if (!Render::Manager::ShaderManager::Get().LoadShaders(shader))
		    {
			    ME_ASSERT(false, "Failed to load shader group: \"{}\"!", shader.ShaderGroupName);
				RequestShutdown();
		    }
		}
    }
}
