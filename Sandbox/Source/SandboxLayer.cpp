#include "SandboxLayer.hpp"
#include <Renderer/ImGui/ImGuiReferences.hpp>

#include "Renderer/Managers/AssetManager.h"

using namespace ME;

SandboxLayer::SandboxLayer()
	: Layer(TEXT("Example"))
{
	m_DWorld = ME::Application::GetWorld()->Create<ME::Music>();
	m_Block = ME::Application::GetWorld()->Create<ME::Block>();
	m_Camera = ME::Application::GetWorld()->Create<ME::EditorCamera>();
}

void SandboxLayer::OnAttach()
{
	ME::Render::PerspectiveProjectionInfo info = {};
	info.AspectRatio = 16.f / 9.f;
	info.Far = 1000.f;
	info.Near = 0.1f;
	info.FieldOfView = 103.f;
	m_Camera->SetPerspectiveProjectionInfo(info);
	m_Camera->SetCameraSpeed(2.f);
	m_Camera->SetCameraSensitivity(10.f);
	
	Components::MeshComponent& meshComp = m_Block->Mesh();
	meshComp.Visible = true;
	meshComp.Opacity = 1;
	meshComp.ShadowsVisible = false;
	meshComp.Mesh = Manager::AssetManager::Get().GetMesh("Flashlight");
	meshComp.Mesh->Load();
	m_Block->SetPosition(ME::Core::Math::Vector3D(0, 0, -10));

	Components::AudioComponent& music = m_DWorld->Audio();
	ME::Audio::AudioSpecification specs = {};
	specs.AudioFile = Manager::AssetManager::Get().GetAudioFile("dWorld.wav");
	music.Audio = ME::Audio::Audio::Create(specs);
	music.Audio->SetVolume(0.1f);
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
	Components::AudioComponent& music = m_DWorld->Audio();
	ImGui::SetCurrentContext(dllContext);

	static bool IsPlaying = false;
	static bool fired = false;

	ImGui::Begin("Music");
	if (ImGui::Checkbox("Enable music", &IsPlaying))
		fired = true;

	ImGui::End();

	if (fired)
	{
		if (IsPlaying)
			music.Audio->Play();
		else
			music.Audio->Stop();

		fired = false;
	}
}

//bool OnKeyInputStartedEvent(ME::Events::KeyInputStartedEvent& event)
