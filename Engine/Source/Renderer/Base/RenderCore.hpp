#pragma once
#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String.hpp>
#include <Core/ClassInterface.hpp>

namespace ME::Render
{
#pragma region Enums
	enum class PipelineBindPoint : uint8
	{
		None = 0,
		Graphics, Compute,
		RayTracing, SubpassShading,
	};

	enum class PipelineStageFlags : uint32
	{
		None = 0,
		TopOfPipe = BIT(0),
		BottomOfPipe = BIT(1),
		DrawIndirect = BIT(2),
		VertexInput = BIT(3),
		VertexShader = BIT(4),
		TesselationEvaluationShader = BIT(5),
		TesselationControlShader = BIT(6),
		GeometryShader = BIT(7),
		FragmentShader = BIT(8),
		EarlyFragmentTests = BIT(9),
		LateFragmentTests = BIT(10),
		ColorAttachmentOutput = BIT(11),
		ComputeShader = BIT(12),
		Transfer = BIT(13),
		AllGraphics = BIT(14),
		AllCommands = BIT(15),
		RayTracing = BIT(16),
		AccelerationStructure = BIT(17),
		ConditionalRendering = BIT(18),
	};

	enum class AccessFlags : uint32
	{
		None = 0,
		IndirectCommandRead = BIT(0),
		IndexRead = BIT(1),
		VertexAttributeRead = BIT(2),
		UniformRead = BIT(3),
		InputAttachmentRead = BIT(4),
		ShaderRead = BIT(5),
		ShaderWrite = BIT(6),
		ColorAttachmentRead = BIT(7),
		ColorAttachmentWrite = BIT(8),
		DepthStencilRead = BIT(9),
		DepthStencilWrite = BIT(10),
		TransferRead = BIT(11),
		TransferWrite = BIT(12),
		HostRead = BIT(13),
		HostWrite = BIT(14),
		MemoryRead = BIT(15),
		MemoryWrite = BIT(16),
	};

	enum class PrimitiveTopology : uint8
	{
		None = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		PatchList
	};

	enum class RasterizationFill : uint8
	{
		Fill = 0,
		Wireframe,
		Point
	};

	enum class RasterizationCull : uint8
	{
		None = 0,
		Back,
		Front,
		FrontAndBack,
	};

	enum class SampleCount : uint8
	{
		None = 0,
		Count1, Count2, Count4, Count8,
		Count16, Count32, Count64
	};

	enum class BlendMask : uint8
	{
		All = 0,
		Red, Green, Blue, Alpha,
	};

	enum class DepthComparison : uint8
	{
		None = 0,
		Less, Greater,
		LessEqual, GreaterEqual,
		NotEqual, Equal,
		Always, Never
	};

	enum class StencilOperation : uint8
	{
		Keep,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClamp,
		Invert,
		IncrementWrap,
		DecrementWrap
	};

	enum class LogicOperation : uint8
	{
		None = 0,
		Clear, Copy, Set, CopyInverted,
		And, AndReverse, AndInverted, Nand,
		Xor, Or, Nor, OrReverse, OrInverted,
		Equivalent, Invert
	};

	enum class InputClassification : uint8
	{
		None = 0,
		PerVertex, PerInstance
	};

	enum class PipelineType : uint8
	{
		None = 0,
		Graphics, Compute
	};

	enum class StoreOperation : uint8
	{
		None = 0,
		DontCare,
		Store,
	};

	enum class LoadOperation : uint8
	{
		None = 0,
		DontCare,
		Load, Clear,
	};

	enum class MemoryType : uint8
	{
		VRAM, RAM
	};

	enum class Format : uint8
	{
		None = 0,
		// 8-bit formats
		R8_UINT, R8_SINT, R8_SRGB, R8_UNORM, R8_SNORM, R8_SSCALED, R8_USCALED,
		RG8_UINT, RG8_SINT, RG8_SRGB, RG8_UNORM, RG8_SNORM, RG8_SSCALED, RG8_USCALED,
		RGB8_UINT, RGB8_SINT, RGB8_SRGB, RGB8_UNORM, RGB8_SNORM, RGB8_SSCALED, RGB8_USCALED,
		BGR8_UINT, BGR8_SINT, BGR8_SRGB, BGR8_UNORM, BGR8_SNORM, BGR8_SSCALED, BGR8_USCALED,
		RGBA8_UINT, RGBA8_SINT, RGBA8_SRGB, RGBA8_UNORM, RGBA8_SNORM, RGBA8_SSCALED, RGBA8_USCALED,
		BGRA8_UINT, BGRA8_SINT, BGRA8_SRGB, BGRA8_UNORM, BGRA8_SNORM, BGRA8_SSCALED, BGRA8_USCALED,


