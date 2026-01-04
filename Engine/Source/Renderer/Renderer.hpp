#pragma once
#include <Core.hpp>

#include "Base/RenderAPI.hpp"
#include "Base/Pipeline.hpp"
#include "Base/RingBuffer.hpp"
#include "Camera.hpp"
#include "Framework/Components/MeshComponent.hpp"
#include "Framework/Components/TransformComponent.hpp"

#define ME_ARGUMENTS(...) __VA_ARGS__

#define ME_ARGUMENT_VOID 

#define ME_RENDERER_METHOD(returnType, methodName,  argumentPack, argumentDefinitions)	\
public:																					\
    static returnType methodName(argumentDefinitions)									\
    {																					\
        return Get().methodName ## Impl(argumentPack);									\
    }																					\
private:																				\
    returnType methodName ## Impl(argumentDefinitions)

namespace ME::Render
{
	constexpr uint32 PRIMARY_PIPELINE_X_THREAD_COUNT = 32;
	constexpr uint32 PRIMARY_PIPELINE_Y_THREAD_COUNT = 4;

	constexpr uint32 VERTEX_SET = 0;
	constexpr uint32 LIGHT_SET = 0;
	constexpr uint32 CAMERA_SET = 1;
	constexpr uint32 MESH_TRANSFORM_SET = 2;
	constexpr uint32 FRUSTUM_CULLING_SET = 3;
	constexpr uint32 GBUFFER_SET = 4;
	constexpr uint32 FRAME_SET = 5;
	constexpr uint32 IMGUI_FRAME_SET = 0;

	constexpr char8 ME_RENDER_PRIMARY_PIPELINE_NAME[] = TEXT("Primary");
	constexpr char8 ME_RENDER_LIGHT_PIPELINE_NAME[] = TEXT("Light");
	constexpr char8 ME_RENDER_MERGE_PIPELINE_NAME[] = TEXT("Merge");

	class MEAPI Renderer
	{
    private:
		struct alignas(16) CameraData
		{
			ME::Core::Math::Matrix4x4 View;
			ME::Core::Math::Matrix4x4 Proj;
			ME::Core::Math::Matrix4x4 InvertedView;
			ME::Core::Math::Matrix4x4 InvertedProj;
		};

		struct alignas(16) MeshShadingInfo
		{
			float32 Opacity;
			uint32 ShadowsVisible;
			uint32 Padding[2];
		};

		struct alignas(16) MeshConstants
		{
			uint32 MeshID;
			uint32 Padding[3];
		};

	    struct alignas(16) MeshInfos
		{
			ME::Core::Array<MeshShadingInfo> MeshRenderingInfos;
			ME::Core::Array<ME::Core::Math::Matrix4x4> Transforms;
			ME::Core::Array<uint32> MeshIDs;
			MeshConstants MeshInfo;
			DrawIndirectIndexedData Data;
			uint32 Padding[3];
		};

		struct alignas(16) GeometryPipelineConstants
		{
			uint32 InstanceCount;
			uint32 CurrentMeshID;
		    uint32 Padding[2];
		};

		struct alignas(16) LightPipelineConstants
		{
			uint32 MaxCount;
			uint32 DirectionalLightCount;
			uint32 PointLightCount;
			uint32 SpotLightCount;
			Core::Math::Vector3D CameraPosition;
			uint32 Padding1;
			Core::Math::Vector3D WarmColor;
			uint32 Padding2;
		};

	public:
		Renderer();
		~Renderer();
		
	public:
		static Renderer& Get()
		{
            static Renderer instance;
			return instance;
		}

		ME_RENDERER_METHOD(bool, Init, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);
		ME_RENDERER_METHOD(void, Shutdown, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);

	    ME_RENDERER_METHOD(void, BeginScene, 
			ME_ARGUMENTS(camera), 
			ME_ARGUMENTS(ME::Core::Memory::Reference<ME::Render::Camera>& camera));

		ME_RENDERER_METHOD(void, EndScene, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);
		ME_RENDERER_METHOD(void, BeginFrame, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);
		ME_RENDERER_METHOD(void, EndFrame, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);

