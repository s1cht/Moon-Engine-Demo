#include "SandboxLayer.h"
#include <Application/Application.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCommand.h>
#include <Renderer/ImGui/ImGuiReferences.h>
#include <Renderer/Camera/PerpectiveCamera.h>

#include <Assets/AssetManager.h>

import Pawn.Core.IO;

using namespace Pawn;

void SandboxLayer::OnAttach()
{
	bool result;

	result = Assets::AssetManager::Load(Core::IO::DirectoryStorage::GetDirectory(TEXT("ProgramPath")) + Core::Containers::String(TEXT("assets/Meshes/torch.obj")));
	if (!result)
	{
		PE_ERROR(TEXT("Failed to load asset!"));
	}

	m_Torch = Assets::AssetManager::Get().GetMesh(TEXT("Wood_Cone.005"));

	Render::BufferLayout layout = {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float4, "COLOR", 0, 0 }
	};

	m_WindowWidth = (uint32)Application::Get().GetWindow()->GetWidth();
	m_WindowHeight = (uint32)Application::Get().GetWindow()->GetHeight();

	m_Primary = Core::Memory::Reference<Render::PipelineState>(Render::PipelineState::Create());

	Render::Shader::SetShaderSourceExtension(TEXT(".hlsl"));
	Render::Shader::SetCompiledShaderExtension(TEXT(".cso"));

	m_VertexShader = Core::Memory::Reference<Render::Shader>(Render::Shader::CreateShader(TEXT("vs_primary"), Render::Shader::Type::Vertex, false));
	m_PixelShader = Core::Memory::Reference<Render::Shader>(Render::Shader::CreateShader(TEXT("ps_primary"), Render::Shader::Type::Pixel, false));

	const float32 points[8][7] = 
	{
		{ -0.5f, -0.5f, -0.5f,   1.f, 0.f, 0.f, 1.f },
		{  0.5f, -0.5f, -0.5f,   0.f, 1.f, 0.f, 1.f },
		{  0.5f,  0.5f, -0.5f,   0.f, 0.f, 1.f, 1.f },
		{ -0.5f,  0.5f, -0.5f,   1.f, 1.f, 0.f, 1.f },
		{ -0.5f, -0.5f,  0.5f,   1.f, 0.f, 1.f, 1.f },
		{  0.5f, -0.5f,  0.5f,   0.f, 1.f, 1.f, 1.f },
		{  0.5f,  0.5f,  0.5f,   1.f, 1.f, 1.f, 1.f },
		{ -0.5f,  0.5f,  0.5f,   0.f, 0.f, 0.f, 1.f },
	};


	const uint32 indices[36] = 
	{
		4, 5, 6,
		4, 6, 7,

		1, 0, 3,
		1, 3, 2,

		0, 4, 7,
		0, 7, 3,

		5, 1, 2,
		5, 2, 6,

		3, 7, 6,
		3, 6, 2,

		0, 1, 5,
		0, 5, 4
	};


	m_VertexBuffer = Core::Memory::Reference<Render::VertexBuffer>(Render::VertexBuffer::Create((void*)points, sizeof(float32) * 8 * 7, Render::Usage::Default));
	m_IndexBuffer = Core::Memory::Reference<Render::IndexBuffer>(Render::IndexBuffer::Create((void*)indices, 36, Render::Usage::Default));

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
	m_Primary->SetVertexShader(m_VertexShader);
	m_Primary->SetPixelShader(m_PixelShader);
	m_Primary->SetDepthStencilState(false, true, Render::DepthComparison::Greater);
	m_Primary->SetRasterizerState(Render::RasterizerCull::Back, Render::RasterizerFill::Fill, true, false, true, false, 0, 0.f, false, 0);

	m_Primary->SetBlendState(true, Render::BlendMask::All);
	m_Primary->SetInputLayout(layout, Pawn::Render::InputClassification::PerVertex, 0);

	m_CameraBuffer = Core::Memory::Reference<Render::Uniform>(Render::Uniform::Create(sizeof(Core::Math::Matrix4x4), Render::Usage::Dynamic));
	m_SceneBuffer = Core::Memory::Reference<Render::Uniform>(Render::Uniform::Create(sizeof(Core::Math::Matrix4x4), Render::Usage::Dynamic));

	m_WorldMatrix = Core::Math::Matrix4x4(1);

	m_Camera = Core::Memory::Reference<Render::Camera::Camera>(new Render::Camera::PerpectiveCamera());
	m_Camera->SetFOV(90.f);
	m_Camera->SetAspectRatio(1080.f / 1920.f);
	m_Camera->SetPosition(Core::Math::Vector3D(0));
	m_Camera->SetRotation(0, 0, 0);

}

