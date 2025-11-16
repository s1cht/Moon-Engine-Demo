#pragma once

#include <Core.hpp>
#include "Renderer/Base/Buffer.hpp"
#include "Renderer/Base/Framebuffer.hpp"

// R - stands for ring
namespace ME::Render
{
	class MEAPI RFramebuffer
	{
	public:
		RFramebuffer(const FramebufferSpecification& specification);
		~RFramebuffer();

	public:
		inline ME::Core::Memory::Reference<Render::Framebuffer> AcquireNextBuffer();
		void Validate();
		void RecreateBuffers(const FramebufferSpecification& specification);
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RFramebuffer> Create(const FramebufferSpecification& specification);

	private:
		void Init();

	private:
		uint32 m_CurrentBuffer;
		ME::Core::Array<ME::Core::Memory::Reference<Render::Framebuffer>> m_Buffers;

		FramebufferSpecification m_Specification;
	};

	class MEAPI RUniform
	{
	public:
		RUniform(const UniformSpecification& specification);
		~RUniform();

	public:
		inline ME::Core::Memory::Reference<Render::Uniform> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RUniform> Create(const UniformSpecification& specification);

	private:
		void Init();

	private:
		uint32 m_CurrentBuffer;
		ME::Core::Array<ME::Core::Memory::Reference<Render::Uniform>> m_Buffers;

		UniformSpecification m_Specification;
	};

	class MEAPI RStorageBuffer
	{
	public:
		RStorageBuffer(const StorageBufferSpecification& specification);
		~RStorageBuffer();

	public:
		inline ME::Core::Memory::Reference<Render::StorageBuffer> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RStorageBuffer> Create(const StorageBufferSpecification& specification);

	private:
		void Init();

	private:
		uint32 m_CurrentBuffer;
		ME::Core::Array<ME::Core::Memory::Reference<Render::StorageBuffer>> m_Buffers;

		StorageBufferSpecification m_Specification;
	};

	class MEAPI RIndirectBuffer
	{
	public:
		RIndirectBuffer(const IndirectBufferSpecification& specification);
		~RIndirectBuffer();

	public:
		inline ME::Core::Memory::Reference<Render::IndirectBuffer> AcquireNextBuffer();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RIndirectBuffer> Create(const IndirectBufferSpecification& specification);

	private:
		void Init();

	private:
		uint32 m_CurrentBuffer;
		ME::Core::Array<ME::Core::Memory::Reference<Render::IndirectBuffer>> m_Buffers;

		IndirectBufferSpecification m_Specification;
	};

	class MEAPI RTexture2D
	{
	public:
		RTexture2D(const Texture2DSpecification& specification);
		~RTexture2D();

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
		uint32 m_CurrentBuffer;
		ME::Core::Array<ME::Core::Memory::Reference<Render::Texture2D>> m_Textures;

		Texture2DSpecification m_Specification;
	};
}