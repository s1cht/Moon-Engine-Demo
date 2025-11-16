#pragma once
#include "RenderObject.hpp"
#include "Shader.hpp"

namespace ME::Render
{
    struct BarrierInfo;
    class IndexBuffer;
}

namespace ME::Render
{
    class VertexBuffer;
}

namespace ME::Render
{
    class IndirectBuffer;
}

namespace ME::Render
{
    class Texture;
    class Texture1D;
	class Texture2D;
	class Texture3D;
	class Uniform;
	class StorageBuffer;
	class CommandBuffer;
	class Pipeline;
}

namespace ME::Render
{
	class MEAPI ResourceHandler : public RenderObject
	{
	public:
		virtual uint32 CreateLayout(const ME::Render::ResourceLayout& layout) = 0;

		virtual uint32 CreateResourceSet(uint32 layout) = 0;
		virtual uint32 CreateResourceSet(const ME::Render::ResourceLayout& layout) = 0;

		virtual uint32 GetResourceSet(uint32 layout) = 0;
		virtual uint32 GetResourceSet(const ME::Render::ResourceLayout& layout) = 0;

		virtual uint32 GetTextureSet(uint32 layout) = 0;
		virtual uint32 GetTextureSet(const ME::Render::ResourceLayout& layout) = 0;

		virtual bool IncrementTextureSetReference(uint32 set) = 0;
		virtual void DecrementTextureSetReference(uint32 set) = 0;

		virtual uint32 GetResourceLayoutIndex(const ME::Render::ResourceLayout& layout) = 0;

		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) = 0;
		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) = 0;
		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::IndirectBuffer> buffer) = 0;
		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::VertexBuffer> buffer) = 0;
		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::IndexBuffer> buffer) = 0;
		virtual void WriteTexture() = 0;

	    virtual void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture1D> texture) = 0;
		virtual void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture2D> texture) = 0;
		virtual void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture3D> texture) = 0;

	    virtual void ClearTextureQueue() = 0;

	    virtual void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex) = 0;
		virtual void BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline,
			uint32 set, uint32 setIndex) = 0;

	    virtual void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src, BarrierInfo dst) = 0;

		virtual void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::Texture1D>& texture, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::Texture2D>& texture, BarrierInfo src, BarrierInfo dst) = 0;
		virtual void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, const ME::Core::Memory::Reference<Render::Texture3D>& texture, BarrierInfo src, BarrierInfo dst) = 0;

	public:
		static ME::Core::Memory::Reference<ME::Render::ResourceHandler> Create(uint32 bufferCount);

	private:
		static ME::Core::Memory::Reference<ME::Render::ResourceHandler> CreateVulkan(uint32 bufferCount);

	};
}
