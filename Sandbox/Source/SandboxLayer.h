#pragma once
#include <Core.hpp>
#include <Core/Layer/Layer.hpp>
#include <Renderer/Base/Buffer.hpp>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/Pipeline.h>
#include <Renderer/Camera/Camera.h>
#include <Events/WindowEvents.h>
#include <Renderer/Assets/Mesh.hpp>

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
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

private:
	float32 m_MouseSensitivity = 0.12f;
	float32 m_CameraSpeed = 0.01f;

	bool m_CameraBufferUpdateRequired = false;
	bool m_UpdateQueued = false;

};
