#pragma once

#include <Core.hpp>
#include "Renderer/Base/Buffer.hpp"
#include "Renderer/Base/Texture.hpp"
#include "Renderer/Base/Framebuffer.hpp"

// R - stands for ring
namespace ME::Render
{
    struct RFramebufferSpecification : FramebufferSpecification
    {
		ME::Core::Array<ME::Core::Memory::Reference<Render::RTexture2D>> RAttachments;
    };

	class MEAPI RingBuffer
	{
	public:
		RingBuffer() : m_CurrentBuffer(0) {}
		virtual ~RingBuffer() = default;

	    uint32 GetCurrentBuffer() const { return m_CurrentBuffer; }

	protected:
		uint32 m_CurrentBuffer;
	};

	class MEAPI RFramebuffer final : public RingBuffer
	{
	public:
		RFramebuffer(const RFramebufferSpecification& specification);
		~RFramebuffer() override;

	public:
		inline ME::Core::Memory::Reference<Render::Framebuffer> AcquireNextBuffer();
		void Validate();
		void RecreateBuffers(const RFramebufferSpecification& specification);
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RFramebuffer> Create(const RFramebufferSpecification& specification);

	private:
		void Init();

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Render::Framebuffer>> m_Buffers;

		RFramebufferSpecification m_Specification;
	};

	class MEAPI RUniform final : public RingBuffer
	{
	public:
		RUniform(const UniformSpecification& specification);
		~RUniform() override;

	public:
		inline ME::Core::Memory::Reference<Render::Uniform> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RUniform> Create(const UniformSpecification& specification);

	private:
		void Init();

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Render::Uniform>> m_Buffers;

		UniformSpecification m_Specification;
	};

	class MEAPI RStorageBuffer final : public RingBuffer
	{
	public:
		RStorageBuffer(const StorageBufferSpecification& specification);
		~RStorageBuffer() override;

	public:
		inline ME::Core::Memory::Reference<Render::StorageBuffer> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RStorageBuffer> Create(const StorageBufferSpecification& specification);

	private:
		void Init();

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Render::StorageBuffer>> m_Buffers;

		StorageBufferSpecification m_Specification;
	};

	class MEAPI RIndirectBuffer final : public RingBuffer
	{
	public:
		RIndirectBuffer(const IndirectBufferSpecification& specification);
		~RIndirectBuffer() override;

	public:
		inline ME::Core::Memory::Reference<Render::IndirectBuffer> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RIndirectBuffer> Create(const IndirectBufferSpecification& specification);

	private:
		void Init();

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Render::IndirectBuffer>> m_Buffers;

		IndirectBufferSpecification m_Specification;
	};

	class MEAPI RTexture2D final : public RingBuffer
	{
	public:
		RTexture2D(const Texture2DSpecification& specification);
		~RTexture2D() override;

	public:
		inline ME::Core::Memory::Reference<Render::Texture2D> AcquireNextTexture();
		void Shutdown();

		const Texture2DSpecification& GetSpecification() const { return m_Specification; }
		ME::Core::Array<ME::Core::Memory::Reference<Render::Texture2D>>& GetTextures() { return m_Textures; }

	public:
		static ME::Core::Memory::Reference<Render::RTexture2D> Create(const Texture2DSpecification& specification);

	private:
		void Init();

	private:
		ME::Core::Array<ME::Core::Memory::Reference<Render::Texture2D>> m_Textures;

		Texture2DSpecification m_Specification;
	};
}