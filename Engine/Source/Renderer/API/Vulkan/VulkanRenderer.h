#pragma once

#include <Core.h>
#include <vulkan/vulkan.hpp>
#include "EngineDefines.h"
#include "Renderer/Base/RendererAPI.h"

namespace Pawn::Render
{
	class PAWN_API VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

	public:
		void PostInit() override;

		void Present() override;
		void Clear(Pawn::Core::Math::Vector4D32 color) override;
		void DrawIndexed(uint32 indexCount, uint32 index) override;
		void Shutdown() override;

		void OnWindowEvent(int32 x, int32 y) override;

		void BindBackBuffer() override;
		void UnbindBackBuffer() override;

	private:
		int32 Init();

	private:
		int32 CreateInstance(const char* ApplicationName, int32 versionNum1, int32 versionNum2, int32 versionNum3);

	private:
		VkInstance m_Instance;

	};
}
