#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>

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

	enum class ShaderStage : uint32
	{
		None		= 0,
		Vertex		= BIT(0),
		Hull		= BIT(1),
		Domain		= BIT(2),
		Geometry	= BIT(3),
		Pixel		= BIT(4),
		Compute		= BIT(5),
		Task		= BIT(6),
		Mesh		= BIT(7),
	};

	struct CompiledShader
	{
		void* Bytecode;
		SIZE_T Size;
		ShaderFormat Format;
	};

	enum class ResourceType : uint8
	{
		None = 0,
		Uniform, StorageBuffer,
		Texture1D, Texture2D, Texture3D,
		Sampler,
	};

	struct ResourceBinding
	{
		ME::Render::ResourceType Type;
		ME::Render::ShaderStage Stage;

	    inline bool operator==(const ResourceBinding& binding) const
		{
			return this->Stage == binding.Stage && this->Type == binding.Type;
		}
	};

	class ResourceLayout : public ME::Core::Array<ME::Render::ResourceBinding>
	{
	public:
		bool operator==(const ResourceLayout& layout) const
		{
			if (layout.Size() != this->Size()) return false;
			for (SIZE_T i = 0; i < layout.Size(); i++)
				if (this->operator[](i) != layout[i]) return false;
			return true;
		}
	};

	class MEAPI ResourceLayoutPack : public ME::Core::Array<ME::Render::ResourceLayout>
	{
	public:
		bool operator==(const ResourceLayoutPack& pack) const
		{
			if (pack.Size() != this->Size()) return false;
			for (SIZE_T i = 0; i < pack.Size(); i++)
				if (this->operator[](i) != pack[i]) break;
			return true;
		}
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

	public:
		static ME::Core::Memory::Reference<Shader> CreateVulkanShader(const ShaderSpecification& specification);

	};

	extern MEAPI SIZE_T SizeOfShaderType(ME::Render::ShaderType type);
	extern MEAPI uint32 GetTypeAPISpecificShaderType(ME::Render::ShaderType type);

	inline constexpr MEAPI ShaderStage operator|(ME::Render::ShaderStage a, ME::Render::ShaderStage b)
	{
		return static_cast<ShaderStage>(static_cast<uint32>(a) | static_cast<uint32>(b));
	}

	inline constexpr MEAPI ShaderStage operator|=(ME::Render::ShaderStage& a, ME::Render::ShaderStage b)
	{
		a = a | b;
		return a;
	}

	inline constexpr ShaderStage operator&(ShaderStage a, ShaderStage b)
	{
		return static_cast<ShaderStage>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}

	/*
API-specific conversion function
*/
	MEAPI uint32 ConvertShaderTypeVulkan(ShaderType type);

#ifdef PLATFORM_WINDOWS
	//extern MEAPI uint32 ConvertShaderTypeDirectX12(ShaderType type);
#else
	//extern MEAPI uint32 ConvertShaderTypeDirectX12(ShaderType type) { ME_ASSERT(false, TEXT("DirectX12 conversion available only on Windows!")); return 0; }
#endif
}

