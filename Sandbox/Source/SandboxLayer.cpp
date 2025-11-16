#include "SandboxLayer.hpp"
#include <Renderer/ImGui/ImGuiReferences.hpp>

#include "Renderer/Managers/AssetManager.h"

using namespace ME;

SandboxLayer::SandboxLayer()
	: Layer(TEXT("Example"))
{
	m_Block = ME::Application::GetWorld()->Create<ME::Block>();
	m_Camera = ME::Application::GetWorld()->Create<ME::PerspectiveCamera>();
}

void SandboxLayer::OnAttach()
{
    Components::CameraComponent& cameraComp = m_Camera->Camera();
	cameraComp.Camera = ME::Core::Memory::MakeReference<ME::Render::PerspectiveCamera>();
	cameraComp.Camera->SetAspectRatio(16.f / 9.f);
	cameraComp.Camera->SetFOV(120);
	cameraComp.Camera->SetFar(1000.f);
	cameraComp.Camera->SetNear(0.1f);
	cameraComp.Camera->SetPosition(Core::Math::Vector3D(0.f, 0.f, 10.f));

	Components::MeshComponent& meshComp = m_Block->Mesh();
	meshComp.Visible = true;
	meshComp.Opacity = 1;
	meshComp.ShadowsVisible = false;
	meshComp.Mesh = Manager::AssetManager::Get().GetMesh("Flashlight");
	meshComp.Mesh->Load();
}

void SandboxLayer::OnUpdate(float64 deltaTime)
{
}

void SandboxLayer::OnRender()
{
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	ME::Core::EventDispatcher dispatcher(event);
	//dispatcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::UpdateRender));
	//dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(SandboxLayer::OnMouseMovedEvent));
}

void SandboxLayer::OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext)
{
	Components::CameraComponent& cameraComp = m_Camera->Camera();

	ImGui::SetCurrentContext(dllContext);

	ImGui::Begin("CameraProps");

	ImGui::Text("%.3f, %.3f, %.3f, %.3f", 
		cameraComp.Camera->GetViewMatrix().Matrix[0][0],
		cameraComp.Camera->GetViewMatrix().Matrix[0][1],
		cameraComp.Camera->GetViewMatrix().Matrix[0][2],
		cameraComp.Camera->GetViewMatrix().Matrix[0][3]
	);
	ImGui::Text("%.3f, %.3f, %.3f, %.3f",
		cameraComp.Camera->GetViewMatrix().Matrix[1][0],
		cameraComp.Camera->GetViewMatrix().Matrix[1][1],
		cameraComp.Camera->GetViewMatrix().Matrix[1][2],
		cameraComp.Camera->GetViewMatrix().Matrix[1][3]
	);
	ImGui::Text("%.3f, %.3f, %.3f, %.3f",
		cameraComp.Camera->GetViewMatrix().Matrix[2][0],
		cameraComp.Camera->GetViewMatrix().Matrix[2][1],
		cameraComp.Camera->GetViewMatrix().Matrix[2][2],
		cameraComp.Camera->GetViewMatrix().Matrix[2][3]
	);
	ImGui::Text("%.3f, %.3f, %.3f, %.3f",
		cameraComp.Camera->GetViewMatrix().Matrix[3][0],
		cameraComp.Camera->GetViewMatrix().Matrix[3][1],
		cameraComp.Camera->GetViewMatrix().Matrix[3][2],
		cameraComp.Camera->GetViewMatrix().Matrix[3][3]
	);
	ImGui::End();
}

//bool OnKeyInputStartedEvent(ME::Events::KeyInputStartedEvent& event)
