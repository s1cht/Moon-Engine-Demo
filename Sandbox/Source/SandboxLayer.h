#pragma once
#include <Core.h>
#include <Core/Layer/Layer.h>
#include <Renderer/Base/Buffer.h>
#include <Renderer/Base/Shader.h>
#include <Renderer/Base/PipelineState.h>
#include <Events/WindowEvents.h>

class SandboxLayer : public Pawn::Layer
{
public:
	SandboxLayer()
		: Layer(TEXT("Example"))
	{
	}
	void OnAttach() override;

	void OnUpdate(float64 deltaTime) override;

	void OnEvent(Pawn::Event& event) override;

private:
	bool SetViewportSize(Pawn::WindowResizedEvent& event);

private:
	Pawn::Memory::Reference<Pawn::Render::BufferLayout> m_Layout;
	Pawn::Memory::Reference<Pawn::Render::PipelineState> m_Primary;
	Pawn::Memory::Reference<Pawn::Render::Shader> m_VertexShader;
	Pawn::Memory::Reference<Pawn::Render::Shader> m_PixelShader;
	Pawn::Memory::Reference<Pawn::Render::VertexBuffer> m_VertexBuffer;
	Pawn::Memory::Reference<Pawn::Render::IndexBuffer> m_IndexBuffer;
private:
	uint32 m_WindowWidth;
	uint32 m_WindowHeight;

};
