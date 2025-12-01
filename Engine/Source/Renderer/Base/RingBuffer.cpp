#include "RingBuffer.hpp"
#include "Renderer/RenderCommand.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Render::RUniform> RUniform::Create(const UniformSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RUniform>(specification);
	}

	ME::Core::Memory::Reference<Render::RIndirectBuffer> RIndirectBuffer::Create(const IndirectBufferSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RIndirectBuffer>(specification);
	}

	ME::Core::Memory::Reference<Render::RFramebuffer> RFramebuffer::Create(const RFramebufferSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RFramebuffer>(specification);
	}

	ME::Core::Memory::Reference<Render::RStorageBuffer> RStorageBuffer::Create(const StorageBufferSpecification& specification)
	{
		return ME::Core::Memory::MakeReference<Render::RStorageBuffer>(specification);
	}

	RFramebuffer::RFramebuffer(const RFramebufferSpecification& specification)
		: m_Specification(specification)
	{
		Init();
	}

    RFramebuffer::~RFramebuffer()
    {
		Shutdown();
    }

    ME::Core::Memory::Reference<Render::Framebuffer> RFramebuffer::AcquireNextBuffer()
	{
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Buffers[m_CurrentBuffer];
	}

	void RFramebuffer::Validate()
	{
		if (m_Buffers.Size() == RenderCommand::Get()->GetSwapChain()->GetFrameCount()) return;

		RecreateBuffers(m_Specification);
	}

	void RFramebuffer::RecreateBuffers(const RFramebufferSpecification& specification)
	{
		Shutdown();
		m_Specification = specification;
		m_CurrentBuffer = 0;

		for (uint32 frameIndex = 0; frameIndex < RenderCommand::Get()->GetSwapChain()->GetFrameCount(); frameIndex++)
		{
			Render::FramebufferSpecification framebufferSpecification = {};
			framebufferSpecification.Attachments = {};
			for (auto& attachment : m_Specification.RAttachments)
				framebufferSpecification.Attachments.EmplaceBack(attachment->GetTextures()[frameIndex]);
			framebufferSpecification.Layers = m_Specification.Layers;
			framebufferSpecification.RenderPass = m_Specification.RenderPass;
			framebufferSpecification.Resolution = m_Specification.Resolution;
			m_Buffers.PushBack(Render::Framebuffer::Create(framebufferSpecification));
		}
	}

	void RFramebuffer::Shutdown()
	{
		if (!m_Buffers.Empty()) return;
		for (auto buffer : m_Buffers)
			buffer->Shutdown();
		m_Buffers.Clear();
	}

	void RFramebuffer::Init()
	{
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 frameIndex = 0; frameIndex < frameCount; frameIndex++)
		{
			Render::FramebufferSpecification framebufferSpecification = {};
			framebufferSpecification.Attachments = {};
			for (auto& attachment : m_Specification.RAttachments)
				framebufferSpecification.Attachments.EmplaceBack(attachment->GetTextures()[frameIndex]);
			framebufferSpecification.Layers = m_Specification.Layers;
			framebufferSpecification.RenderPass = m_Specification.RenderPass;
			framebufferSpecification.Resolution = m_Specification.Resolution;
			m_Buffers.PushBack(Render::Framebuffer::Create(framebufferSpecification));
		}
	}

	RUniform::RUniform(const UniformSpecification& specification)
		: m_Specification(specification)
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
			spec.DebugName = m_Specification.DebugName + TEXT(" frame ")+ Core::ToString(i);

			m_Buffers.PushBack(Render::Uniform::Create(spec));
		}
	}

	RStorageBuffer::RStorageBuffer(const StorageBufferSpecification& specification)
		: m_Specification(specification)
	{
		Init();
	}

	RStorageBuffer::~RStorageBuffer()
	{
		Shutdown();
	}

	ME::Core::Memory::Reference<ME::Render::StorageBuffer> RStorageBuffer::AcquireNextBuffer()
	{
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Buffers[m_CurrentBuffer];
	}

	void RStorageBuffer::Shutdown()
	{
		if (!m_Buffers.Empty()) return;
		for (auto uniform : m_Buffers)
		{
			uniform->Shutdown();
		}
		m_Buffers.Clear();
	}

    void RStorageBuffer::Init()
	{
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 i = 0; i < frameCount; i++)
		{
			StorageBufferSpecification spec = m_Specification;
			spec.DebugName = m_Specification.DebugName + TEXT(" frame ") + Core::ToString(i);

			m_Buffers.PushBack(Render::StorageBuffer::Create(spec));
		}
	}

	RIndirectBuffer::RIndirectBuffer(const IndirectBufferSpecification& specification)
		: m_Specification(specification)
	{
		Init();
	}

	RIndirectBuffer::~RIndirectBuffer()
	{
		Shutdown();
	}

	ME::Core::Memory::Reference<ME::Render::IndirectBuffer> RIndirectBuffer::AcquireNextBuffer()
	{
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Buffers[m_CurrentBuffer];
	}

	void RIndirectBuffer::Shutdown()
	{
		if (!m_Buffers.Empty()) return;
		for (auto uniform : m_Buffers)
		{
			uniform->Shutdown();
		}
		m_Buffers.Clear();
	}

    void RIndirectBuffer::Init()
	{
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 i = 0; i < frameCount; i++)
		{
			IndirectBufferSpecification spec = m_Specification;
			spec.DebugName = m_Specification.DebugName + TEXT(" frame ") + Core::ToString(i);

			m_Buffers.PushBack(Render::IndirectBuffer::Create(spec));
		}
	}

    RTexture2D::RTexture2D(const Texture2DSpecification& specification)
		: m_Specification(specification)
    {
		Init();
    }

    RTexture2D::~RTexture2D()
    {
		Shutdown();
    }

    ME::Core::Memory::Reference<Render::Texture2D> RTexture2D::AcquireNextTexture()
    {
		m_CurrentBuffer = (m_CurrentBuffer + 1) % RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		return m_Textures[m_CurrentBuffer];
    }

    void RTexture2D::Shutdown()
    {
		if (!m_Textures.Empty()) return;
		for (auto texture : m_Textures)
		{
			texture->Shutdown();
		}
		m_Textures.Clear();
    }

    ME::Core::Memory::Reference<Render::RTexture2D> RTexture2D::Create(const Texture2DSpecification& specification)
    {
		return ME::Core::Memory::MakeReference<Render::RTexture2D>(specification);
    }

    void RTexture2D::Init()
    {
		uint32 frameCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
		for (uint32 i = 0; i < frameCount; i++)
		{
			Texture2DSpecification spec = m_Specification;
			spec.DebugName = m_Specification.DebugName + TEXT(" frame ") + Core::ToString(i);

			m_Textures.PushBack(Render::Texture2D::Create(spec));
		}
    }
}
