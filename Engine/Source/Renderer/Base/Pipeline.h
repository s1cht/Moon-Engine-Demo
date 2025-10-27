#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Math/Math.hpp>

#include "Renderer/Managers/ShaderManager.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace ME::Render
{
	class Pipeline;
	class CommandBuffer;
	class RenderPass;

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

	struct VertexBufferElement
	{
		ShaderType Type;
		Core::String Name;
		SIZE_T Size;
		SIZE_T Offset;
		uint32 SemanticIndex;
		uint32 InputSlot;

		VertexBufferElement(ShaderType type, Core::String name, uint32 semanticIndex, uint32 inputSlot)
			: Type(type), Name(name), Size(SizeOfShaderType(type)), Offset(0), SemanticIndex(semanticIndex), InputSlot(inputSlot)
		{
		}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout()
			: m_InputClassification(InputClassification::None), m_Elements({}), m_Stride(0)
		{
		}

		VertexBufferLayout(InputClassification inputClassification, std::initializer_list<VertexBufferElement> elements)
			: m_InputClassification(inputClassification), m_Elements(elements), m_Stride(0)
		{
			CalculateOffsetAndStride();
		}

		VertexBufferLayout(const VertexBufferLayout& other)
			: m_Elements(other.m_Elements), m_Stride(other.m_Stride)
		{
		}

	public:
		inline const Core::Containers::Array<VertexBufferElement>& GetElements() const { return m_Elements; }
		inline const uint32 GetStride() const { return m_Stride; }
		inline const InputClassification GetInputClassification() const { return m_InputClassification; }

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
		Core::Containers::Array<VertexBufferElement> m_Elements;
		uint32 m_Stride;
		InputClassification m_InputClassification;

	};

	struct InputAssesemblySpecification
	{
		PrimitiveTopology Topology;
		bool PrimitiveRestart;
	};

	struct RasterizationSpecification
	{
		bool DiscardEnabled;
		bool FrontCounterClockwise;
		bool DepthBiasEnabled;
		bool DepthClampEnabled;
		float32 DepthBiasConstantFactor;
		float32 DepthBiasClamp;
		float32 DepthBiasSlopeFactor;
		float32 LineWidth;
		RasterizationCull Cull;
		RasterizationFill Fill;
	};

	struct MultisamplingSpecification
	{
		bool EnableSampleShading;
		bool AlphaToOne;
		bool AlphaToCoverage;
		float32 MinSampleShading;
		SampleCount Samples;
	};

	struct DepthStencilSpecification
	{
		bool DepthEnabled;
		bool StencilEnabled;
		float32 MinDepthBounds;
		float32 MaxDepthBounds;
		DepthComparison DepthFunction;
	};

	struct BlendAttachmentSpecification
	{
		bool EnableBlend;
		bool EnableAlphaMask;
		
	};

	struct ColorBlendingSpecification
	{
		ME::Core::Containers::Array<BlendAttachmentSpecification> Attachments;
		ME::Core::Math::Vector4D32 BlendConstants;
		LogicOperation LogicOperation;
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
			: m_Elements({})
		{
		}

		PushConstants(std::initializer_list<PushConstant> elements)
			: m_Elements(elements)
		{
			CalculateOffset();
		}

		PushConstants(const PushConstants& other)
			: m_Elements(other.m_Elements)
		{
		}

	public:
		inline const Core::Containers::Array<PushConstant>& GetElements() const { return m_Elements; }
		inline uint32 GetOffset(uint32 elementId) const { return m_Elements[elementId].Offset; }
		inline const InputClassification GetInputClassification() const { return m_InputClassification; }

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
		Core::Containers::Array<PushConstant> m_Elements;
		InputClassification m_InputClassification;

	};

	struct PipelineSpecification
	{
		PipelineType Type;
		ME::Core::Memory::Reference<RenderPass> RenderPass;
		uint32 Subpass;
		PushConstants Constants;

		// Compute pipeline
	    ME::Core::Memory::Reference<ME::Render::Shader> ComputeShader;
		// Graphics pipeline
		Render::Manager::ShaderManager::ShaderGroup Shaders;
		VertexBufferLayout BufferLayout;
		InputAssesemblySpecification InputAssembly;
		uint32 PatchControlPoints;
		RasterizationSpecification Rasterization;
		MultisamplingSpecification Multisampling;
		DepthStencilSpecification DepthStencil;
		ColorBlendingSpecification ColorBlending;
		ME::Core::Memory::Reference<Pipeline> BasePipeline;
	};

	struct ViewportSpecification
	{
		int32 X;
		int32 Y;
		uint32 Width;
		uint32 Height;
		int32 MinDepth;
		int32 MaxDepth;
	};

	class MEAPI Pipeline : public RenderObject
	{
	public:
		virtual void SetViewports(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Containers::Array<ME::Render::ViewportSpecification> specifications) = 0;
		virtual void SetScissors(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Containers::Array<ME::Core::Math::Rect2D> scissors) = 0;
		virtual void SetConstants(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ShaderStage shaderStage, void* constants,
			SIZE_T constantsSize) = 0;

		virtual void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual inline const PipelineSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Pipeline> Create(const PipelineSpecification& specification);

	private:
		static ME::Core::Memory::Reference<Pipeline> CreateVulkanPipeline(const PipelineSpecification& specification);

	};

	inline constexpr MEAPI PipelineStageFlags operator|(ME::Render::PipelineStageFlags a, ME::Render::PipelineStageFlags b)
	{
		return static_cast<PipelineStageFlags>((uint32)a | (uint32)b);
	}

	inline constexpr MEAPI AccessFlags operator|(ME::Render::AccessFlags a, ME::Render::AccessFlags b)
	{
		return static_cast<AccessFlags>((uint32)a | (uint32)b);
	}

	inline constexpr MEAPI PipelineStageFlags operator|=(ME::Render::PipelineStageFlags& a, ME::Render::PipelineStageFlags b)
	{
		a = a | b;
		return a;
	}

	inline constexpr MEAPI AccessFlags operator|=(ME::Render::AccessFlags& a, ME::Render::AccessFlags b)
	{
		a = a | b;
		return a;
	}

	inline constexpr PipelineStageFlags operator&(PipelineStageFlags a, PipelineStageFlags b)
	{
		return static_cast<PipelineStageFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}

	inline constexpr AccessFlags operator&(AccessFlags a, AccessFlags b)
	{
		return static_cast<AccessFlags>(static_cast<uint32>(a) & static_cast<uint32>(b));
	}

}