void SandboxLayer::OnUpdate(float64 deltaTime)
{
	Render::BufferLayout layout = {
		{ Render::ShaderType::Float3, "POSITION", 0, 0 },
		{ Render::ShaderType::Float4, "COLOR", 0, 0 }
	};

	auto camPos = m_Camera->GetPosition();
	static float32 yaw = 0.f;
	static float32 pitch = 0.f;
	static const float32 cameraSpeed = 0.001f;
	static const float32 cameraSens = 0.001f;
	static bool update = false;

	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_W))
	{
		camPos += Core::Math::Vector3D::ForwardVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_S))
	{
		camPos += Core::Math::Vector3D::BackwardVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_A))
	{
		camPos += Core::Math::Vector3D::LeftVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_D))
	{
		camPos += Core::Math::Vector3D::RightVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_E))
	{
		camPos += Core::Math::Vector3D::UpVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_Q))
	{
		camPos += Core::Math::Vector3D::DownVector() * ((float64)cameraSpeed * deltaTime);
		update = true;
	}

	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_UP))
	{
		yaw += cameraSens * deltaTime;
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_DOWN))
	{
		yaw -= cameraSens * deltaTime;
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_LEFT))
	{
		pitch += cameraSens * deltaTime;
		update = true;
	}
	if (Input::InputController::IsKeyDown(Input::Keycode::PE_KEY_RIGHT))
	{
		pitch -= cameraSens * deltaTime;
		update = true;
	}

	if (update)
	{
		m_Camera->SetRotation(yaw, pitch, yaw);
		m_Camera->SetPosition(camPos);
		update = false;
	}

	m_Camera->Update();

	Core::Math::Matrix4x4 mat = m_Camera->GetProjViewMatrix().Transpose();

	Render::Renderer::BeginScene(m_Camera);
	D3DPERF_BeginEvent(0xffffffff, TEXT("Start"));
	m_CameraBuffer->SetData(const_cast<void*>(static_cast<const void*>(&mat)), sizeof(Core::Math::Matrix4x4));
	m_SceneBuffer->SetData(static_cast<void*>(&m_WorldMatrix), sizeof(Core::Math::Matrix4x4));
	m_Primary->SetPrimitiveTopology(Pawn::Render::PrimitiveTopology::TriangleList, 0);
	m_Primary->BindInput();

#define TORCH

#ifdef TORCH
	m_Torch->GetVertexBuffer()->Bind(layout);
	m_Torch->GetIndexBuffer()->Bind();
#else
	m_VertexBuffer->Bind(layout);
	m_IndexBuffer->Bind();
#endif
	m_CameraBuffer->Bind(0, Render::Shader::Type::Vertex);
	m_SceneBuffer->Bind(1, Render::Shader::Type::Vertex);

	m_Primary->Bind();
	Render::RenderCommand::BindBackBuffer();

#ifdef TORCH
	Render::Renderer::Submit(m_Torch->GetIndexBuffer()->GetCount(), 0);
#else
	Render::Renderer::Submit(m_IndexBuffer->GetCount(), 0);
