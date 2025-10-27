#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/CommandBuffer.h"

namespace ME::Render
{
	class MOON_API VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer();
		~VulkanCommandBuffer() override;

	public:
		void Shutdown() override;
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
	
