#include "Renderer.hpp"

#include "RenderCommand.h"
#include "Assets/Mesh.hpp"
#include "Core/Containers/Tables/Map.hpp"

constexpr char8 ME_RENDER_PRIMARY_PIPELINE_NAME[] = TEXT("Primary");

namespace ME::Render
{
    Renderer::Renderer()
    :	m_DepthBuffer(nullptr),
		m_PrimaryRenderPass(nullptr),
		m_PrimaryPipeline(nullptr),
		m_MeshTransforms(nullptr),
		m_QueuedMeshes(ME::Core::Containers::UnorderedMap<uint64, ME::Core::Containers::Array<ME::Core::Math::Matrix4x4>>())
    {
    }

    Renderer::~Renderer()
    {
    }

    bool Renderer::InitImpl()
	{
		if (!RenderCommand::Init())
		    return false;

		m_VertexInputFormat = VertexBufferLayout(InputClassification::PerVertex, 
{
			{ ShaderType::Float3, "POSITION", 0, 0 },
			{ ShaderType::Float2, "TCOORD", 1, 0 },
			{ ShaderType::Float3, "NORMALPOS", 2, 0 }
		});
#pragma region Render pass
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Texture2D>> swapChainTextures = RenderCommand::Get()->GetSwapChain()->GetImages();
		ME::Core::Containers::Array<AttachmentSpecification> attachments;

		auto imageSpec = swapChainTextures[0]->GetSpecification();
		AttachmentSpecification attachmentSpecs = {};
		attachmentSpecs.IsStencil = false;
		attachmentSpecs.IsDepth = false;
		attachmentSpecs.AttachmentFormat = imageSpec.Format;
		attachmentSpecs.LoadOp = LoadOperation::Clear;
		attachmentSpecs.StoreOp = StoreOperation::Store;
		attachmentSpecs.InitialLayout = ImageLayout::ColorAttachment;
		attachmentSpecs.FinalLayout = ImageLayout::Present;
		attachmentSpecs.SampleCount = SampleCount::Count1;

		AttachmentSpecification depthSpecs = {};
		depthSpecs.IsStencil = true;
		depthSpecs.IsDepth = true;
		depthSpecs.AttachmentFormat = m_DepthBuffer->GetSpecification().Format;
		depthSpecs.LoadOp = LoadOperation::Clear;
		depthSpecs.StoreOp = StoreOperation::DontCare;
		depthSpecs.InitialLayout = m_DepthBuffer->GetSpecification().Layout;
		depthSpecs.FinalLayout = ImageLayout::DepthStencilAttachment;
		depthSpecs.SampleCount = m_DepthBuffer->GetSpecification().SampleCount;

		attachments.EmplaceBack(attachmentSpecs);
		attachments.EmplaceBack(depthSpecs);

		SubpassSpecification subpass = {};
		subpass.ColorAttachmentRefs = { 0 };
		subpass.DepthStencilAttachmentRef = { 1 };
		subpass.PipelineBindPoint = PipelineBindPoint::Graphics;

		SubpassDependency subpassDependency = {};
		subpassDependency.SubpassSrc = ~0u;
		subpassDependency.AccessFlagsDst = AccessFlags::DepthStencilWrite | AccessFlags::ColorAttachmentWrite;
		subpassDependency.PipelineStageFlagsSrc = PipelineStageFlags::ColorAttachmentOutput | PipelineStageFlags::EarlyFragmentTests;
		subpassDependency.PipelineStageFlagsDst = PipelineStageFlags::ColorAttachmentOutput | PipelineStageFlags::EarlyFragmentTests;

		RenderPassSpecification mainRPSpecs = {};
		mainRPSpecs.AttachmentSpecs = attachments;
		mainRPSpecs.SubpassSpecs = { subpass };
		mainRPSpecs.SubpassDependencies = { subpassDependency };
		mainRPSpecs.DebugName = "Main render pass";

		m_PrimaryRenderPass = RenderPass::Create(mainRPSpecs);
#pragma endregion
	    RenderCommand::CreateFramebuffers(m_PrimaryRenderPass);
#pragma region Primary graphics pipeline
		auto primaryShaderGroup = Manager::ShaderManager::GetShaderGroup(ME_RENDER_PRIMARY_PIPELINE_NAME);

		InputAssesemblySpecification iaSpec = {};
		iaSpec.Topology = PrimitiveTopology::TriangleList;
		iaSpec.PrimitiveRestart = false;

		RasterizationSpecification rasterSpec = {};
		rasterSpec.DiscardEnabled = false;
		rasterSpec.FrontCounterClockwise = true;
		rasterSpec.DepthBiasEnabled = false;
		rasterSpec.DepthClampEnabled = false;
		rasterSpec.DepthBiasConstantFactor = 1.0f;
		rasterSpec.DepthBiasClamp = 1.f;
		rasterSpec.DepthBiasSlopeFactor = 1.f;
		rasterSpec.LineWidth = 1.0f;
		rasterSpec.Cull = RasterizationCull::Back;
		rasterSpec.Fill = RasterizationFill::Fill;

		MultisamplingSpecification multisamplingSpec = {};
		multisamplingSpec.Samples = SampleCount::Count1;
		multisamplingSpec.EnableSampleShading = false;
		multisamplingSpec.AlphaToOne = false;
		multisamplingSpec.MinSampleShading = 1;
		multisamplingSpec.AlphaToCoverage = false;

		DepthStencilSpecification dsSpec = {};
		dsSpec.DepthEnabled = true;
		dsSpec.StencilEnabled = false;
		dsSpec.MaxDepthBounds = 1.0f;
		dsSpec.MinDepthBounds = 0.f;
		dsSpec.DepthFunction = DepthComparison::LessEqual;

		ColorBlendingSpecification cbSpec = {};
		cbSpec.Attachments = { { false, false  } };
		cbSpec.BlendConstants = { 0.f, 0.f, 0.f, 0.f };
		cbSpec.LogicOperation = LogicOperation::None;

		PipelineSpecification mainGraphicsSpecs = {};
		mainGraphicsSpecs.Type = PipelineType::Graphics;
		mainGraphicsSpecs.RenderPass = m_PrimaryRenderPass;
		mainGraphicsSpecs.Subpass = 0;
		mainGraphicsSpecs.Shaders = primaryShaderGroup;
		mainGraphicsSpecs.BufferLayout = m_VertexInputFormat;
		mainGraphicsSpecs.InputAssembly = iaSpec;
		mainGraphicsSpecs.Rasterization = rasterSpec;
		mainGraphicsSpecs.Multisampling = multisamplingSpec;
		mainGraphicsSpecs.DepthStencil = dsSpec;
		mainGraphicsSpecs.ColorBlending = cbSpec;
		mainGraphicsSpecs.BasePipeline = nullptr;
		mainGraphicsSpecs.Constants = {
		    PushConstant(ShaderStage::Task, 0, sizeof(uint32))
		};

		m_PrimaryPipeline = Pipeline::Create(mainGraphicsSpecs);
#pragma endregion
#pragma region Buffers
		Texture2DSpecification depthBufferSpecification = {};
		depthBufferSpecification.Data = nullptr;
		depthBufferSpecification.Resolution = RenderCommand::Get()->GetSwapChain()->GetExtent();
		depthBufferSpecification.IsDepth = true;
		depthBufferSpecification.IsStencil = true;
		depthBufferSpecification.CubeMapCount = 0;
		depthBufferSpecification.DataSize = 0;
		depthBufferSpecification.DebugName = "Depth buffer";
		depthBufferSpecification.Format = Format::D24_UNORM_S8_UINT;
		depthBufferSpecification.Layout = ImageLayout::DepthStencilAttachment;
		depthBufferSpecification.SampleCount = SampleCount::Count1;
		depthBufferSpecification.MipLevels = 1;
		depthBufferSpecification.EnableAnisotropy = false;
		depthBufferSpecification.MaxAnisotropy = 1;
		depthBufferSpecification.MinFilter = SamplerFilter::None;
		depthBufferSpecification.MagFilter = SamplerFilter::None;
		depthBufferSpecification.Usage = ImageUsageFlags::DepthStencilAttachment;

		m_DepthBuffer = Texture2D::Create(depthBufferSpecification);


#pragma endregion
		return true;
	}
	void Renderer::ShutdownImpl()
	{
		RenderCommand::Shutdown();
	}

