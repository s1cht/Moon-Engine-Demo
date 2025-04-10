#pragma once
#include <Core.h>
#include <Core/Layer/Layer.h>
#include <Renderer/Base/Buffer.h>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/PipelineState.h>
#include <Renderer/Camera/Camera.h>
#include <Events/WindowEvents.h>
#include <Assets/Mesh.h>

class SandboxLayer : public Pawn::Core::Layer
{
public:
	SandboxLayer()
		: Layer(TEXT("Example"))
	{
	}
	void OnAttach() override;

	void OnUpdate(float64 deltaTime) override;
	void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) override;

	void OnEvent(Pawn::Core::Event & event) override;

private:
	bool SetViewportSize(Pawn::Events::WindowResizedEvent& event);

private:
	Pawn::Core::Memory::Reference<Pawn::Render::BufferLayout> m_Layout;
	Pawn::Core::Memory::Reference<Pawn::Render::PipelineState> m_Primary;
	Pawn::Core::Memory::Reference<Pawn::Render::Shader> m_VertexShader;
	Pawn::Core::Memory::Reference<Pawn::Render::Shader> m_PixelShader;
	Pawn::Core::Memory::Reference<Pawn::Render::Uniform> m_CameraBuffer;
	Pawn::Core::Memory::Reference<Pawn::Render::Uniform> m_SceneBuffer;
	Pawn::Core::Memory::Reference<Pawn::Render::IndexBuffer> m_IndexBuffer;
	Pawn::Core::Memory::Reference<Pawn::Render::VertexBuffer> m_VertexBuffer;
	Pawn::Core::Memory::Reference<Pawn::Assets::Mesh> m_Torch;

	Pawn::Core::Math::Matrix4x4 m_WorldMatrix;

private:
	Pawn::Core::Memory::Reference<Pawn::Render::Camera::Camera> m_Camera;

private:
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

};
