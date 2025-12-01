#pragma once
#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/CommandBuffer.hpp"

namespace ME::Render
{
	class MEAPI VulkanCommandBuffer : public CommandBuffer
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
		inline VkCommandBuffer GetCommandBuffer() { return m_Buffer; }

	private:
		void Init();

	private:
		VkCommandBuffer m_Buffer;
	};
}
	
