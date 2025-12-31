#pragma once
#include <Core.hpp>

#include "RenderCore.hpp"

namespace ME::Render
{
	class MEAPI CommandBuffer : public RenderObject
	{
	public:
		virtual void Record() = 0;
		virtual void Finish() = 0;
		virtual void Reset() = 0;

	public:
		static ME::Core::Memory::Reference<Render::CommandBuffer> CreateCommandBuffer();

	private:
		static ME::Core::Memory::Reference<Render::CommandBuffer> CreateVulkan();

	};
}