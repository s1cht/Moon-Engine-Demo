#pragma once

#include <Core.hpp>

#include "RenderObject.hpp"

namespace ME::Render
{
	enum class ShaderType : SIZE_T
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Uint, Uint2, Uint3, Uint4,
		Bool
	};

	enum class ShaderFormat : uint8
	{
		None = 0,
		SPIRV, DXIL, Metal
	};

	struct CompiledShader
	{
		void* Bytecode;
		SIZE_T Size;
		ShaderFormat Format;
	};

	class MOON_API Shader : public RenderObject
	{
	public:
		enum class Type
		{
			None = 0,
			Vertex, Pixel, Compute, Geometry, Hull, Domain
		};;

	public:
		virtual ~Shader() = default;

		virtual inline ME::Render::CompiledShader GetCompiledShader() = 0;
		virtual constexpr Shader::Type GetShaderType() const = 0;

	public:
		static ME::Core::Memory::Reference<Shader> Create(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType);

	public:
		static ME::Core::Memory::Reference<Shader> CreateVulkanShader(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType);

	};

	extern MOON_API SIZE_T SizeOfShaderType(ME::Render::ShaderType type);
	extern MOON_API uint32 GetTypeAPISpecificShaderType(ME::Render::ShaderType type);

	/*
API-specific conversion function
*/
MOON_API uint32 ConvertShaderTypeVulkan(ShaderType type);

#ifdef PLATFORM_WINDOWS
	//extern MOON_API uint32 ConvertShaderTypeDirectX12(ShaderType type);
#else
	//extern MOON_API uint32 ConvertShaderTypeDirectX12(ShaderType type) { ME_ASSERT(false, TEXT("DirectX12 conversion available only on Windows!")); return 0; }
#endif
}

