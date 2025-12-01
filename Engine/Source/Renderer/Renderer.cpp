#include "Renderer.hpp"
#include <Core/Containers/Map.hpp>
#include <Core/Containers/UnorderedMap.hpp>

#include "EngineDefines.hpp"
#include "RenderCommand.hpp"
#include "Application/Application.hpp"
#include "Renderer/Base/RenderPass.hpp"
#include "Managers/MeshManager.hpp"

constexpr char8 FRUSTUM_CULLING_SHADER[] = TEXT("Frustum_Culling");
constexpr char8 MERGE_SHADERS[] = TEXT("MergeStageSH");

namespace ME::Render
{
    Renderer::Renderer()
    :	m_GeometryPipeline(nullptr),
		m_GPass(nullptr),
		m_MeshTransforms(nullptr),
		m_QueuedMeshes(ME::Core::UnorderedMap<uint64, MeshInfos>())
    {
    }

    Renderer::~Renderer()
    {
    }

	bool Renderer::InitImpl()
	{
		Render::ResourceBinding commonBinding = {};
		commonBinding.Stage = Render::ShaderStage::Vertex |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Task |
			Render::ShaderStage::Compute |
			Render::ShaderStage::Domain |
			Render::ShaderStage::Hull;
		commonBinding.Type = Render::ResourceType::StorageBuffer;

		// Set 0
		Render::ResourceLayout vertexSet;
		// Vertices		| Binding 0
		vertexSet.EmplaceBack(commonBinding);
		// Indices		| Binding 1
		vertexSet.EmplaceBack(commonBinding);
		// Meshlets		| Binding 2
		vertexSet.EmplaceBack(commonBinding);
		// MeshBoxes	| Binding 3
		vertexSet.EmplaceBack(commonBinding);
		// DrawMeshData	| Binding 4
		vertexSet.EmplaceBack(commonBinding);

		// Set 1
		Render::ResourceLayout cameraDataSet;
		// Frustum Data	| Binding 0
		cameraDataSet.EmplaceBack(Render::ResourceType::Uniform, Render::ShaderStage::Vertex | Render::ShaderStage::Task | Render::ShaderStage::Compute);
		// Camera Data	| Binding 1
		cameraDataSet.EmplaceBack(Render::ResourceType::Uniform, Render::ShaderStage::Vertex | Render::ShaderStage::Task | Render::ShaderStage::Mesh | Render::ShaderStage::Compute);

		// Set 2
		Render::ResourceLayout meshDataSet;
		// Transforms	| Binding 0
		meshDataSet.EmplaceBack(Render::ResourceType::StorageBuffer, Render::ShaderStage::Vertex | Render::ShaderStage::Task | Render::ShaderStage::Mesh | Render::ShaderStage::Compute);
		// Mesh infos	| Binding 1
		meshDataSet.EmplaceBack(Render::ResourceType::StorageBuffer, Render::ShaderStage::Pixel | Render::ShaderStage::Vertex | Render::ShaderStage::Compute);
		// Mesh ids		| Binding 2
		meshDataSet.EmplaceBack(Render::ResourceType::StorageBuffer, Render::ShaderStage::Vertex | Render::ShaderStage::Pixel | Render::ShaderStage::Task | Render::ShaderStage::Mesh);

		// Set 3
		Render::ResourceLayout frustumCullingSetLayout;
		// Input | Binding 0
		frustumCullingSetLayout.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);
		// OutputArray | Binding 1
		frustumCullingSetLayout.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);
		// OutputCount | Binding 2
		frustumCullingSetLayout.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);

		// Set 4
		Render::ResourceLayout gBufferSet;
		// BaseColor wt combined sampler | Binding 0
		gBufferSet.EmplaceBack(
			true,
			Render::ResourceType::Texture2D,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);
		gBufferSet.EmplaceBack(
			Render::ResourceType::Sampler,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);
        // Normal wt combined sampler | Binding 1
		gBufferSet.EmplaceBack(
			true,
			Render::ResourceType::Texture2D,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);
		gBufferSet.EmplaceBack(
			Render::ResourceType::Sampler,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);
        // Depth wt combined sampler | Binding 2
		gBufferSet.EmplaceBack(
			true,
			Render::ResourceType::Texture2D,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);
		gBufferSet.EmplaceBack(
			Render::ResourceType::Sampler,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);

		// Set 0 Merge
		Render::ResourceLayout mergeImguiSet;
		// BaseColor | Binding 0
		mergeImguiSet.EmplaceBack(
			true,
			Render::ResourceType::Texture2D,
			Render::ShaderStage::Pixel | 
			Render::ShaderStage::Compute
		);
		mergeImguiSet.EmplaceBack(
			Render::ResourceType::Sampler,
			Render::ShaderStage::Pixel |
			Render::ShaderStage::Compute
		);

        Render::ResourceLayout emptySet = {};

		Render::ResourceLayoutPack primaryPipelineSets = {};
		primaryPipelineSets.EmplaceBack(vertexSet);
		primaryPipelineSets.EmplaceBack(cameraDataSet);
		primaryPipelineSets.EmplaceBack(meshDataSet);
		primaryPipelineSets.EmplaceBack(frustumCullingSetLayout);
		primaryPipelineSets.EmplaceBack(gBufferSet);

		Render::ResourceLayoutPack mergePipelineSets = {};
		mergePipelineSets.EmplaceBack(mergeImguiSet);
		mergePipelineSets.EmplaceBack(emptySet);
		mergePipelineSets.EmplaceBack(emptySet);
		mergePipelineSets.EmplaceBack(emptySet);
		mergePipelineSets.EmplaceBack(gBufferSet);

		ME::Render::Manager::ShaderGroupSpecification primaryShaderGroupSpecs = {};
		primaryShaderGroupSpecs.Type = Render::Manager::ShaderGroupType::Vertex;
		primaryShaderGroupSpecs.Pixel = TEXT("Deffered/GPassPixel.hlsl");
		primaryShaderGroupSpecs.Vertex.Vertex = TEXT("Deffered/GPassVert.hlsl");
		primaryShaderGroupSpecs.Layout = primaryPipelineSets;
		primaryShaderGroupSpecs.ShaderGroupName = ME_RENDER_PRIMARY_PIPELINE_NAME;
		primaryShaderGroupSpecs.Defines = {};

		ME::Render::Manager::ShaderGroupSpecification mergeShaderGroupSpecs = {};
		mergeShaderGroupSpecs.Type = Render::Manager::ShaderGroupType::Vertex;
		mergeShaderGroupSpecs.Pixel = TEXT("Deffered/MergeShader.hlsl");
		mergeShaderGroupSpecs.Vertex.Vertex = TEXT("Deffered/MergeShader.hlsl");
		mergeShaderGroupSpecs.Layout = mergePipelineSets;
		mergeShaderGroupSpecs.ShaderGroupName = ME_RENDER_MERGE_PIPELINE_NAME;
		mergeShaderGroupSpecs.Defines = {};

		ME::Render::Manager::ShaderManager::Get().LoadShaders(primaryShaderGroupSpecs);
		ME::Render::Manager::ShaderManager::Get().LoadShaders(mergeShaderGroupSpecs);
		Manager::ShaderManager::ShaderGroup primaryShaderGroup =
			Manager::ShaderManager::GetShaderGroup(ME_RENDER_PRIMARY_PIPELINE_NAME);
		Manager::ShaderManager::ShaderGroup mergeShaderGroup =
			Manager::ShaderManager::GetShaderGroup(ME_RENDER_MERGE_PIPELINE_NAME);
		Manager::ShaderManager::ComputeShaderGroup frustumCullingShaderGroup;

		ME::Render::Manager::ShaderGroupSpecification frustumCullShader = {};
		frustumCullShader.Type = Manager::ShaderGroupType::Compute;
		frustumCullShader.Layout = primaryShaderGroup.ResourceLayout;
		frustumCullShader.Compute = TEXT("Deffered/FrustumCulling.hlsl");
		frustumCullShader.ShaderGroupName = FRUSTUM_CULLING_SHADER;
		frustumCullShader.Defines = {};

		ME::Render::Manager::ShaderManager::Get().LoadShaders(frustumCullShader);
		frustumCullingShaderGroup = Manager::ShaderManager::GetComputeShaderGroup(FRUSTUM_CULLING_SHADER);