		// 16-bit formats
		R16_UINT, R16_SINT, R16_UNORM, R16_SNORM, R16_SFLOAT, R16_SSCALED, R16_USCALED,
		RG16_UINT, RG16_SINT, RG16_UNORM, RG16_SNORM, RG16_SFLOAT, RG16_SSCALED, RG16_USCALED,
		RGB16_UINT, RGB16_SINT, RGB16_UNORM, RGB16_SNORM, RGB16_SFLOAT, RGB16_SSCALED, RGB16_USCALED,
		RGBA16_UINT, RGBA16_SINT, RGBA16_UNORM, RGBA16_SNORM, RGBA16_SFLOAT, RGBA16_SSCALED, RGBA16_USCALED,

		// 32-bit formats
		R32_UINT, R32_SINT, R32_SFLOAT,
		RG32_UINT, RG32_SINT, RG32_SFLOAT,
		RGB32_UINT, RGB32_SINT, RGB32_SFLOAT,
		RGBA32_UINT, RGBA32_SINT, RGBA32_SFLOAT,

		// 64-bit formats
		R64_UINT, R64_SINT, R64_SFLOAT,
		RG64_UINT, RG64_SINT, RG64_SFLOAT,
		RGB64_UINT, RGB64_SINT, RGB64_SFLOAT,
		RGBA64_UINT, RGBA64_SINT, RGBA64_SFLOAT,

		// Depth formats
		D16_UNORM,
		D16_UNORM_S8_UINT,
		D24_UNORM_S8_UINT,
		D32_SFLOAT,
		D32_SFLOAT_S8_UINT,

	};

	enum class ImageLayout : uint8
	{
		Undefined = 0,
		General,
		Present,
		ColorAttachment,
		DepthAttachment, StencilAttachment, DepthStencilAttachment,
		DepthReadOnly, StencilReadOnly, DepthStencilReadOnly,
		ShaderReadOnly,
		TransferSrc, TransferDst,
	};

	enum class ImageUsageFlags : uint16
	{
		None = 0,
		TransferSrc = BIT(0),
		TransferDst = BIT(1),
		Sampled = BIT(2),
		Storage = BIT(3),
		ColorAttachment = BIT(4),
		DepthStencilAttachment = BIT(5),
		TransientAttachment = BIT(6),
		InputAttachment = BIT(7),
		HostTransfer = BIT(8),
		VideoDecodeDst = BIT(9),
		VideoDecodeSrc = BIT(10),
		VideoDecodeDpb = BIT(11),
		VideoEncodeDst = BIT(12),
		VideoEncodeSrc = BIT(13),
		VideoEncodeDpb = BIT(14),
	};

	enum class SamplerFilter : uint8
	{
		None = 0,
		Nearest, Linear
	};

	enum class ResourceType : uint8
	{
		None = 0,
		Uniform, StorageBuffer,
		Texture1D, Texture2D, Texture3D,
		Sampler,
	};

	enum class ShaderType : uint8
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

	enum class ShaderStage : uint8
	{
		None = 0,
		Vertex = BIT(0),
		Hull = BIT(1),
		Domain = BIT(2),
		Geometry = BIT(3),
		Pixel = BIT(4),
		Compute = BIT(5),
		Task = BIT(6),
		Mesh = BIT(7),
	};
#pragma endregion
#pragma region Definitions
	// RenderAPI
	struct BarrierInfo;
	// Pipeline
	struct InputAssemblySpecification;
	struct RasterizationSpecification;
	struct MultisamplingSpecification;
	struct StencilFaceSpecification;
	struct DepthStencilSpecification;
	struct BlendAttachmentSpecification;
	struct ColorBlendingSpecification;
	struct PipelineSpecification;
	struct ViewportSpecification;
	// RenderPass
	struct AttachmentSpecification;
	struct SubpassSpecification;
	struct SubpassDependency;
	struct RenderPassSpecification;
	struct DepthStencilClearValue;
	struct ClearValue;
	struct RenderPassBeginInfo;
	// Framebuffer
	struct FramebufferSpecification;
	// Buffer
	struct BufferSpecification;
	struct VertexBufferSpecification;
	struct IndexBufferSpecification;
	struct UniformSpecification;
	struct StorageBufferSpecification;
	struct IndirectBufferSpecification;
	// Texture
	struct TextureSpecification;
	struct Texture1DSpecification;
	struct Texture2DSpecification;
	struct Texture3DSpecification;
	// Shader
	struct CompiledShader;
	struct ShaderSpecification;


	// Classes
	class RenderAPI;
	class ResourceHandler;
	class SwapChain;
	class Pipeline;
	class RenderPass;
	class Shader;

	class Framebuffer;
	class RFramebuffer;

	class CommandBuffer;

	class Buffer;
	class Uniform;
	class StorageBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class IndirectBuffer;

	class RUniform;
	class RStorageBuffer;
	class RIndirectBuffer;

	class Texture;
	class Texture1D;
	class Texture2D;
	class Texture3D;

	class RTexture1D;
	class RTexture2D;
	class RTexture3D;

	class VertexBufferLayout;
	class PushConstants;

	class ResourceLayout;
	class ResourceLayoutPack;
#pragma endregion
#pragma region Functions
	extern MEAPI SIZE_T SizeOfShaderType(ME::Render::ShaderType type);
#pragma endregion

