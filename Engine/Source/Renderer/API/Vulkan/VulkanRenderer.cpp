#include "VulkanRenderer.h"
#include "Application/Application.h"
#include <Core/Misc/Assertion.h>

import Pawn.Core.Container.Array;

namespace Pawn::Render
{
    inline RendererAPI* RendererAPI::CreateVulkan()
    {
        return new VulkanRenderer();
    }

    VulkanRenderer::VulkanRenderer()
    {
        Init();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Shutdown();
    }

    void VulkanRenderer::PostInit()
    {
    }

    void VulkanRenderer::Present()
    {
    }

    void VulkanRenderer::Clear(Pawn::Core::Math::Vector4D32 color)
    {
    }

    void VulkanRenderer::DrawIndexed(uint32 indexCount, uint32 index)
    {
    }

    void VulkanRenderer::Shutdown()
    {
        if (m_Instance != nullptr)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = nullptr;
        }
    }

    void VulkanRenderer::OnWindowEvent(int32 x, int32 y)
    {
    }

    void VulkanRenderer::BindBackBuffer()
    {
    }

    void VulkanRenderer::UnbindBackBuffer()
    {
    }

    int32 VulkanRenderer::Init()
    {
        int32 result;

        result = CreateInstance("App", 1, 0, 0);
        if (result != 0)
        {
            PE_ASSERT(TEXT("Vulkan instance creation failed! Error: {0}"), result);
            //Shutdown();
        }

        return result;
    }

    int32 VulkanRenderer::CreateInstance(const char* applicationName, int32 versionMajor, int32 versionMinor, int32 versionPatch)
    {
        VkResult result;
		uint32 extensionCount;
        Pawn::Core::Containers::Array<VkExtensionProperties> extensions;
        const Pawn::ApplicationData& appData = Pawn::Application::Get().GetAppData();

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = Pawn::Core::Containers::StringToAnsiString(appData.ApplicationName).GetString();
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appData.VersionMajor, appData.VersionMinor, appData.VersionPatch);
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VER_MAJOR, ENGINE_VER_MINOR, ENGINE_VER_PATCH);
        appInfo.apiVersion = VK_API_VERSION_1_0;

  //      extensionCount = 0;
  //      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  //      extensions.Reserve(extensionCount);
  //      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.Data());

  //      extensions.SetSize(extensionCount);

  //      for (auto& val : extensions)
  //          PE_INFO("Extension \t{0}; \n\tVersion: {1}", val.extensionName, val.specVersion);

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = 0;
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledExtensionNames = nullptr;
        instanceInfo.ppEnabledLayerNames = nullptr;
        instanceInfo.flags = 0;

        result = vkCreateInstance(&instanceInfo, nullptr, &m_Instance);

        return (int32)result;
    }
}