#pragma region Buffers
		int32 gbufferSet = RenderCommand::GetResourceHandler()->CreateResourceSet(primaryShaderGroup.ResourceLayout[GBUFFER_SET]);

		Texture2DSpecification gBaseColorSpecification = {};
		gBaseColorSpecification.SetIndex = gbufferSet;
		gBaseColorSpecification.Set = GBUFFER_SET;
		gBaseColorSpecification.Binding = 0;
		gBaseColorSpecification.Data = nullptr;
		gBaseColorSpecification.Resolution = RenderCommand::Get()->GetSwapChain()->GetExtent();
		gBaseColorSpecification.IsDepth = false;
		gBaseColorSpecification.IsStencil = false;
		gBaseColorSpecification.CubeMapCount = 0;
		gBaseColorSpecification.DataSize = 0;
		gBaseColorSpecification.DebugName = "G-Buffer: base color";
		gBaseColorSpecification.Format = Format::RGBA8_UNORM;
		gBaseColorSpecification.Layout = ImageLayout::General;
		gBaseColorSpecification.SampleCount = SampleCount::Count1;
		gBaseColorSpecification.MipLevels = 1;
		gBaseColorSpecification.EnableAnisotropy = false;
		gBaseColorSpecification.MaxAnisotropy = 1;
		gBaseColorSpecification.MinFilter = SamplerFilter::Linear;
		gBaseColorSpecification.MagFilter = SamplerFilter::Linear;
		gBaseColorSpecification.Usage = ImageUsageFlags::ColorAttachment | ImageUsageFlags::Storage | ImageUsageFlags::Sampled;
		m_gBaseColor = RTexture2D::Create(gBaseColorSpecification);

		Texture2DSpecification gNormalSpecification = {};
		gNormalSpecification.SetIndex = gbufferSet;
		gNormalSpecification.Set = GBUFFER_SET;
		gNormalSpecification.Binding = 1;
		gNormalSpecification.Data = nullptr;
		gNormalSpecification.Resolution = RenderCommand::Get()->GetSwapChain()->GetExtent();
		gNormalSpecification.IsDepth = false;
		gNormalSpecification.IsStencil = false;
		gNormalSpecification.CubeMapCount = 0;
		gNormalSpecification.DataSize = 0;
		gNormalSpecification.DebugName = "G-Buffer: normal";
		gNormalSpecification.Format = Format::RGBA16_SFLOAT;
		gNormalSpecification.Layout = ImageLayout::General;
		gNormalSpecification.SampleCount = SampleCount::Count1;
		gNormalSpecification.MipLevels = 1;
		gNormalSpecification.EnableAnisotropy = false;
		gNormalSpecification.MaxAnisotropy = 1;
		gNormalSpecification.MinFilter = SamplerFilter::Linear;
		gNormalSpecification.MagFilter = SamplerFilter::Linear;
		gNormalSpecification.Usage = ImageUsageFlags::ColorAttachment | ImageUsageFlags::Storage | ImageUsageFlags::Sampled;
		m_gNormal = RTexture2D::Create(gNormalSpecification);

		Texture2DSpecification gDepthBufferSpecification = {};
		gDepthBufferSpecification.SetIndex = gbufferSet;
		gDepthBufferSpecification.Set = GBUFFER_SET;
		gDepthBufferSpecification.Binding = 2;
		gDepthBufferSpecification.Data = nullptr;
		gDepthBufferSpecification.DataSize = 0;
		gDepthBufferSpecification.Resolution = RenderCommand::Get()->GetSwapChain()->GetExtent();
		gDepthBufferSpecification.IsDepth = true;
		gDepthBufferSpecification.IsStencil = false;
		gDepthBufferSpecification.Format = Format::D24_UNORM_S8_UINT;
		gDepthBufferSpecification.Layout = ImageLayout::DepthStencilAttachment;
		gDepthBufferSpecification.CubeMapCount = 0;
		gDepthBufferSpecification.MipLevels = 1;
		gDepthBufferSpecification.EnableAnisotropy = false;
		gDepthBufferSpecification.MaxAnisotropy = 1;
		gDepthBufferSpecification.MinFilter = SamplerFilter::Linear;
		gDepthBufferSpecification.MagFilter = SamplerFilter::Linear;
		gDepthBufferSpecification.SampleCount = SampleCount::Count1;
		gDepthBufferSpecification.Usage = ImageUsageFlags::DepthStencilAttachment | ImageUsageFlags::Sampled | ImageUsageFlags::TransferDst;
		gDepthBufferSpecification.DebugName = "G-Buffer: depth";
		m_gDepth = RTexture2D::Create(gDepthBufferSpecification);

		int32 cameraSetId = RenderCommand::GetResourceHandler()->CreateResourceSet(primaryShaderGroup.ResourceLayout[CAMERA_SET]);

		// Set 1 Binding 0 - Camera Frustum Buffer
		UniformSpecification cameraFrustumBufferSpec = {};
		cameraFrustumBufferSpec.Set = 1;
		cameraFrustumBufferSpec.Binding = 0;
		cameraFrustumBufferSpec.Size = sizeof(ME::Core::Math::Frustum);
		cameraFrustumBufferSpec.DebugName = "Camera frustum Buffer";
		cameraFrustumBufferSpec.ResourceBinding = primaryShaderGroup.ResourceLayout[CAMERA_SET][0];
		cameraFrustumBufferSpec.MemoryType = MemoryType::RAM;
		cameraFrustumBufferSpec.SetIndex = cameraSetId;
		m_CameraFrustumBuffer = RUniform::Create(cameraFrustumBufferSpec);

		// Set 1 Binding 1 - Camera Buffer
		UniformSpecification cameraBufferSpec = {};
		cameraBufferSpec.Set = 1;
		cameraBufferSpec.Binding = 1;
		cameraBufferSpec.Size = sizeof(CameraData);
		cameraBufferSpec.DebugName = "Camera Buffer";
		cameraBufferSpec.ResourceBinding = primaryShaderGroup.ResourceLayout[CAMERA_SET][1];
		cameraBufferSpec.MemoryType = MemoryType::RAM;
		cameraBufferSpec.SetIndex = cameraSetId;
		m_CameraBuffer = RUniform::Create(cameraBufferSpec);

		uint32 meshInfoSet = RenderCommand::GetResourceHandler()->CreateResourceSet(primaryShaderGroup.ResourceLayout[MESH_TRANSFORM_SET]);

		// Set 2 Binding 0 - Mesh Transforms
		StorageBufferSpecification meshTransformBufferSpec = {};
		meshTransformBufferSpec.Set = 2;
		meshTransformBufferSpec.Binding = 0;
		meshTransformBufferSpec.Size = sizeof(ME::Core::Math::Matrix4x4) * ME_MESH_MNG_MAX_MESH_COUNT; // Max 100k instances per draw call
		meshTransformBufferSpec.MemoryType = MemoryType::RAM;
		meshTransformBufferSpec.DebugName = "Mesh Transform Buffer";
		meshTransformBufferSpec.ResourceBinding = primaryShaderGroup.ResourceLayout[MESH_TRANSFORM_SET][0];
		meshTransformBufferSpec.SetIndex = meshInfoSet;
		m_MeshTransforms = RStorageBuffer::Create(meshTransformBufferSpec);

		// Set 2 Binding 1 - Mesh rendering infos
		StorageBufferSpecification meshRenderingInfoBufferSpec = {};
		meshRenderingInfoBufferSpec.Set = 2;
		meshRenderingInfoBufferSpec.Binding = 1;
		meshRenderingInfoBufferSpec.Size = sizeof(MeshShadingInfo) * ME_MESH_MNG_MAX_MESH_COUNT; // Max 100k instances per draw call
		meshRenderingInfoBufferSpec.MemoryType = MemoryType::RAM;
		meshRenderingInfoBufferSpec.DebugName = "Mesh rendering info buffer";
		meshRenderingInfoBufferSpec.ResourceBinding = primaryShaderGroup.ResourceLayout[MESH_TRANSFORM_SET][1];
		meshRenderingInfoBufferSpec.SetIndex = meshInfoSet;
		m_MeshRenderingInfos = RStorageBuffer::Create(meshRenderingInfoBufferSpec);

		StorageBufferSpecification meshIDsBufferSpec = {};
		meshIDsBufferSpec.Set = 2;
		meshIDsBufferSpec.Binding = 2;
		meshIDsBufferSpec.Size = sizeof(uint32) * ME_MESH_MNG_MAX_MESH_COUNT; // Max 100k instances per draw call
		meshIDsBufferSpec.MemoryType = MemoryType::RAM;
		meshIDsBufferSpec.DebugName = "Mesh ids buffer";
		meshIDsBufferSpec.ResourceBinding = primaryShaderGroup.ResourceLayout[MESH_TRANSFORM_SET][2];
		meshIDsBufferSpec.SetIndex = meshInfoSet;
		m_MeshIDs = RStorageBuffer::Create(meshIDsBufferSpec);

		uint32 frustumCullingSet = RenderCommand::GetResourceHandler()->CreateResourceSet(frustumCullingShaderGroup.ResourceLayout[FRUSTUM_CULLING_SET]);

		// Set 3 Binding 0 - Frustum culling input
		StorageBufferSpecification frustumCullingInput = {};
		frustumCullingInput.Set = FRUSTUM_CULLING_SET;
		frustumCullingInput.Binding = 0;
		frustumCullingInput.Size = sizeof(Render::DrawIndirectIndexedData) * ME_MESH_MNG_MAX_MESH_COUNT; // Max 100k instances per draw call
		frustumCullingInput.MemoryType = MemoryType::RAM;
		frustumCullingInput.DebugName = "Frustum cull input buffer";
		frustumCullingInput.ResourceBinding = primaryShaderGroup.ResourceLayout[FRUSTUM_CULLING_SET][0];
		frustumCullingInput.SetIndex = frustumCullingSet;
		m_InputMeshInfos = RStorageBuffer::Create(frustumCullingInput);

		// Set 3 Binding 1 - Frustum culling output
		IndirectBufferSpecification frustumCullingOutput = {};
		frustumCullingOutput.Set = FRUSTUM_CULLING_SET;
		frustumCullingOutput.Binding = 1;
		frustumCullingOutput.Size = sizeof(DrawIndirectIndexedData) * ME_MESH_MNG_MAX_MESH_COUNT; // Max 100k instances per draw call
		frustumCullingOutput.DebugName = "Frustum cull output buffer";
		frustumCullingOutput.ResourceBinding = primaryShaderGroup.ResourceLayout[FRUSTUM_CULLING_SET][1];
		frustumCullingOutput.SetIndex = frustumCullingSet;
		m_OutputMeshInfos = RIndirectBuffer::Create(frustumCullingOutput);

		// Set 3 Binding 2 - Frustum culling output count
		IndirectBufferSpecification frustumCullingOutputCount = {};
		frustumCullingOutputCount.Set = FRUSTUM_CULLING_SET;
		frustumCullingOutputCount.Binding = 2;
		frustumCullingOutputCount.Size = sizeof(uint32);
		frustumCullingOutputCount.DebugName = "Frustum cull output count buffer";
		frustumCullingOutputCount.ResourceBinding = primaryShaderGroup.ResourceLayout[FRUSTUM_CULLING_SET][2];
		frustumCullingOutputCount.SetIndex = frustumCullingSet;
		m_OutputMeshInfoCount = RIndirectBuffer::Create(frustumCullingOutputCount);

		m_ImguiSet = RenderCommand::GetResourceHandler()->CreateResourceSet(mergeShaderGroup.ResourceLayout[IMGUI_FRAME_SET]);

