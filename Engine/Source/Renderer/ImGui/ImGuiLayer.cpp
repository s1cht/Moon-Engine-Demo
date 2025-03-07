#include "pch.h"
#include "ImGuiLayer.h"
#include "imgui.h"

namespace Pawn
{
	ImGuiLayer::ImGuiLayer()
		: Layer(TEXT("ImGuiLayer"))
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;



	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnUpdate()
	{

	}

	void ImGuiLayer::OnEvent(Event& event)
	{
	
	}
}