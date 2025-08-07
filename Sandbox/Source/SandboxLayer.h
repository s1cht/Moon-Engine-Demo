#pragma once
#include <Core.hpp>
#include <Core/Layer/Layer.hpp>
#include <Renderer/Base/Buffer.h>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/Pipeline.h>
#include <Renderer/Camera/Camera.h>
#include <Events/WindowEvents.h>
#include <Assets/Mesh.h>

#include "Renderer/Base/CommandBuffer.h"
#include "Renderer/Base/Framebuffer.h"

struct Light
{
	ME::Core::Math::Vector4D32 Color;
	ME::Core::Math::Vector3D32 Direction;
	float32 Padding;
};

class SandboxLayer : public ME::Core::Layer
{
public:
	SandboxLayer()
		: Layer(TEXT("Example"))
	{
	}
	void OnAttach() override;

	void OnUpdate(float64 deltaTime) override;
	void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) override;

	void OnEvent(ME::Core::Event & event) override;

private:
	bool SetViewportSize(ME::Events::WindowResizedEvent& event);

private:
	ME::Core::Memory::Reference<ME::Render::RenderPass> m_MainRenderPass;
	ME::Core::Memory::Reference<ME::Render::Pipeline> m_Primary;
	ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Framebuffer>> m_WindowFramebuffers;
	ME::Render::VertexBufferLayout m_Layout;


	ME::Core::Memory::Reference<ME::Render::Uniform> m_CameraBuffer;
	ME::Core::Memory::Reference<ME::Render::Uniform> m_SceneBuffer;
	ME::Core::Memory::Reference<ME::Render::Uniform> m_LightBuffer;
	ME::Core::Memory::Reference<ME::Render::IndexBuffer> m_IndexBuffer;
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> m_VertexBuffer;

	ME::Core::Math::Matrix4x4 m_WorldMatrix;
	Light m_Light;

private:
	ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>> m_Flashlight;


private:
	ME::Core::Memory::Reference<ME::Render::Camera::Camera> m_Camera;

private:
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

};
