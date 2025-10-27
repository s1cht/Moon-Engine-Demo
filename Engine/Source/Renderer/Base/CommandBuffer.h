#pragma once

#include <Core.hpp>
#include "RenderObject.hpp"

namespace ME::Render
{
	class MOON_API CommandBuffer : public RenderObject
	{
	public:
		virtual ~CommandBuffer() {};

		virtual void Record() = 0;
		virtual void Finish() = 0;
		virtual void Reset() = 0;

	public:
		static ME::Core::Memory::Reference<Render::CommandBuffer> CreateCommandBuffer();

	private:
		static ME::Core::Memory::Reference<Render::CommandBuffer> CreateVulkanCommandBuffer();

	};
}