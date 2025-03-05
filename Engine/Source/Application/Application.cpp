#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Core/Utils/Logging/Logger.h"
#include "Core/Events/MouseEvents.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Utils/Benchmark/Benchmark.h"
#include "Core/Utils/MemWatch/MemWatch.h"
#include "Core/Math/Vector2D.h"
#include "Core/Math/Vector3D.h"
#include "Core/Containers/String.h"
#include "Core/Containers/Array.h"

namespace Pawn {

	using namespace std::chrono_literals;
	using namespace Math;


	Application::Application()
	{	
		m_Input = std::make_unique<Input>();
		m_Input->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetInputHandler(m_Input.get());
		m_Runs = true;
	}

	void Application::Run()
	{
		while (m_Runs)
		{
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			m_Window->OnUpdate();
		};
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::OnEvent(Event& event)
	{
		for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin();)
		{
			(*--it)->OnEvent(event);
			if (event.IsHandled)
				break;
		}
	}

}