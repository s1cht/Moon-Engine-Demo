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
	m_Light = ME::Application::GetWorld()->Create<ME::PointLight>();
	m_SpotLight = ME::Application::GetWorld()->Create<ME::SpotLight>();
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
	m_Camera->SetPosition(ME::Core::Math::Vector3D(0.f, 5.f, 0.f));

	Components::MeshComponent& meshComp = m_Block->Mesh();
	meshComp.Visible = true;
	meshComp.Opacity = 1;
	meshComp.ShadowsVisible = false;
	meshComp.Mesh = Manager::AssetManager::Get().GetMesh("Flashlight");
	meshComp.Mesh->Load();
	m_Block->SetPosition(ME::Core::Math::Vector3D(0, 0, 0));

	Render::SpotLight& light = m_SpotLight->Light();
	light.Length = 50;
	light.SetColor(ME::Core::Math::Color3(255, 255, 255));

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
	Render::SpotLight& light = m_SpotLight->Light();
	light.Position = m_Camera->GetPosition();
	light.Direction = m_Camera->GetQuaternion().Conjugate().RotateVector(ME::Core::Math::Vector3D::ForwardVector);
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	ME::Core::EventDispatcher dispatcher(event);
	//dispatcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::UpdateRender));
	//dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(SandboxLayer::OnMouseMovedEvent));
}

void SandboxLayer::OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext)
{
	ImGui::SetCurrentContext(dllContext);

	static bool IsPlaying = false;
	static bool fired = false;

	ImGui::Begin("Music");
	if (ImGui::Checkbox("Enable music", &IsPlaying))
		fired = true;
	ImGui::End();

	static bool lightUpdated = false;
	static bool lightEnabled = true;
	static float lightDiameter = 10.f;
	static float lightBrightness = 1.f;
	static ME::Core::Math::Vector3D lightPosition;
	static int lightColor[3] = { 0 };

	static bool flashlightUpdated = false;
	static bool flashlightEnabled = true;
	static float flashlightAngles = 10.f;
	static float flashlightLength = 10.f;
	static float flashlightBrightness = 1.f;
	static int flashlightColor[3] = { 0 };

	ImGui::Begin("Lights");
	ImGui::NewLine();
	if (ImGui::InputFloat3("Position", lightPosition.XYZ))
		lightUpdated = false;
	if (ImGui::InputInt3("Color", lightColor))
	{
		ME::Core::Math::Clamp(0, 255, lightColor[0]);
		ME::Core::Math::Clamp(0, 255, lightColor[1]);
		ME::Core::Math::Clamp(0, 255, lightColor[2]);
		lightUpdated = false;
	}
	if (ImGui::DragFloat("Brightness", &lightBrightness, 0.1, 0, 1000.f))
		lightUpdated = false;
	if (ImGui::DragFloat("Diameter", &lightDiameter, 0.1, 0.001, 1000.f))
		lightUpdated = false;
	if (ImGui::Checkbox("Enabled", &lightEnabled))
		lightUpdated = false;

	ImGui::NewLine();

	ImGui::Text("Camera light");
	if (ImGui::InputInt3("Flashlight Color", flashlightColor))
	{
		ME::Core::Math::Clamp(0, 255, flashlightColor[0]);
		ME::Core::Math::Clamp(0, 255, flashlightColor[1]);
		ME::Core::Math::Clamp(0, 255, flashlightColor[2]);
		flashlightUpdated = false;
	}
	if (ImGui::DragFloat("Flashlight Brightness", &flashlightBrightness, 0.1f, 0, 1000.f))
		flashlightUpdated = false;
	if (ImGui::DragFloat("Flashlight Angles", &flashlightAngles, 1, 1.f, 180.f))
		flashlightUpdated = false;
	if (ImGui::DragFloat("Flashlight Length", &flashlightLength, 1, 1.f, 500.f))
		flashlightUpdated = false;
	if (ImGui::Checkbox("Flashlight Enabled", &flashlightEnabled))
		flashlightUpdated = false;
	ImGui::End();

	Components::AudioComponent& music = m_DWorld->Audio();
	Render::PointLight& light = m_Light->Light();
	Render::SpotLight& flashlight = m_SpotLight->Light();

	if (fired)
	{
		if (IsPlaying)
			music.Audio->Play();
		else
			music.Audio->Stop();

		fired = false;
	}

	if (!flashlightUpdated)
	{
		flashlight.Angle = ME::Core::Math::ToRadians(flashlightAngles);
		flashlight.Length = (flashlightLength);
		flashlight.Brightness = flashlightBrightness;
		flashlight.SetColor(ME::Core::Math::Color3(flashlightColor[0], flashlightColor[1], flashlightColor[2]));
		flashlight.SetEnabled(flashlightEnabled);
		flashlightUpdated = true;
	}

	if (!lightUpdated)
	{
		light.Diameter = lightDiameter;
		light.Position = lightPosition;	
		light.Brightness = lightBrightness;
		light.SetColor(ME::Core::Math::Color3(lightColor[0], lightColor[1], lightColor[2]));
		light.SetEnabled(lightEnabled);
		lightUpdated = true;
	}
}

//bool OnKeyInputStartedEvent(ME::Events::KeyInputStartedEvent& event)