#pragma endregion

		m_VertexInputFormat = VertexBufferLayout(InputClassification::PerVertex,
			{
						{ ShaderType::Float3, "POSITION", 0, 0 },
						{ ShaderType::Float2, "TCOORD", 1, 0 },
						{ ShaderType::Float3, "NORMALPOS", 2, 0 }
			});
#pragma region Render pass
		ME::Core::Array<ME::Core::Memory::Reference<Texture2D>> swapChainTextures = RenderCommand::Get()->GetSwapChain()->GetImages();
		ME::Core::Array<AttachmentSpecification> attachments;

		AttachmentSpecification baseColorAttachment = {};
		baseColorAttachment.IsDepth = false;
		baseColorAttachment.IsStencil = false;
		baseColorAttachment.AttachmentFormat = m_gBaseColor->GetSpecification().Format;
		baseColorAttachment.LoadOp = LoadOperation::Clear;
		baseColorAttachment.StoreOp = StoreOperation::Store;
		baseColorAttachment.InitialLayout = ImageLayout::Undefined;
		baseColorAttachment.FinalLayout = ImageLayout::ShaderReadOnly;
		baseColorAttachment.SampleCount = m_gBaseColor->GetSpecification().SampleCount;

		AttachmentSpecification normalAttachment = {};
		normalAttachment.IsDepth = false;
		normalAttachment.IsStencil = false;
		normalAttachment.AttachmentFormat = m_gNormal->GetSpecification().Format;
		normalAttachment.LoadOp = LoadOperation::Clear;
		normalAttachment.StoreOp = StoreOperation::Store;
		normalAttachment.InitialLayout = ImageLayout::Undefined;
		normalAttachment.FinalLayout = ImageLayout::ShaderReadOnly;
		normalAttachment.SampleCount = m_gNormal->GetSpecification().SampleCount;

		AttachmentSpecification depthSpecs = {};
		depthSpecs.IsDepth = true;
		depthSpecs.IsStencil = false;
		depthSpecs.AttachmentFormat = m_gDepth->GetSpecification().Format;
		depthSpecs.LoadOp = LoadOperation::Clear;
		depthSpecs.StoreOp = StoreOperation::Store;
		depthSpecs.InitialLayout = ImageLayout::Undefined;
		depthSpecs.FinalLayout = ImageLayout::DepthStencilAttachment;
		depthSpecs.SampleCount = m_gDepth->GetSpecification().SampleCount;

		attachments.EmplaceBack(baseColorAttachment);
		attachments.EmplaceBack(normalAttachment);
		attachments.EmplaceBack(depthSpecs);

		SubpassSpecification subpass = {};
		subpass.ColorAttachmentRefs = { 0, 1 };
		subpass.DepthStencilAttachmentRef = 2;
		subpass.PipelineBindPoint = PipelineBindPoint::Graphics;

		SubpassDependency gPassDep1 = {};
		gPassDep1.SubpassSrc				= ~0u;
		gPassDep1.SubpassDst				= 0;
		gPassDep1.AccessFlagsSrc			= AccessFlags::MemoryRead;
		gPassDep1.AccessFlagsDst			= AccessFlags::ColorAttachmentWrite |
		                                      AccessFlags::ColorAttachmentRead	| 
			                                  AccessFlags::DepthStencilRead		|
			                                  AccessFlags::DepthStencilWrite;
		gPassDep1.PipelineStageFlagsSrc		= PipelineStageFlags::BottomOfPipe;
		gPassDep1.PipelineStageFlagsDst		= PipelineStageFlags::ColorAttachmentOutput |
											  PipelineStageFlags::EarlyFragmentTests |
											  PipelineStageFlags::LateFragmentTests;

		SubpassDependency gPassDep2 = {};
		gPassDep2.SubpassSrc				= 0;
		gPassDep2.SubpassDst				= ~0u;
		gPassDep2.AccessFlagsSrc			= AccessFlags::ColorAttachmentWrite | 
			                                  AccessFlags::DepthStencilWrite;
		gPassDep2.AccessFlagsDst			= AccessFlags::ShaderRead;
		gPassDep2.PipelineStageFlagsSrc		= PipelineStageFlags::ColorAttachmentOutput | PipelineStageFlags::LateFragmentTests;
		gPassDep2.PipelineStageFlagsDst		= PipelineStageFlags::FragmentShader;

		RenderPassSpecification gPassSpecs = {};
		gPassSpecs.AttachmentSpecs = attachments;
		gPassSpecs.SubpassSpecs = { subpass };
		gPassSpecs.SubpassDependencies = { gPassDep1, gPassDep2 };
		gPassSpecs.DebugName = "G-pass";
		m_GPass = RenderPass::Create(gPassSpecs);

		RFramebufferSpecification framebufferSpecification = {};
		framebufferSpecification.RAttachments = { m_gBaseColor, m_gNormal, m_gDepth };
		framebufferSpecification.Layers = 1;
		framebufferSpecification.RenderPass = m_GPass;
		framebufferSpecification.Resolution = gBaseColorSpecification.Resolution;
		m_GFramebuffer = RFramebuffer::Create(framebufferSpecification);

		auto swapChainImages = RenderCommand::Get()->GetSwapChain()->GetImages()[0];

		AttachmentSpecification presentSpecs = {};
		presentSpecs.IsDepth = false;
		presentSpecs.IsStencil = false;
		presentSpecs.AttachmentFormat = swapChainImages->GetSpecification().Format;
		presentSpecs.LoadOp = LoadOperation::Clear;
		presentSpecs.StoreOp = StoreOperation::Store;
		presentSpecs.InitialLayout = ImageLayout::Undefined;
		presentSpecs.FinalLayout = ImageLayout::Present;
		presentSpecs.SampleCount = SampleCount::Count1;

		SubpassSpecification mergeSubpass = {};
		mergeSubpass.ColorAttachmentRefs = { 0 };
		mergeSubpass.DepthStencilAttachmentRef = ~0u;
		mergeSubpass.PipelineBindPoint = PipelineBindPoint::Graphics;

		SubpassDependency mergeDep1 = {};
		mergeDep1.SubpassSrc = ~0u;
		mergeDep1.SubpassDst = 0;
		mergeDep1.AccessFlagsSrc = AccessFlags::None;
		mergeDep1.AccessFlagsDst = AccessFlags::ColorAttachmentWrite;
		mergeDep1.PipelineStageFlagsSrc = PipelineStageFlags::ColorAttachmentOutput;
		mergeDep1.PipelineStageFlagsDst = PipelineStageFlags::ColorAttachmentOutput;

		SubpassDependency mergeDep2 = {};
		mergeDep2.SubpassSrc = 0;
		mergeDep2.SubpassDst = ~0u;
		mergeDep2.AccessFlagsSrc = AccessFlags::ColorAttachmentWrite;
		mergeDep2.AccessFlagsDst = AccessFlags::MemoryRead;
		mergeDep2.PipelineStageFlagsSrc = PipelineStageFlags::ColorAttachmentOutput;
		mergeDep2.PipelineStageFlagsDst = PipelineStageFlags::BottomOfPipe;

		RenderPassSpecification mergePassSpecs = {};
		mergePassSpecs.AttachmentSpecs = { presentSpecs };
		mergePassSpecs.SubpassSpecs = { mergeSubpass };
		mergePassSpecs.SubpassDependencies = { mergeDep1, mergeDep2 };
		mergePassSpecs.DebugName = "Merge pass";
		m_MergePass = RenderPass::Create(mergePassSpecs);