	struct DrawIndirectIndexedData
	{
		uint32 indexCount;
		uint32 instanceCount;
		uint32 firstIndex;
		uint32 vertexOffset;
		uint32 firstInstance;
	};

	class MEAPI RenderObject : public Core::ClassInterface<RenderObject>
	{
	public:
		virtual ~RenderObject() = default;
		virtual void Shutdown() = 0;
	};

	class MEAPI RenderBindable : public RenderObject
	{
	public:
		virtual void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer) = 0;
	};

	class MEAPI RenderMemoryObject : public RenderObject
	{
	public:
		virtual void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, uint32 offset) = 0;
		virtual void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Pipeline> pipeline) = 0;

		virtual void Write() = 0;

		virtual void Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src, BarrierInfo dst) = 0;
	};

	struct ResourceBinding
	{
		bool Combined = false;
		ME::Render::ResourceType Type;
		ME::Render::ShaderStage Stage;

		ResourceBinding() = default;
		ResourceBinding(ME::Render::ResourceType type, ME::Render::ShaderStage stage) : Type(type), Stage(stage) {}
		ResourceBinding(bool combined, ME::Render::ResourceType type, ME::Render::ShaderStage stage) : Combined(combined), Type(type), Stage(stage) {}

		inline bool operator==(const ResourceBinding& binding) const
		{
			return this->Stage == binding.Stage && this->Type == binding.Type && this->Combined == binding.Combined;
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

	struct VertexBufferElement
	{
		ShaderType Type;
		ME::Core::String Name;
		SIZE_T Size;
		SIZE_T Offset;
		uint32 SemanticIndex;
		uint32 InputSlot;

		VertexBufferElement(ShaderType type, ME::Core::String name, uint32 semanticIndex, uint32 inputSlot)
			: Type(type), Name(std::move(name)), Size(SizeOfShaderType(type)), Offset(0), SemanticIndex(semanticIndex), InputSlot(inputSlot) {}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout()
			: m_Elements({}), m_Stride(0), m_InputClassification(InputClassification::None) {}

		VertexBufferLayout(InputClassification inputClassification, std::initializer_list<VertexBufferElement> elements)
			: m_Elements(elements), m_Stride(0), m_InputClassification(inputClassification)
		{
			CalculateOffsetAndStride();
		}

		VertexBufferLayout(const VertexBufferLayout& other) = default;

	public:
		inline const Core::Array<VertexBufferElement>& GetElements() const { return m_Elements; }
		inline uint32 GetStride() const { return m_Stride; }
		inline InputClassification GetInputClassification() const { return m_InputClassification; }

	private:
		void CalculateOffsetAndStride()
		{
			uint32 offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += static_cast<uint32>(element.Size);
				m_Stride += static_cast<uint32>(element.Size);
			}
		}

	private:
		ME::Core::Array<VertexBufferElement> m_Elements;
		uint32 m_Stride;
		InputClassification m_InputClassification;
	};

	struct PushConstant
	{
		ME::Render::ShaderStage Stage;
		uint32 Offset = 0;
		uint32 Size = 0;
	};

	class PushConstants
	{
	public:
		PushConstants()
			: m_Elements({}) {}

		PushConstants(std::initializer_list<PushConstant> elements)
			: m_Elements(elements)
		{
			CalculateOffset();
		}

		PushConstants(const PushConstants& other) = default;

	public:
		inline const Core::Array<PushConstant>& GetElements() const { return m_Elements; }
		inline uint32 GetOffset(uint32 elementId) const { return m_Elements[elementId].Offset; }

	private:
		void CalculateOffset()
		{
			uint32 offset = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
			}
		}

	private:
		ME::Core::Array<PushConstant> m_Elements;
	};

	// Enum operators

	// ShaderStage flags
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

	// PipelineStage flags
	inline constexpr MEAPI PipelineStageFlags operator|(ME::Render::PipelineStageFlags a, ME::Render::PipelineStageFlags b)
	{
		return static_cast<PipelineStageFlags>(static_cast<uint32>(a) | static_cast<uint32>(b));
	}
	inline constexpr MEAPI PipelineStageFlags operator|=(ME::Render::PipelineStageFlags& a, ME::Render::PipelineStageFlags b)
	{
		a = a | b;
		return a;
	}
	inline constexpr PipelineStageFlags operator&(PipelineStageFlags a, PipelineStageFlags b)
	{
		return static_cast<PipelineStageFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}

	// Access flags
	inline constexpr MEAPI AccessFlags operator|(ME::Render::AccessFlags a, ME::Render::AccessFlags b)
	{
		return static_cast<AccessFlags>(static_cast<uint32>(a) | static_cast<uint32>(b));
	}
	inline constexpr MEAPI AccessFlags operator|=(ME::Render::AccessFlags& a, ME::Render::AccessFlags b)
	{
		a = a | b;
		return a;
	}
	inline constexpr AccessFlags operator&(AccessFlags a, AccessFlags b)
	{
		return static_cast<AccessFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}
}