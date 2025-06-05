#pragma once

#include <Core.hpp>

namespace Pawn::Render
{
	class PAWN_API CommandBuffer
	{
	public:
		virtual ~CommandBuffer() {};

		virtual void Record() = 0;
		virtual void Finish() = 0;
		virtual void Reset() = 0;

	public:
		static CommandBuffer* CreateCommandBuffer();

	private:
		static CommandBuffer* CreateVulkanCommandBuffer();
		//static CommandBuffer* CreateDirectX11CommandBuffer();

	};
}