#pragma endregion
		RenderCommand::CreateFramebuffers(m_MergePass, {});
#pragma region Primary graphics pipeline
		{
			InputAssemblySpecification iaSpec = {};
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
			dsSpec.DepthWriteEnabled = true;
			dsSpec.DepthFunction = DepthComparison::Less;
			dsSpec.StencilEnabled = true;
			dsSpec.FrontFace = {};
			dsSpec.BackFace = dsSpec.FrontFace;
		    dsSpec.DepthBoundsEnabled = true;
		    dsSpec.MaxDepthBounds = 1.0f;
			dsSpec.MinDepthBounds = 0.f;

			ColorBlendingSpecification cbSpec = {};
			cbSpec.Attachments = {
				{ false, false  },
			    { false, false  }
			};
			cbSpec.BlendConstants = { 0.f, 0.f, 0.f, 0.f };
			cbSpec.LogicOperation = LogicOperation::None;

			PipelineSpecification geometryPipelineSpecs = {};
			geometryPipelineSpecs.Type = PipelineType::Graphics;
			geometryPipelineSpecs.RenderPass = m_GPass;
			geometryPipelineSpecs.Subpass = 0;
			geometryPipelineSpecs.Shaders = primaryShaderGroup;
			geometryPipelineSpecs.BufferLayout = m_VertexInputFormat;
			geometryPipelineSpecs.InputAssembly = iaSpec;
			geometryPipelineSpecs.Rasterization = rasterSpec;
			geometryPipelineSpecs.Multisampling = multisamplingSpec;
			geometryPipelineSpecs.DepthStencil = dsSpec;
			geometryPipelineSpecs.ColorBlending = cbSpec;
			geometryPipelineSpecs.BasePipeline = nullptr;
			geometryPipelineSpecs.Constants =
			{
				PushConstant(ShaderStage::Vertex, 0, sizeof(PrimaryPipelinePushConstants))
			};
			m_GeometryPipeline = Pipeline::Create(geometryPipelineSpecs);
		}
        {
            VertexBufferLayout vertexFormat = VertexBufferLayout(InputClassification::PerVertex, {});

			InputAssemblySpecification iaSpec = {};
			iaSpec.Topology = PrimitiveTopology::TriangleStrip;
			iaSpec.PrimitiveRestart = false;

			RasterizationSpecification rasterSpec = {};
			rasterSpec.DiscardEnabled = false;
			rasterSpec.FrontCounterClockwise = false;
			rasterSpec.DepthBiasEnabled = false;
			rasterSpec.DepthClampEnabled = false;
			rasterSpec.DepthBiasConstantFactor = 1.0f;
			rasterSpec.DepthBiasClamp = 1.f;
			rasterSpec.DepthBiasSlopeFactor = 1.f;
			rasterSpec.LineWidth = 1.0f;
			rasterSpec.Cull = RasterizationCull::None;
			rasterSpec.Fill = RasterizationFill::Fill;

			MultisamplingSpecification multisamplingSpec = {};
			multisamplingSpec.Samples = SampleCount::Count1;
			multisamplingSpec.EnableSampleShading = false;
			multisamplingSpec.AlphaToOne = false;
			multisamplingSpec.MinSampleShading = 1;
			multisamplingSpec.AlphaToCoverage = false;

			DepthStencilSpecification dsSpec = {};
			dsSpec.DepthEnabled = false;
			dsSpec.StencilEnabled = false;
			dsSpec.MaxDepthBounds = 1.0f;
			dsSpec.MinDepthBounds = 0.f;
			dsSpec.DepthFunction = DepthComparison::Always;

			ColorBlendingSpecification cbSpec = {};
			cbSpec.Attachments = {
			    { false, false },
			};
			cbSpec.BlendConstants = { 0.f, 0.f, 0.f, 0.f };
			cbSpec.LogicOperation = LogicOperation::None;

			PipelineSpecification pipelineSpecs = {};
			pipelineSpecs.Type = PipelineType::Graphics;
			pipelineSpecs.RenderPass = m_MergePass;
			pipelineSpecs.Subpass = 0;
			pipelineSpecs.Shaders = mergeShaderGroup;
			pipelineSpecs.BufferLayout = vertexFormat;
			pipelineSpecs.InputAssembly = iaSpec;
			pipelineSpecs.Rasterization = rasterSpec;
			pipelineSpecs.Multisampling = multisamplingSpec;
			pipelineSpecs.DepthStencil = dsSpec;
			pipelineSpecs.ColorBlending = cbSpec;
			pipelineSpecs.BasePipeline = nullptr;
			pipelineSpecs.Constants = {};
			m_MergePipeline = Pipeline::Create(pipelineSpecs);
        }

