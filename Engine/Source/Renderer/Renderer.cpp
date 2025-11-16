#include "Renderer.hpp"
#include <Core/Containers/Map.hpp>
#include <Core/Containers/UnorderedMap.hpp>

#include "EngineDefines.hpp"
#include "RenderCommand.hpp"
#include "Managers/MeshManager.hpp"

constexpr uint32 VERTEX_SET = 0;
constexpr uint32 CAMERA_SET = 1;
constexpr uint32 MESH_TRANSFORM_SET = 2;
constexpr uint32 FRUSTUM_CULLING_SET = 3;
constexpr char8 FRUSTUM_CULLING_SHADER[] = TEXT("Frustum_Culling");

namespace ME::Render
{
    Renderer::Renderer()
    :	m_PrimaryPipeline(nullptr),
		m_PrimaryRenderPass(nullptr),
		m_DepthBuffer(nullptr),
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
		Render::ResourceLayout frustumResultSet;
		// Input | Binding 0
		frustumResultSet.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);
		// OutputArray | Binding 1
		frustumResultSet.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);
		// OutputCount | Binding 2
		frustumResultSet.EmplaceBack(
			Render::ResourceType::StorageBuffer,
			Render::ShaderStage::Vertex |
			Render::ShaderStage::Task |
			Render::ShaderStage::Mesh |
			Render::ShaderStage::Compute
		);

		Render::ResourceLayoutPack primaryPipelineSets = {};
		primaryPipelineSets.EmplaceBack(vertexSet);
		primaryPipelineSets.EmplaceBack(cameraDataSet);
		primaryPipelineSets.EmplaceBack(meshDataSet);
		primaryPipelineSets.EmplaceBack(frustumResultSet);

        ME::Render::Manager::ShaderGroupSpecification primaryShaderGroupSpecs = {};
		primaryShaderGroupSpecs.Type = Render::Manager::ShaderGroupType::Vertex;
		primaryShaderGroupSpecs.Pixel = TEXT("PrimaryPipeline/PixelPrimary.hlsl");
		primaryShaderGroupSpecs.Vertex.Vertex = TEXT("PrimaryPipeline/VertexPrimary.hlsl");
		primaryShaderGroupSpecs.Layout = primaryPipelineSets;
		primaryShaderGroupSpecs.ShaderGroupName = ME_RENDER_PRIMARY_PIPELINE_NAME;
		primaryShaderGroupSpecs.Defines = {};

		ME::Render::Manager::ShaderManager::Get().LoadShaders(primaryShaderGroupSpecs);
        Manager::ShaderManager::ShaderGroup primaryShaderGroup = 
			Manager::ShaderManager::GetShaderGroup(ME_RENDER_PRIMARY_PIPELINE_NAME);
		Manager::ShaderManager::ComputeShaderGroup frustumCullingShaderGroup;

        ME::Render::Manager::ShaderGroupSpecification frustumCullShader = {};
		frustumCullShader.Type = Manager::ShaderGroupType::Compute;
		frustumCullShader.Layout = primaryShaderGroup.ResourceLayout;
		frustumCullShader.Compute = TEXT("PrimaryPipeline/FrustumCulling.hlsl");
		frustumCullShader.ShaderGroupName = FRUSTUM_CULLING_SHADER;
		frustumCullShader.Defines = {};

		ME::Render::Manager::ShaderManager::Get().LoadShaders(frustumCullShader);
		frustumCullingShaderGroup = Manager::ShaderManager::GetComputeShaderGroup(FRUSTUM_CULLING_SHADER);

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
		depthBufferSpecification.MinFilter = SamplerFilter::Linear;
		depthBufferSpecification.MagFilter = SamplerFilter::Linear;
		depthBufferSpecification.Usage = ImageUsageFlags::DepthStencilAttachment;
		m_DepthBuffer = RTexture2D::Create(depthBufferSpecification);

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

		auto imageSpec = swapChainTextures[0]->GetSpecification();
		AttachmentSpecification attachmentSpecs = {};
		attachmentSpecs.IsDepth = false;
		attachmentSpecs.IsStencil = false;
		attachmentSpecs.AttachmentFormat = imageSpec.Format;
		attachmentSpecs.LoadOp = LoadOperation::Clear;
		attachmentSpecs.StoreOp = StoreOperation::Store;
		attachmentSpecs.InitialLayout = ImageLayout::Undefined;
		attachmentSpecs.FinalLayout = ImageLayout::ColorAttachment;
		attachmentSpecs.SampleCount = SampleCount::Count1;

		AttachmentSpecification depthSpecs = {};
		depthSpecs.IsDepth = true;
		depthSpecs.IsStencil = true;
		depthSpecs.AttachmentFormat = m_DepthBuffer->GetSpecification().Format;
		depthSpecs.LoadOp = LoadOperation::Clear;
		depthSpecs.StoreOp = StoreOperation::DontCare;
		depthSpecs.InitialLayout = ImageLayout::Undefined;
		depthSpecs.FinalLayout = ImageLayout::DepthStencilAttachment;
		depthSpecs.SampleCount = m_DepthBuffer->GetSpecification().SampleCount;

		attachments.EmplaceBack(attachmentSpecs);
		attachments.EmplaceBack(depthSpecs);

		SubpassSpecification subpass = {};
		subpass.ColorAttachmentRefs = { 0 };
		subpass.DepthStencilAttachmentRef = 1;
		subpass.PipelineBindPoint = PipelineBindPoint::Graphics;

		SubpassDependency subpassDependency = {};
		subpassDependency.SubpassSrc = ~0u;
		subpassDependency.SubpassDst = 0;
		subpassDependency.AccessFlagsSrc = AccessFlags::ColorAttachmentWrite;
		subpassDependency.AccessFlagsDst = AccessFlags::ColorAttachmentWrite | AccessFlags::DepthStencilWrite;
		subpassDependency.PipelineStageFlagsSrc = PipelineStageFlags::ColorAttachmentOutput;
		subpassDependency.PipelineStageFlagsDst = PipelineStageFlags::ColorAttachmentOutput | PipelineStageFlags::EarlyFragmentTests;

		RenderPassSpecification mainRPSpecs = {};
		mainRPSpecs.AttachmentSpecs = attachments;
		mainRPSpecs.SubpassSpecs = { subpass };
		mainRPSpecs.SubpassDependencies = { subpassDependency };
		mainRPSpecs.DebugName = "Main render pass";

		m_PrimaryRenderPass = RenderPass::Create(mainRPSpecs);
