#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Renderer/Base/Shader.h"

namespace Pawn::Render
{
	enum class ShaderType : SIZE_T
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Uint, Uint2, Uint3, Uint4,
		Mat3x3, Mat4x4,
		Bool
	};

	enum class Usage
	{
		Default = 0,
		Immutable,
		Dynamic,
		Staging
	};

	extern PAWN_API SIZE_T SizeOfShaderType(Pawn::Render::ShaderType type);
	extern PAWN_API uint32 GetTypeAPISpecificShaderType(Pawn::Render::ShaderType type);

	struct BufferElement
	{
		ShaderType Type;
		Core::Containers::AnsiString Name;
		SIZE_T Size;
		SIZE_T Offset;
		uint32 SemanticIndex;
		uint32 InputSlot;

		BufferElement(ShaderType type, Core::Containers::AnsiString name, uint32 semanticIndex, uint32 inputSlot)
			: Type(type), Name(name), Size(SizeOfShaderType(type)), Offset(0), SemanticIndex(semanticIndex), InputSlot(inputSlot)
		{
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout()
			: m_Elements({}), m_Stride(0)
		{
		}

		BufferLayout(std::initializer_list<BufferElement> elements)
			: m_Elements(elements), m_Stride(0)
		{
			CalculateOffsetAndStride();
		}

		BufferLayout(const BufferLayout& other)
			: m_Elements(other.m_Elements), m_Stride(other.m_Stride)
		{
		}

	public:
		inline const Core::Containers::Array<BufferElement>& GetElements() const { return m_Elements; }
		inline const uint32 GetStride() const { return m_Stride; }

	private:
		void CalculateOffsetAndStride()
		{
			uint32 offset = 0;

			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += (uint32)element.Size;
				m_Stride += (uint32)element.Size;
			}
		}

	private:
		Core::Containers::Array<BufferElement> m_Elements;
		uint32 m_Stride;

	};

	class PAWN_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void Bind(BufferLayout& layout) = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data, SIZE_T size) = 0;
		virtual void* GetData() = 0;

	public:
		static VertexBuffer* Create(void* data, SIZE_T size, Usage usage);
		static VertexBuffer* CreateDirectX11VertexBuffer(void* data, SIZE_T size, Usage usage);
		static VertexBuffer* CreateDirectX12VertexBuffer(void* data, SIZE_T size, Usage usage);
		// ...
	};

	class PAWN_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data, uint32 count) = 0;
		virtual void* GetData() = 0;

		virtual uint32 GetCount() = 0;

	public:
		static IndexBuffer* Create(void* data, uint32 count, Usage usage);
		static IndexBuffer* CreateDirectX11IndexBuffer(void* data, uint32 count, Usage usage);

	};

	class PAWN_API Uniform
	{
	public:
		virtual ~Uniform() {};

		virtual void Bind(uint32 index, Shader::Type stage) = 0;
		virtual void Unbind(uint32 index, Shader::Type stage) = 0;

		virtual void SetData(void* data, SIZE_T size) = 0;
		virtual void* GetData() = 0;

	public:
		static Uniform* Create(SIZE_T size, Usage usage);
		static Uniform* CreateDirectX11Uniform(SIZE_T size, Usage usage);

	};

	/*
	API-specific conversion function
	*/
	//PAWN_API uint32 ConvertShaderTypeVulkan(ShaderType type);

#ifdef PLATFORM_WINDOWS
	extern PAWN_API uint32 ConvertShaderTypeDirectX11(ShaderType type);
	//extern PAWN_API uint32 ConvertShaderTypeDirectX12(ShaderType type);
#else
	extern PAWN_API uint32 ConvertShaderTypeDirectX11(ShaderType type) { PE_ASSERT(false, TEXT("DirectX11 conversion available only on Windows!")); return 0; }
	//extern PAWN_API uint32 ConvertShaderTypeDirectX12(ShaderType type) { PE_ASSERT(false, TEXT("DirectX12 conversion available only on Windows!")); return 0; }
#endif

}