#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Rect2D.hpp>
#include <Core/Containers/Array.hpp>

#include "RenderCore.hpp"

namespace ME::Render
{
	struct FramebufferSpecification
	{
		Core::Memory::Reference<Render::RenderPass> RenderPass;
		ME::Core::Math::Resolution2D<uint32> Resolution;
		ME::Core::Array<ME::Core::Memory::Reference<Render::Texture2D>> Attachments;
		uint32 Layers;
		ME::Core::String DebugName;
	};

	class MEAPI Framebuffer : public RenderObject
	{
		ME_RENDER_OBJECT_TYPE(Framebuffer);

	public:
		virtual inline const FramebufferSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Render::Framebuffer> Create(const FramebufferSpecification& specification);

	private:
	    static ME::Core::Memory::Reference<Render::Framebuffer> CreateVulkan(const FramebufferSpecification& specification);
	};
}

