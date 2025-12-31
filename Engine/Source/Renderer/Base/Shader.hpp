#pragma once
#include <Core.hpp>

#include "RenderCore.hpp"

namespace ME::Render
{
	struct CompiledShader
	{
		void* Bytecode;
		SIZE_T Size;
		ShaderFormat Format;
	};

	struct ShaderSpecification
	{
		ME::Render::CompiledShader CompiledShader;
		ME::Render::ShaderStage Stage;
		ME::Render::ResourceLayoutPack Layouts;
	};

	class MEAPI Shader : public RenderObject
	{
	public:
		virtual const ME::Render::ShaderSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Shader> Create(const ShaderSpecification& specification);

	private:
		static ME::Core::Memory::Reference<Shader> CreateVulkan(const ShaderSpecification& specification);
	};
}

