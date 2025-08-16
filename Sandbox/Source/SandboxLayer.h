#pragma once
#include <Core.hpp>
#include <Core/Layer/Layer.hpp>
#include <Renderer/Base/Buffer.h>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/Pipeline.h>
#include <Renderer/Camera/Camera.h>
#include <Events/WindowEvents.h>
#include <Assets/Mesh.h>

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Renderer/Base/CommandBuffer.h"
#include "Renderer/Base/Framebuffer.h"
#include "Renderer/Base/RingBuffer.h"

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
	void OnRender() override;
	void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) override;

	void OnEvent(ME::Core::Event & event) override;

private:
	bool UpdateRender(ME::Events::WindowResizedEvent& event);
	bool OnMouseMovedEvent(ME::Events::MouseMovedEvent& event);

	//bool OnKeyInputStartedEvent(ME::Events::KeyInputStartedEvent& event);
	bool OnKeyInputStartedEvent(float32 deltaTime);

private:
	ME::Core::Memory::Reference<ME::Render::RenderPass> m_MainRenderPass;
	ME::Core::Memory::Reference<ME::Render::Pipeline> m_MainGraphicsPipeline;
	ME::Render::VertexBufferLayout m_Layout;

	ME::Core::Memory::Reference<ME::Render::Texture2D> m_StoneTexture;

	ME::Core::Memory::Reference<ME::Render::RUniform> m_CameraBuffer;
	ME::Core::Memory::Reference<ME::Render::RUniform> m_ObjectBuffer;
	ME::Core::Memory::Reference<ME::Render::RUniform> m_SceneBuffer;
	ME::Core::Memory::Reference<ME::Render::RUniform> m_LightBuffer;
	ME::Core::Memory::Reference<ME::Render::IndexBuffer> m_IndexBuffer;
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> m_VertexBuffer;

	ME::Core::Math::Matrix4x4 m_WorldMatrix;
	Light m_Light;

private:
	ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>> m_Flashlight;
	ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Assets::Mesh>> m_Torch;


private:
	ME::Core::Memory::Reference<ME::Render::Camera::Camera> m_Camera;

private:
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

private:
	float32 m_MouseSensitivity = 0.12f;
	float32 m_CameraSpeed = 0.01f;

	bool m_CameraBufferUpdateRequired = false;
	bool m_UpdateQueued = false;

};
