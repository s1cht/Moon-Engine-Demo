#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/Array.hpp>

namespace Pawn::Render
{
	class Texture2D;
}

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

			virtual Pawn::Core::Containers::Array<Texture2D*>& GetImages() = 0;

		public:
			static SwapChain* Create(int32& result, Window* window);

		private:
			static SwapChain* CreateVulkanSwapChain(int32& result);
			static SwapChain* CreateDirectX11SwapChain(int32& result, Window* window);

		};
	};
};

