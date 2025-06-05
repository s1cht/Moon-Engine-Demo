#pragma once

#include <Core.hpp>

#include <vulkan/vulkan.hpp>
#include "Renderer/Base/CommandBuffer.h"

namespace Pawn::Render
{
	class PAWN_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer();
		~VulkanCommandBuffer() override;

	public:
		void Record() override;
		void Finish() override;
		void Reset() override;

	public:
		inline VkCommandBuffer GetBuffer() { return m_Buffer; }

	private:
		void Init();

	private:
		VkCommandBuffer m_Buffer;

	};
}
	
