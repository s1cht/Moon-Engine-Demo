#pragma once

#include <Core.hpp>
#include "Renderer/Base/Buffer.h"
#include "Renderer/Base/Framebuffer.h"

// R - stands for ring
namespace ME::Render
{
	class MOON_API RFramebuffer
	{
	public:


	};

	class MOON_API RUniform
	{
	public:
		RUniform(const UniformSpecification& specification);
		~RUniform();

	public:
		inline ME::Core::Memory::Reference<Render::Uniform> AcquireNextBuffer();
		void RecreateBuffers();
		void Shutdown();

	public:
		static ME::Core::Memory::Reference<Render::RUniform> Create(const UniformSpecification& specification);

	private:
		void Init();

	private:
		uint32 m_CurrentBuffer;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Render::Uniform>> m_Buffers;

		UniformSpecification m_Specification;

	};
}