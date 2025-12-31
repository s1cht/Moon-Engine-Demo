#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Rect2D.hpp>
#include <Core/Containers/Array.hpp>

#include "RenderCore.hpp"

namespace ME
{
	class Window;
}

namespace ME::Render
{
	class MEAPI SwapChain : public RenderObject
	{
	public:
		virtual void Resize(uint32 x, uint32 y) = 0;
		virtual void SetFullscreen(bool fullscreen) = 0;
		virtual void SetVSync(bool enabled) = 0;

		virtual uint32 GetFrameIndex() const = 0;
		virtual uint32 GetImageIndex() const = 0;
		virtual uint32 GetFrameCount() const = 0;
		virtual uint32 GetLastImageIndex() const = 0;

		virtual Core::Math::Resolution2D<uint32> GetExtent() = 0;
		virtual ME::Core::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> GetImages() = 0;

		virtual bool UpdateRequired() const = 0;
		virtual void Updated() = 0;

	public:
		static ME::Core::Memory::Reference<SwapChain> Create(int32& result);

	private:
		static ME::Core::Memory::Reference<SwapChain> CreateVulkan(int32& result);
	};
};