#endif

	D3DPERF_EndEvent();
}

void SandboxLayer::OnEvent(Core::Event& event)
{
	Pawn::Core::EventDispatcher dispathcher(event);
	dispathcher.Dispatch<Events::WindowResizedEvent>(BIND_EVENT_FN(SandboxLayer::SetViewportSize));

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);
}

void SandboxLayer::OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext)
{
	ImGui::SetCurrentContext(dllContext);
	static bool statsVisible = true;
	static bool settsVisible = true;

	if (ImGui::Begin("Camera stats", &statsVisible))
	{
		ImGui::Text("CameraPosition: %.5f, %.5f, %.5f", m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z);
		ImGui::Text("------------");
		ImGui::Text("Camera view matrix:");
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a11, m_Camera->GetViewMatrix().a21, m_Camera->GetViewMatrix().a31, m_Camera->GetViewMatrix().a41);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a12, m_Camera->GetViewMatrix().a22, m_Camera->GetViewMatrix().a32, m_Camera->GetViewMatrix().a42);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a13, m_Camera->GetViewMatrix().a23, m_Camera->GetViewMatrix().a33, m_Camera->GetViewMatrix().a43);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetViewMatrix().a14, m_Camera->GetViewMatrix().a24, m_Camera->GetViewMatrix().a34, m_Camera->GetViewMatrix().a44);

		ImGui::Text("------------");
		ImGui::Text("Camera projection matrix:");
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a11, m_Camera->GetProjectionMatrix().a21, m_Camera->GetProjectionMatrix().a31, m_Camera->GetProjectionMatrix().a41);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a12, m_Camera->GetProjectionMatrix().a22, m_Camera->GetProjectionMatrix().a32, m_Camera->GetProjectionMatrix().a42);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a13, m_Camera->GetProjectionMatrix().a23, m_Camera->GetProjectionMatrix().a33, m_Camera->GetProjectionMatrix().a43);
		ImGui::Text("%.5f %.5f %.5f %.5f", m_Camera->GetProjectionMatrix().a14, m_Camera->GetProjectionMatrix().a24, m_Camera->GetProjectionMatrix().a34, m_Camera->GetProjectionMatrix().a44);
	}

	ImGui::End();

	static int32 CameraFOV = 70.f;
	static float32 CameraNear = 0.1f;
	static float32 CameraFar = 1000.f;
	static float32 CameraAspectRatioX = 1920.f;
	static float32 CameraAspectRatioY = 1080.f;

	if (ImGui::Begin("Camera settings", &settsVisible))
	{
		if (ImGui::DragInt("Camera FOV", &CameraFOV, 1.f, 10, 120))
		{
			m_Camera->SetFOV(CameraFOV);
		}
		if (ImGui::DragFloat("Camera near", &CameraNear, 1.f, 0, 100.f))
		{
			m_Camera->SetNear(CameraNear);
		}
		if (ImGui::DragFloat("Camera far", &CameraFar, 0.1f, 0, 100000.f))
		{
			m_Camera->SetFar(CameraFar);
		}
		if (ImGui::DragFloat("Camera AR X", &CameraAspectRatioX, 0.1f, 0, 8120.f))
		{
			m_Camera->SetAspectRatio(CameraAspectRatioX / CameraAspectRatioY);
		}
		if (ImGui::DragFloat("Camera AR Y", &CameraAspectRatioY, 0.1f, 0, 8120.f))
		{
			m_Camera->SetAspectRatio(CameraAspectRatioX / CameraAspectRatioY);
		}
	}

	ImGui::End();
}

bool SandboxLayer::SetViewportSize(Pawn::Events::WindowResizedEvent& event)
{
	m_WindowWidth = (uint32)event.GetSizeX();
	m_WindowHeight = (uint32)event.GetSizeY();

	m_Primary->SetViewport(m_WindowWidth, m_WindowHeight);

	return false;
}
