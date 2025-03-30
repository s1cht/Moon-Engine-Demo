#pragma once
#include <Core.h>
#include <Core/Layer/Layer.h>
#include <Renderer/Base/Buffer.h>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/PipelineState.h>
#include <Events/WindowEvents.h>

class SandboxLayer : public Pawn::Core::Layer
{
public:
	SandboxLayer()
		: Layer(TEXT("Example"))
	{
	}
	void OnAttach() override;

	void OnUpdate(float64 deltaTime) override;

	void OnEvent(Pawn::Core::Event & event) override;

private:
	bool SetViewportSize(Pawn::Events::WindowResizedEvent& event);

private:
	Pawn::Core::Memory::Reference<Pawn::Render::BufferLayout> m_Layout;
	Pawn::Core::Memory::Reference<Pawn::Render::PipelineState> m_Primary;
	Pawn::Core::Memory::Reference<Pawn::Render::Shader> m_VertexShader;
	Pawn::Core::Memory::Reference<Pawn::Render::Shader> m_PixelShader;
	Pawn::Core::Memory::Reference<Pawn::Render::VertexBuffer> m_VertexBuffer;
	Pawn::Core::Memory::Reference<Pawn::Render::IndexBuffer> m_IndexBuffer;
private:
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

};
