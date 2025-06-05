#include "VulkanCommandBuffer.h"
#include <Core.hpp>

#include "Renderer/RenderCommand.h"
#include "Renderer/API/Vulkan/VulkanMacros.hpp"
#include "Renderer/API/Vulkan/VulkanRenderer.h"

namespace Pawn::Render
{
	CommandBuffer* CommandBuffer::CreateVulkanCommandBuffer()
	{
		return new VulkanCommandBuffer();
	}

	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		Init();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::Record()
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(m_Buffer, &beginInfo);
	}

	void VulkanCommandBuffer::Finish()
	{
		vkEndCommandBuffer(m_Buffer);
	}

	void VulkanCommandBuffer::Reset()
	{
		vkResetCommandBuffer(m_Buffer, 0);
	}

	void VulkanCommandBuffer::Init()
	{
		VkResult result;
		VulkanRenderer* render = static_cast<VulkanRenderer*>(RenderCommand::Get());

		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandBufferCount = 1;
		info.commandPool = render->GetCommandPool();
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		result = vkAllocateCommandBuffers(render->GetDevice(), &info, &m_Buffer);
		if (PE_VK_FAILED(result))
			PE_ASSERT(false, TEXT("Vulkan command buffer allocation failed! Error: {0}"), (int32)result);

		return;
	}



}