		ME_RENDERER_METHOD(void, RenderMesh, 
			ME_ARGUMENTS(meshComponent, transformComponent),
			ME_ARGUMENTS(Components::MeshComponent& meshComponent,
			Components::TransformComponent& transformComponent));

		ME_RENDERER_METHOD(ME::Core::Memory::Reference<ME::Render::Pipeline>, GetPipeline, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);
		ME_RENDERER_METHOD(ME::Core::Memory::Reference<ME::Render::RenderPass>, GetRenderPass, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);
		ME_RENDERER_METHOD(ME::Core::Memory::Reference<ME::Render::RTexture2D>, GetDepthBuffer, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);

	public:
		inline static void SetRenderAPI(RenderAPI::API api);
		inline static RenderAPI::API GetRenderAPI();

	private:
		void ProcessQueuedMeshes();
		void AcquireNewBuffers();
		void LightStage();
		void MergeStage();

	private:
		ME::Core::Memory::Reference<ME::Render::Pipeline> m_FrustumCullPipeline;
		// G-Pass
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_GPass;
		ME::Core::Memory::Reference<ME::Render::RFramebuffer> m_GFramebuffer;
		ME::Core::Memory::Reference<ME::Render::Pipeline> m_GeometryPipeline;
		
	    // Light
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_Frames;
		ME::Core::Memory::Reference<ME::Render::RFramebuffer> m_LightFramebuffer;
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_LightPass;
		ME::Core::Memory::Reference<ME::Render::Pipeline> m_LightPipeline;
	    
	    // Merge
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_MergePass;
		ME::Core::Memory::Reference<ME::Render::Pipeline> m_MergePipeline;
		ME::Render::VertexBufferLayout m_VertexInputFormat;

	private:
		// G-Buffers
	    ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gBaseColor;
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gNormal;
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gWorldPosition;
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gEmissive;
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gSpecular;
		ME::Core::Memory::Reference<ME::Render::RTexture2D> m_gDepth;

	    ME::Core::Memory::Reference<ME::Render::RUniform> m_CameraBuffer;
		ME::Core::Memory::Reference<ME::Render::RUniform> m_CameraFrustumBuffer;

	    ME::Core::Memory::Reference<ME::Render::RStorageBuffer> m_MeshTransforms;
	    ME::Core::Memory::Reference<ME::Render::RStorageBuffer> m_MeshRenderingInfos;
	    ME::Core::Memory::Reference<ME::Render::RStorageBuffer> m_MeshIDs;

		ME::Core::Memory::Reference<ME::Render::RStorageBuffer> m_InputMeshInfos;
		ME::Core::Memory::Reference<ME::Render::RIndirectBuffer> m_OutputMeshInfos;
		ME::Core::Memory::Reference<ME::Render::RIndirectBuffer> m_OutputMeshInfoCount;

	    uint32 m_ImguiSet;

	private:
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> m_CurrentCommandBuffer;

		ME::Core::Memory::Reference<ME::Render::Framebuffer> m_CurrentLightFramebuffer;
		
		ME::Core::Memory::Reference<ME::Render::Framebuffer> m_CurrentGFramebuffer;

		ME::Core::Memory::Reference<ME::Render::Uniform> m_CurrentCameraBuffer;
		ME::Core::Memory::Reference<ME::Render::Uniform> m_CurrentCameraFrustumBuffer;

		ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_CurrentMeshTransforms;
		ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_CurrentMeshRenderingInfos;
		ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_CurrentMeshIDs;

		ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_CurrentInputMeshInfos;
		ME::Core::Memory::Reference<ME::Render::IndirectBuffer> m_CurrentOutputMeshInfos;
		ME::Core::Memory::Reference<ME::Render::IndirectBuffer> m_CurrentOutputMeshInfoCount;

		ME::Core::Memory::WeakReference<ME::Render::Camera> m_CurrentCamera;

	private:
	    ME::Core::UnorderedMap<uint64, MeshInfos> m_QueuedMeshes;
	    ME::Core::Array<DrawIndirectIndexedData> m_DrawIndirectData;
	};
}


#undef ME_RENDERER_METHOD
#undef ME_ARGUMENT_VOID
#undef ME_ARGUMENTS