#pragma endregion
		RenderCommand::CreateFramebuffers(m_PrimaryRenderPass, { m_DepthBuffer });
#pragma region Primary graphics pipeline
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
		dsSpec.DepthFunction = DepthComparison::LessEqual;

		ColorBlendingSpecification cbSpec = {};
		cbSpec.Attachments = { { true, true  } };
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
		mainGraphicsSpecs.Constants = 
		{
		    PushConstant(ShaderStage::Vertex, 0, sizeof(PrimaryPipelinePushConstants))
		};

		m_PrimaryPipeline = Pipeline::Create(mainGraphicsSpecs);
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
		RenderCommand::WriteResource(Manager::MeshManager::Get().GetVertexBuffer());
		RenderCommand::WriteResource(Manager::MeshManager::Get().GetIndexBuffer());		
		RenderCommand::WriteResource(Manager::MeshManager::Get().GetMeshletBuffer());	// Unused
		RenderCommand::WriteResource(Manager::MeshManager::Get().GetMeshBoxBuffer());	// In FrustumCulling shader
		RenderCommand::WriteResource(Manager::MeshManager::Get().GetDrawBuffer());		// Unused
        // Set 1
		RenderCommand::WriteResource(m_CurrentCameraBuffer);
		RenderCommand::WriteResource(m_CurrentCameraFrustumBuffer);
    }

	void Renderer::EndSceneImpl()
	{
		ProcessQueuedMeshes();
		RenderCommand::EndRenderPass(m_CurrentCommandBuffer);
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
		infos.Data.firstIndex = meshComponent.Mesh->GetIndexAllocation()->Offset / sizeof(uint32);
		infos.Data.indexCount = meshComponent.Mesh->GetIndexAllocation()->Size / sizeof(uint32);
		infos.Data.vertexOffset = meshComponent.Mesh->GetVertexAllocation()->Offset / sizeof(Assets::Vertex);

		m_QueuedMeshes.Insert(meshComponent.Mesh->GetMeshID(), infos);
    }

    ME::Core::Memory::Reference<ME::Render::Pipeline> Renderer::GetPipelineImpl()
    {
        return m_PrimaryPipeline;
    }

    ME::Core::Memory::Reference<ME::Render::RenderPass> Renderer::GetRenderPassImpl()
    {
        return m_PrimaryRenderPass;
    }

    ME::Core::Memory::Reference<ME::Render::RTexture2D> Renderer::GetDepthBufferImpl()
    {
        return m_DepthBuffer;
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
		RenderCommand::WriteResource(m_CurrentInputMeshInfos);
		RenderCommand::WriteResource(m_CurrentOutputMeshInfos);
		RenderCommand::WriteResource(m_CurrentOutputMeshInfoCount);
		RenderCommand::WriteResource(m_CurrentMeshTransforms);
		RenderCommand::WriteResource(m_CurrentMeshIDs);
		RenderCommand::WriteResource(m_CurrentMeshRenderingInfos);

		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_FrustumCullPipeline, Manager::MeshManager::Get().GetVertexBuffer());
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_FrustumCullPipeline, m_CurrentCameraFrustumBuffer);
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_FrustumCullPipeline, m_CurrentMeshTransforms);
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_FrustumCullPipeline, m_CurrentInputMeshInfos);

		uint32 meshCount = static_cast<uint32>(m_QueuedMeshes.Size());
		m_FrustumCullPipeline->SetConstants(m_CurrentCommandBuffer, ShaderStage::Compute, &meshCount, sizeof(uint32));
		m_FrustumCullPipeline->Bind(m_CurrentCommandBuffer);
        RenderCommand::Dispatch(m_CurrentCommandBuffer, 
			static_cast<uint32>((static_cast<uint32>(m_QueuedMeshes.Size()) + 127) / 128), (maxInstanceCount + 3) / 4, 1);

		RenderCommand::BufferBarrier(m_CurrentCommandBuffer, m_CurrentOutputMeshInfos,
			{ AccessFlags::ShaderWrite, PipelineStageFlags::ComputeShader },
			{ AccessFlags::IndirectCommandRead, PipelineStageFlags::DrawIndirect });
		RenderCommand::BufferBarrier(m_CurrentCommandBuffer, m_CurrentOutputMeshInfoCount, 
			{ AccessFlags::ShaderWrite, PipelineStageFlags::ComputeShader }, 
			{ AccessFlags::IndirectCommandRead, PipelineStageFlags::DrawIndirect });

		Render::ClearValue clrVal = {};
		clrVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f);
		Render::ClearValue depthClrVal = {};
		clrVal.DepthClearValue = { .Depth = 1.f, .Stencil = 0 };

		Render::RenderPassBeginInfo beginInfo = {};
		beginInfo.RenderPass = m_PrimaryRenderPass;
		beginInfo.Framebuffer = Render::RenderCommand::GetAvailableFramebuffer();
		beginInfo.ClearValues = { clrVal, depthClrVal };
		beginInfo.RenderArea = {};
		beginInfo.RenderArea.Offset = { 0,0 };
		beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

		static Core::Math::Rect2D scissor = {};
		scissor.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();
		scissor.Offset.x = 0;
		scissor.Offset.y = 0;

		Render::RenderCommand::BeginRenderPass(m_CurrentCommandBuffer, beginInfo);

		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, Manager::MeshManager::Get().GetVertexBuffer());
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, m_CurrentCameraBuffer);
		m_PrimaryPipeline->SetViewports(m_CurrentCommandBuffer, { {0,0,scissor.Extent.x, scissor.Extent.y, 0, 1} });
		m_PrimaryPipeline->SetScissors(m_CurrentCommandBuffer, { scissor });
		m_PrimaryPipeline->Bind(m_CurrentCommandBuffer);

		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, Manager::MeshManager::Get().GetVertexBuffer());
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, m_CurrentCameraFrustumBuffer);
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, m_CurrentMeshTransforms);
		RenderCommand::BindResourceSet(m_CurrentCommandBuffer, m_PrimaryPipeline, m_CurrentInputMeshInfos);

        RenderCommand::BindVertexBuffer(m_CurrentCommandBuffer, Manager::MeshManager::Get().GetVertexBuffer(), 0);
        RenderCommand::BindIndexBuffer(m_CurrentCommandBuffer, Manager::MeshManager::Get().GetIndexBuffer(), 0);
		RenderCommand::DrawIndexedIndirectCount(m_CurrentCommandBuffer, PipelineStageFlags::ComputeShader,
			m_CurrentOutputMeshInfos, 0,
			m_CurrentOutputMeshInfoCount, 0,
			static_cast<uint32>(m_QueuedMeshes.Size()), sizeof(DrawIndirectIndexedData));	
    }

    void Renderer::AcquireNewBuffers()
    {
		m_CurrentCommandBuffer = RenderCommand::GetAvailableCommandBuffer();

		m_CurrentCameraBuffer = m_CameraBuffer->AcquireNextBuffer();
		m_CurrentCameraFrustumBuffer = m_CameraFrustumBuffer->AcquireNextBuffer();

		m_CurrentMeshTransforms = m_MeshTransforms->AcquireNextBuffer();
		m_CurrentMeshRenderingInfos = m_MeshRenderingInfos->AcquireNextBuffer();
		m_CurrentMeshIDs = m_MeshIDs->AcquireNextBuffer();

		m_CurrentInputMeshInfos = m_InputMeshInfos->AcquireNextBuffer();
		m_CurrentOutputMeshInfos = m_OutputMeshInfos->AcquireNextBuffer();
		m_CurrentOutputMeshInfoCount = m_OutputMeshInfoCount->AcquireNextBuffer();
    }
}
