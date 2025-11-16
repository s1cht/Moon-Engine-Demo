#include "VulkanCommandBuffer.hpp"

#include "Renderer/RenderCommand.hpp"
#include "Renderer/RenderResourcesTracker.hpp"
#include "Renderer/API/Vulkan/VulkanRenderAPI.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateVulkanCommandBuffer()
	{
		auto object = ME::Core::Memory::Reference<Render::CommandBuffer>(new VulkanCommandBuffer());
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		Init();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
	}

	void VulkanCommandBuffer::Shutdown()
	{
		m_Buffer = nullptr;
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
            ME_ASSERT(false, "Vulkan command buffer allocation failed! Error: {0}", static_cast<int32>(result));

		return;
	}



}
