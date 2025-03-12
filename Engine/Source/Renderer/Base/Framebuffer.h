#pragma once

#include <Core.h>

namespace Pawn::Render
{
	class PAWN_API Framebuffer
	{
	public:
		virtual bool Bind() = 0;
		virtual bool Unbind() = 0;

		virtual void Shutdown() = 0;

	private:
		static Framebuffer* Create(uint32 x, uint32 y);
		static Framebuffer* CreateDirectX11Framebuffer(uint32 x, uint32 y);

	};
}

