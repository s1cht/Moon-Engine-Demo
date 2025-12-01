#pragma once
#include <Core.hpp>
#include <Core/Layer/Layer.hpp>
#include <Core/Math/Math.hpp>

#include "Application/Application.hpp"
#include "Framework/Entities/Block.hpp"
#include "Framework/Entities/Camera.hpp"
#include "Framework/Entities/Music.hpp"

class SandboxLayer : public ME::Core::Layer
{
public:
	SandboxLayer();
	void OnAttach() override;

	void OnUpdate(float64 deltaTime) override;
	void OnRender() override;
	void OnImGuiRender(float64 deltaTime, ImGuiContext* dllContext) override;

	void OnEvent(ME::Core::Event & event) override;

private:
	float32 m_MouseSensitivity = 0.12f;
	float32 m_CameraSpeed = 0.01f;

	ME::Core::Memory::Reference<ME::Block> m_Block;
	ME::Core::Memory::Reference<ME::Music> m_DWorld;
	ME::Core::Memory::Reference<ME::PerspectiveCamera> m_Camera;
};
