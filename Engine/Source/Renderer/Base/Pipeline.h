#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>

#include "Renderer/Base/Buffer.h"
#include "Renderer/Base/Shader.h"

namespace Pawn::Render
{
	class Shader;

	enum class PipelineBindPoint : uint8
	{
		None = 0,
		Graphics, Compute,
		RayTracing, SubpassShading,
	};

	enum class PipelineStageFlags : uint32
	{
		None = 0,
		TopOfPipe								= BIT(0),
		BottomOfPipe							= BIT(1),
		DrawIndirect							= BIT(2),
		VertexInput								= BIT(3),
		VertexShader							= BIT(4),
		TesselationEvaluationShader				= BIT(5),
		TesselationControlShader				= BIT(6),
		GeometryShader							= BIT(7),
		FragmentShader							= BIT(8),
		EarlyFragmentTests						= BIT(9),
		LateFragmentTests						= BIT(10),
		ColorAttachmentOutput					= BIT(11),
		ComputeShader							= BIT(12),
		Transfer								= BIT(13),
		AllGraphics								= BIT(14),
		AllCommands								= BIT(15),
		RayTracing								= BIT(16),
		AccelerationStructure					= BIT(17),
		ConditionalRendering					= BIT(18),
	};

	enum class AccessFlags : uint32
	{
		None = 0,
		IndirectCommandRead						= BIT(0),
		IndexRead								= BIT(1),
		VertexAttributeRead						= BIT(2),
		UniformRead								= BIT(3),
		InputAttachmentRead						= BIT(4),
		ShaderRead								= BIT(5),
		ShaderWrite								= BIT(6),
		ColorAttachmentRead						= BIT(7),
		ColorAttachmentWrite					= BIT(8),
		DepthStencilRead						= BIT(9),
		DepthStencilWrite						= BIT(10),
		TransferRead							= BIT(11),
		TransferWrite							= BIT(12),
		HostRead								= BIT(13),
		HostWrite								= BIT(14),
		MemoryRead								= BIT(15),
		MemoryWrite								= BIT(16),
	};

	enum class PrimitiveTopology
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

	enum class RasterizerFill
	{
		Fill = 0,
		Wireframe = 1,
	};

	enum class RasterizerCull
	{
		None = 0,
		Back = 1,
		Front = 2,
	};

	enum class BlendMask
	{
		All = 0,
		Red = 1, Green = 2, Blue = 3, Alpha = 4,
	};

	enum class DepthComparison
	{
		None = 0,
		Less = 1, Greater = 2,
		LessEqual = 3, GreaterEqual = 4,
		NotEqual = 5, Equal = 6,
		Always = 7
	};

	enum class InputClassification
	{
		None = 0,
		PerVertex, PerInstance
	};

	class PAWN_API Pipeline
	{
	public:
		virtual ~Pipeline() {};

		/*virtual void SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate) = 0;

		virtual void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) = 0;

		virtual void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) = 0;

		virtual void SetBlendState(bool enableBlend, BlendMask mask) = 0;

		virtual void SetViewport(uint32 x, uint32 y) = 0;

		virtual void SetRasterizerState(RasterizerCull cull, RasterizerFill fill, 
			bool frontCounterClockwise, bool scissorEnabled, bool depthClipEnabled,
			bool slopeScaledDepthBias, int32 depthBias, float32 depthBiasClamp,
			bool multisampleEnabled, int32 sampleCount ) = 0;
		

		virtual void SetVertexShader(Pawn::Core::Memory::Reference<Shader> vertexShader) = 0;
		virtual void SetPixelShader(Pawn::Core::Memory::Reference<Shader> pixelShader) = 0;
		virtual void SetComputeShader(Pawn::Core::Memory::Reference<Shader> computeShader) = 0;
		virtual void SetGeometryShader(Pawn::Core::Memory::Reference<Shader> geometryShader) = 0;
		virtual void SetHullShader(Pawn::Core::Memory::Reference<Shader> hullShader) = 0;
		virtual void SetDomainShader(Pawn::Core::Memory::Reference<Shader> domainShader) = 0;
		*/

		virtual void BindInput() = 0;
		virtual void Bind() = 0;
		virtual void BindUniforms(Pawn::Core::Containers::Array<Uniform*>& uniforms, Shader::Type stage) = 0;

	public:
		static Pipeline* Create();

	private:
		static Pipeline* CreateDirectX11Pipeline();
		static Pipeline* CreateDirectX12Pipeline();
		static Pipeline* CreateVulkanPipeline();
		//...

	};

	inline constexpr PAWN_API PipelineStageFlags operator|(Pawn::Render::PipelineStageFlags a, Pawn::Render::PipelineStageFlags b)
	{
		return static_cast<PipelineStageFlags>((uint32)a | (uint32)b);
	}

	inline constexpr PAWN_API AccessFlags operator|(Pawn::Render::AccessFlags a, Pawn::Render::AccessFlags b)
	{
		return static_cast<AccessFlags>((uint32)a | (uint32)b);
	}

	inline constexpr PAWN_API PipelineStageFlags operator|=(Pawn::Render::PipelineStageFlags& a, Pawn::Render::PipelineStageFlags b)
	{
		a = a | b;
		return a;
	}

	inline constexpr PAWN_API AccessFlags operator|=(Pawn::Render::AccessFlags& a, Pawn::Render::AccessFlags b)
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

