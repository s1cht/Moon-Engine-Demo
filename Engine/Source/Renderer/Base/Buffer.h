#pragma once
#include <Core.h>
#include <Core/Containers/String.h>
#include <Core/Misc/Assertion.h>

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

	struct BufferElement
	{
		ShaderType Type;
		AnsiString Name;
		SIZE_T Size;
		SIZE_T Offset;
		uint32 SemanticIndex;
		uint32 InputSlot;

		BufferElement(ShaderType type, AnsiString name, uint32 semanticIndex, uint32 inputSlot)
			: Type(type), Name(name), Size(SizeOfShaderType(type)), Offset(0), SemanticIndex(semanticIndex), InputSlot(inputSlot)
		{
		}
	};

	enum class Usage
	{
		Default = 0,
		Immutable,
		Dynamic,
		Staging
	};

	class PAWN_API BufferLayout
	{
	public:
		BufferLayout(std::initializer_list<BufferElement>& elements)
			: m_Elements(elements), m_Stride(0)
		{
			CalculateOffsetAndStride();
		}

	public:
		inline const Array<BufferElement, 10>& GetElements() const { return m_Elements; }
		inline const uint32 GetStride() const { return m_Stride; }

	private:
		void CalculateOffsetAndStride()
		{
			uint32 offset = 0;

			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		Array<BufferElement, 10> m_Elements;
		uint32 m_Stride;

	};

	class PAWN_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data) = 0;
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

		virtual void SetData(void* data) = 0;
		virtual void* GetData() = 0;

		virtual uint32 GetCount() = 0;

	public:
		static IndexBuffer* Create(void* data, uint32 count, Usage usage);
		static IndexBuffer* CreateDirectX11IndexBuffer(void* data, uint32 count, Usage usage);

	};

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {};

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetData(void* data) = 0;
		virtual void* GetData() = 0;

	public:
		static UniformBuffer* Create(void* data, SIZE_T size, Usage usage);
		static UniformBuffer* CreateDirectX11UniformBuffer(void* data, SIZE_T size, Usage usage);

	};

	SIZE_T SizeOfShaderType(ShaderType type);
	uint32 GetTypeAPISpecificShaderType(ShaderType type);

	/*
	API-specific conversion function
	*/
	static uint32 ConvertShaderTypeVulkan(ShaderType type) { PE_ASSERT(false, TEXT("Vulkan is unsupported!")); return 0; }

#ifdef PLATFORM_WINDOWS
	static uint32 ConvertShaderTypeDirectX11(ShaderType type);
	//static uint32 ConvertShaderTypeDirectX12(ShaderType type);
#else
	static uint32 ConvertShaderTypeDirectX11(ShaderType type) { PE_ASSERT(false, TEXT("DirectX11 conversion available only on Windows!")); return 0; }
	static uint32 ConvertShaderTypeDirectX12(ShaderType type) { PE_ASSERT(false, TEXT("DirectX12 conversion available only on Windows!")); return 0; }
#endif

}