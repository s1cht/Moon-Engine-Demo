#include "RingBuffer.h"

#include "Renderer/RenderCommand.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<Render::RUniform> RUniform::Create(const UniformSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RUniform>(specification);
	}

	ME::Core::Memory::Reference<Render::RFramebuffer> RFramebuffer::Create(
		const FramebufferSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RFramebuffer>(specification);
	}

	RFramebuffer::RFramebuffer(const FramebufferSpecification& specification)
		: m_Specification(specification), m_CurrentBuffer(0)
	{
		Init();
	}

	RFramebuffer::~RFramebuffer()
	{
	}

	ME::Core::Memory::Reference<Render::Framebuffer> RFramebuffer::AcquireNextBuffer()
	{
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Buffers[m_CurrentBuffer];
	}

	void RFramebuffer::Validate()
	{
		if (m_Buffers.GetSize() == RenderCommand::Get()->GetSwapChain()->GetFrameCount()) return;

		RecreateBuffers(m_Specification);
	}

	void RFramebuffer::RecreateBuffers(const FramebufferSpecification& specification)
	{
		Shutdown();
		m_Specification = specification;
		m_CurrentBuffer = 0;

		for (uint32 i = 0; i < RenderCommand::Get()->GetSwapChain()->GetFrameCount(); i++)
		{
			m_Buffers.PushBack(Render::Framebuffer::Create(m_Specification));
		}
	}

	void RFramebuffer::Shutdown()
	{
		if (!m_Buffers.Empty()) return;
		for (auto buffer : m_Buffers)
		{
			buffer->Shutdown();
		}
		m_Buffers.Clear();
	}

	void RFramebuffer::Init()
	{
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 i = 0; i < frameCount; i++)
		{
			m_Buffers.PushBack(Render::Framebuffer::Create(m_Specification));
		}
	}

	RUniform::RUniform(const UniformSpecification& specification)
		: m_CurrentBuffer(0), m_Specification(specification)
	{
		Init();
	}

	RUniform::~RUniform()
	{
		Shutdown();
	}

	ME::Core::Memory::Reference<ME::Render::Uniform> RUniform::AcquireNextBuffer()
	{
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Buffers[m_CurrentBuffer];
	}

	void RUniform::RecreateBuffers()
	{

	}

	void RUniform::Shutdown()
	{
		if (!m_Buffers.Empty()) return;
		for (auto uniform : m_Buffers)
		{
			uniform->Shutdown();
		}
		m_Buffers.Clear();
	}

	void RUniform::Init()
	{
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 i = 0; i < frameCount; i++)
		{
			UniformSpecification spec = m_Specification;
			spec.DebugName = m_Specification.DebugName + TEXT(" frame ") + Core::Containers::ToString(i);

			m_Buffers.PushBack(Render::Uniform::Create(spec));
		}
	}
}
