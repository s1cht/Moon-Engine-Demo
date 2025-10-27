#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/Array.hpp>

#include "RenderObject.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace ME::Render
{
	class Texture2D;
}

namespace ME
{
	class Window;
	
	namespace Render
	{
		class MOON_API SwapChain : public RenderObject
		{
		public:
			virtual void Resize(uint32 x, uint32 y) = 0;
			virtual void SetFullscreen(bool fullscreen) = 0;
			virtual void SetVSync(bool enabled) = 0;

			virtual uint32 GetFrameIndex() const = 0;
			virtual uint32 GetFrameCount() const = 0;

			virtual Core::Math::Resolution2D<uint32> GetExtent() = 0;
			virtual ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> GetImages() = 0;

			virtual bool UpdateRequired() const = 0;
			virtual void Updated() = 0;

		public:
			static ME::Core::Memory::Reference<SwapChain> Create(int32& result, Window* window);

		private:
			static ME::Core::Memory::Reference<SwapChain> CreateVulkanSwapChain(int32& result);

		};
	};
};

