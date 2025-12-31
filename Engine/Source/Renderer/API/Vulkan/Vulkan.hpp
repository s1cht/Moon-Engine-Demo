#pragma once

#include <Core.hpp>

#ifdef PLATFORM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
	#include "Platform/Windows/Win32Window.hpp"
	#include "Platform/Windows/Win32Platform.hpp"
#endif

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include "Renderer/Base/RenderCore.hpp"
#include "VulkanMacros.hpp"

namespace ME::Render
{
	class VulkanRenderAPI;
	class VulkanResourceHandler;
	class VulkanSwapChain;
	class VulkanPipeline;
	class VulkanRenderPass;
	class VulkanShader;

	class VulkanFramebuffer;

	class VulkanCommandBuffer;

	class VulkanUniform;
	class VulkanStorageBuffer;
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;
	class VulkanIndirectBuffer;

	class VulkanTexture1D;
	class VulkanTexture2D;
	class VulkanTexture3D;
}
