#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>

namespace Pawn
{
	class Window;
	
	namespace Render
	{
		class PAWN_API SwapChain
		{
		public:
			virtual void Shutdown() = 0;

			virtual void Present() = 0;
			virtual void Resize(uint32 x, uint32 y) = 0;
			virtual void SetFullscreen(bool fullscreen) = 0;
			virtual void SetVSync(bool enabled) = 0;

		public:
			static SwapChain* Create(int32& result, Window* window);

		private:
			static SwapChain* CreateVulkanSwapChain(int32& result);
			static SwapChain* CreateDirectX11SwapChain(int32& result, Window* window);

		};
	};
};

