#include "VulkanCommandBuffer.hpp"

#include "Renderer/RenderCommand.hpp"

#include "Renderer/API/Vulkan/VulkanRenderAPI.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateVulkan()
	{
		auto object = ME::Core::Memory::MakeReference<VulkanCommandBuffer>();
		return object;
	}

	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		Init();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		Shutdown();
	}

	void VulkanCommandBuffer::Shutdown()
	{
		VulkanRenderAPI* render = RenderCommand::Get()->As<VulkanRenderAPI>();
		if (m_Buffer != nullptr)
		{
			vkFreeCommandBuffers(render->GetDevice(), render->GetCommandPool(), 1, &m_Buffer);
		    m_Buffer = nullptr;
		}
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
        VulkanRenderAPI* render = RenderCommand::Get()->As<VulkanRenderAPI>();

		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandBufferCount = 1;
		info.commandPool = render->GetCommandPool();
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult result = vkAllocateCommandBuffers(render->GetDevice(), &info, &m_Buffer);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("CommandBuffer", "Failed to allocate buffer! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
		}
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_COMMAND_BUFFER);
	}
}