    void Renderer::BeginSceneImpl(const ME::ECS::Scene& scene)
    {
    }

	void Renderer::EndSceneImpl()
	{
            
	}

    void Renderer::RenderMeshImpl(const ME::Core::Memory::Reference<ME::Assets::Mesh>& mesh,
        const ME::Core::Math::Matrix4x4& transform)
    {
		if (m_QueuedMeshes.Contains(mesh->GetMeshID()))
			m_QueuedMeshes[mesh->GetMeshID()].EmplaceBack(transform);

		m_QueuedMeshes.Insert(mesh->GetMeshID(), ME::Core::Containers::Array({ transform }));
    }

    void Renderer::SetRenderAPI(RenderAPI::API api)
	{
		RenderAPI::SetRendererAPI(api);
	}
	inline RenderAPI::API Renderer::GetRenderAPI()
	{
		return RenderAPI::GetRendererAPI();
	}

    void Renderer::ProcessQueuedMeshes()
    {
		ME::Core::Memory::Reference<CommandBuffer> currentCommandBuffer = RenderCommand::GetAvailableCommandBuffer();

	    for (auto& meshData : m_QueuedMeshes)
		{
			m_MeshTransforms->SetData(currentCommandBuffer, meshData.Value2.Data(), meshData.Value2.Size() * sizeof(ME::Core::Math::Matrix4x4));
			RenderCommand::WriteResource(m_MeshTransforms);
			RenderCommand::DispatchMesh(currentCommandBuffer, 128, 0, 0);
		}
    }
}