#pragma endregion
#pragma region Frustum cull pipeline
		PipelineSpecification frustumPipelineSpecs = {};
		frustumPipelineSpecs.BasePipeline = nullptr;
		frustumPipelineSpecs.Type = PipelineType::Compute;
		frustumPipelineSpecs.ComputeShader = frustumCullingShaderGroup.Shader;
		frustumPipelineSpecs.BasePipeline = nullptr;
        frustumPipelineSpecs.Constants = 
		{
		    PushConstant(ShaderStage::Compute, 0, sizeof(uint32))	
		};

		m_FrustumCullPipeline = Pipeline::Create(frustumPipelineSpecs);
#pragma endregion

		return true;
	}
	void Renderer::ShutdownImpl()
	{
		RenderCommand::Shutdown();
	}

    void Renderer::BeginSceneImpl(ME::Core::Memory::Reference<ME::Render::Camera>& camera)
    {
		// Updating camera if required
		if (camera->BufferUpdateRequired())
		{
			CameraData data = CameraData(camera->GetViewMatrix(), camera->GetProjectionMatrix());
			ME::Core::Math::Frustum frustum = ME::Core::Math::ExtractFrustumFromMatrix(camera->GetProjectionMatrix() * camera->GetViewMatrix());
			m_CurrentCameraBuffer->SetData(m_CurrentCommandBuffer, &data, sizeof(CameraData), 0);
			m_CurrentCameraFrustumBuffer->SetData(m_CurrentCommandBuffer, &frustum, sizeof(ME::Core::Math::Frustum), 0);
		}

		// Set 0
		Manager::MeshManager::Get().GetVertexBuffer()->Write();
		Manager::MeshManager::Get().GetIndexBuffer()->Write();		
		Manager::MeshManager::Get().GetMeshletBuffer()->Write();	// Unused
		Manager::MeshManager::Get().GetMeshBoxBuffer()->Write();	// In FrustumCulling shader
		Manager::MeshManager::Get().GetDrawBuffer()->Write();		// Unused
        // Set 1
		m_CurrentCameraBuffer->Write();
		m_CurrentCameraFrustumBuffer->Write();
    }

	void Renderer::EndSceneImpl()
	{
		ProcessQueuedMeshes();		
		MergeStage();
	}

    void Renderer::BeginFrameImpl()
    {
		Render::RenderCommand::NewFrame();
		AcquireNewBuffers();
    }

    void Renderer::EndFrameImpl()
    {
		m_QueuedMeshes.Clear();

        Render::RenderCommand::EndFrame();
		Render::RenderCommand::Present();
    }

    void Renderer::RenderMeshImpl(Components::MeshComponent& meshComponent,
        Components::TransformComponent& transformComponent)
    {
		if (!meshComponent.Visible) return;
		if (!meshComponent.Mesh) return;
		if (!meshComponent.Mesh->IsLoaded()) return;

		MeshShadingInfo renderingInfo = {};
		renderingInfo.Opacity = meshComponent.Opacity;
		renderingInfo.ShadowsVisible = meshComponent.ShadowsVisible;

		if (m_QueuedMeshes.Contains(meshComponent.Mesh->GetMeshID()))
		{
			m_QueuedMeshes[meshComponent.Mesh->GetMeshID()].Transforms.EmplaceBack(transformComponent.Transform.Matrix());
			m_QueuedMeshes[meshComponent.Mesh->GetMeshID()].MeshRenderingInfos.EmplaceBack(renderingInfo);
			m_QueuedMeshes[meshComponent.Mesh->GetMeshID()].MeshIDs.EmplaceBack(static_cast<uint32>(meshComponent.Mesh->GetMeshID()));
			m_QueuedMeshes[meshComponent.Mesh->GetMeshID()].Data.instanceCount++;
		    return;
		}

		MeshConstants constants = {};
		constants.MeshID = static_cast<uint32>(meshComponent.Mesh->GetMeshID());

		MeshInfos infos = {};
		infos.Transforms = { transformComponent.Transform.Matrix() };
		infos.MeshRenderingInfos = { renderingInfo };
		infos.MeshIDs = { constants.MeshID };
		infos.MeshInfo = constants;
		infos.Data.instanceCount = 1;
		infos.Data.firstIndex = static_cast<uint32>(meshComponent.Mesh->GetIndexAllocation()->Offset) / sizeof(uint32);
		infos.Data.indexCount = static_cast<uint32>(meshComponent.Mesh->GetIndexAllocation()->Size) / sizeof(uint32);
		infos.Data.vertexOffset = static_cast<uint32>(meshComponent.Mesh->GetVertexAllocation()->Offset) / sizeof(Assets::Vertex);

		m_QueuedMeshes.Insert(meshComponent.Mesh->GetMeshID(), infos);
    }

    ME::Core::Memory::Reference<ME::Render::Pipeline> Renderer::GetPipelineImpl()
    {
        return m_GeometryPipeline;
    }

    ME::Core::Memory::Reference<ME::Render::RenderPass> Renderer::GetRenderPassImpl()
    {
        return m_GPass;
    }

    ME::Core::Memory::Reference<ME::Render::RTexture2D> Renderer::GetDepthBufferImpl()
    {
        return m_gDepth;
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
		uint32 currentInstanceId = 0;
		uint32 maxInstanceCount = 0;

		for (auto& mesh : m_QueuedMeshes)
		{
			mesh.Value2.Data.firstInstance = currentInstanceId;
			m_CurrentInputMeshInfos->SetData(&mesh.Value2.Data,
				sizeof(DrawIndirectIndexedData),
				currentInstanceId * sizeof(DrawIndirectIndexedData));
			m_CurrentMeshTransforms->SetData(mesh.Value2.Transforms.Data(),
				mesh.Value2.Transforms.Size() * sizeof(Core::Math::Matrix4x4),
				currentInstanceId * sizeof(Core::Math::Matrix4x4)
			);
			m_CurrentMeshRenderingInfos->SetData(mesh.Value2.MeshRenderingInfos.Data(),
				mesh.Value2.MeshRenderingInfos.Size() * sizeof(MeshShadingInfo),
				currentInstanceId * sizeof(MeshShadingInfo)
			);
			m_CurrentMeshIDs->SetData(mesh.Value2.MeshIDs.Data(),
				mesh.Value2.MeshIDs.Size() * sizeof(uint32),
				currentInstanceId * sizeof(uint32));
			currentInstanceId += mesh.Value2.Data.instanceCount;
			maxInstanceCount = Core::Algorithm::Max(maxInstanceCount, mesh.Value2.Data.instanceCount);
		}

		m_CurrentInputMeshInfos->Write();
		m_CurrentOutputMeshInfos->Write();
		m_CurrentOutputMeshInfoCount->Write();
		m_CurrentMeshTransforms->Write();
		m_CurrentMeshIDs->Write();
		m_CurrentMeshRenderingInfos->Write();

		Manager::MeshManager::Get().GetVertexBuffer()->Bind(m_CurrentCommandBuffer, m_FrustumCullPipeline);
		m_CurrentCameraFrustumBuffer->Bind(m_CurrentCommandBuffer, m_FrustumCullPipeline);
		m_CurrentMeshTransforms->Bind(m_CurrentCommandBuffer, m_FrustumCullPipeline);
		m_CurrentInputMeshInfos->Bind(m_CurrentCommandBuffer, m_FrustumCullPipeline);

		uint32 meshCount = static_cast<uint32>(m_QueuedMeshes.Size());
		m_FrustumCullPipeline->SetConstants(m_CurrentCommandBuffer, ShaderStage::Compute, &meshCount, sizeof(uint32));
		m_FrustumCullPipeline->Bind(m_CurrentCommandBuffer);

        RenderCommand::Dispatch(m_CurrentCommandBuffer, 
			static_cast<uint32>((static_cast<uint32>(m_QueuedMeshes.Size()) + 127) / 128), (maxInstanceCount + 3) / 4, 1);

		m_CurrentOutputMeshInfos->Barrier(m_CurrentCommandBuffer,
			{ AccessFlags::ShaderWrite, PipelineStageFlags::ComputeShader },
			{ AccessFlags::IndirectCommandRead, PipelineStageFlags::DrawIndirect });
		m_CurrentOutputMeshInfoCount->Barrier(m_CurrentCommandBuffer,
			{ AccessFlags::ShaderWrite, PipelineStageFlags::ComputeShader }, 
			{ AccessFlags::IndirectCommandRead, PipelineStageFlags::DrawIndirect });

		Render::ClearValue clrVal = {};
		clrVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f);
		Render::ClearValue zeroVal = {};
		zeroVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 0.f);
		Render::ClearValue depthClrVal = {};
		depthClrVal.UsingDepth = true;
		depthClrVal.DepthClearValue = { .Depth = 1.f, .Stencil = 0 };

		Render::RenderPassBeginInfo beginInfo = {};
		beginInfo.Framebuffer = m_CurrentGFramebuffer;
		beginInfo.ClearValues = { clrVal, zeroVal, depthClrVal };
		beginInfo.RenderArea.Offset = { 0,0 };
		beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

		static Core::Math::Rect2D scissor = {};
		scissor.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();
		scissor.Offset.x = 0;
		scissor.Offset.y = 0;

		m_GPass->Begin(m_CurrentCommandBuffer, beginInfo);

        m_GeometryPipeline->SetViewports(m_CurrentCommandBuffer, { {0,0,scissor.Extent.x, scissor.Extent.y, 0, 1} });
		m_GeometryPipeline->SetScissors(m_CurrentCommandBuffer, { scissor });
		m_GeometryPipeline->Bind(m_CurrentCommandBuffer);

		Manager::MeshManager::Get().GetVertexBuffer()->Bind(m_CurrentCommandBuffer, m_GeometryPipeline);
		m_CurrentCameraFrustumBuffer->Bind(m_CurrentCommandBuffer, m_GeometryPipeline);
		m_CurrentMeshTransforms->Bind(m_CurrentCommandBuffer, m_GeometryPipeline);
		m_CurrentInputMeshInfos->Bind(m_CurrentCommandBuffer, m_GeometryPipeline);

		Manager::MeshManager::Get().GetVertexBuffer()->Bind(m_CurrentCommandBuffer, 0);
		Manager::MeshManager::Get().GetIndexBuffer()->Bind(m_CurrentCommandBuffer, 0);

		RenderCommand::DrawIndexedIndirectCount(m_CurrentCommandBuffer, PipelineStageFlags::ComputeShader,
			m_CurrentOutputMeshInfos, 0,
			m_CurrentOutputMeshInfoCount, 0,
			static_cast<uint32>(m_QueuedMeshes.Size()), sizeof(DrawIndirectIndexedData));	
    
		m_GPass->End(m_CurrentCommandBuffer);
	}

    void Renderer::AcquireNewBuffers()
    {
		m_CurrentCommandBuffer = RenderCommand::GetAvailableCommandBuffer();

		m_CurrentGFramebuffer = m_GFramebuffer->AcquireNextBuffer();

		m_CurrentCameraBuffer = m_CameraBuffer->AcquireNextBuffer();
		m_CurrentCameraFrustumBuffer = m_CameraFrustumBuffer->AcquireNextBuffer();

		m_CurrentMeshTransforms = m_MeshTransforms->AcquireNextBuffer();
		m_CurrentMeshRenderingInfos = m_MeshRenderingInfos->AcquireNextBuffer();
		m_CurrentMeshIDs = m_MeshIDs->AcquireNextBuffer();

		m_CurrentInputMeshInfos = m_InputMeshInfos->AcquireNextBuffer();
		m_CurrentOutputMeshInfos = m_OutputMeshInfos->AcquireNextBuffer();
		m_CurrentOutputMeshInfoCount = m_OutputMeshInfoCount->AcquireNextBuffer();
    }

    void Renderer::MergeStage()
	{
		ME::Core::Memory::Reference<ME::Render::Texture2D> renderedFrame = m_gBaseColor->GetTextures()[m_GFramebuffer->GetCurrentBuffer()];

		Render::RenderPassBeginInfo beginInfo = {};
		beginInfo.Framebuffer = RenderCommand::GetAvailableFramebuffer();
		beginInfo.ClearValues = { {Core::Math::Vector4D(0.f, 0.f, 0.f, 0.f)} };
		beginInfo.RenderArea = {};
		beginInfo.RenderArea.Offset = { 0,0 };
		beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

		static Core::Math::Rect2D scissor = {};
		scissor.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();
		scissor.Offset.x = 0;
		scissor.Offset.y = 0;

		m_MergePass->Begin(m_CurrentCommandBuffer, beginInfo);

		m_MergePipeline->SetViewports(m_CurrentCommandBuffer, { {0,0,scissor.Extent.x, scissor.Extent.y, 0, 1} });
		m_MergePipeline->SetScissors(m_CurrentCommandBuffer, { scissor });
		m_MergePipeline->Bind(m_CurrentCommandBuffer);

		renderedFrame->Write();
		renderedFrame->Bind(m_CurrentCommandBuffer, m_MergePipeline);

		RenderCommand::Draw(m_CurrentCommandBuffer, 4, 1, 0, 0);
		m_MergePass->End(m_CurrentCommandBuffer);
	}
}
