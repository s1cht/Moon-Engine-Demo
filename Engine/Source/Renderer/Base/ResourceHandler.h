#pragma once
#include "RenderObject.hpp"
#include "Shader.h"

namespace ME::Render
{
	class Uniform;
	class StorageBuffer;
	class CommandBuffer;
	class Pipeline;
}

namespace ME::Render
{
	class MOON_API ResourceHandler : public RenderObject
	{
	public:
		virtual uint32 CreateLayout(const ME::Render::ResourceLayout& layout) = 0;
		virtual uint32 CreateResourceSet(uint32 layout) = 0;
		virtual uint32 CreateResourceSet(const ME::Render::ResourceLayout& layout) = 0;

		virtual uint32 GetResourceLayoutIndex(const ME::Render::ResourceLayout& layout) = 0;

		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) = 0;
		virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) = 0;
		virtual void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex) = 0;

	public:
		static ME::Core::Memory::Reference<ME::Render::ResourceHandler> Create();

	private:
		static ME::Core::Memory::Reference<ME::Render::ResourceHandler> CreateVulkan();

	};
}
