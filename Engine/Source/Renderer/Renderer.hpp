#pragma once

#include <Core.hpp>
#include "Base/RenderAPI.h"
#include "Base/Buffer.hpp"
#include "ECS/Scene.hpp"
#include "Utility/AssetLoader.h"

#define ME_ARGUMENTS(...) __VA_ARGS__

#define ME_ARGUMENT_VOID 

#define ME_RENDERER_METHOD(returnType, methodName,  argumentPack, argumentDefinitions)	\
public:																					\
    static returnType methodName(argumentDefinitions)									\
    {																					\
        return Get().methodName ## Impl(argumentPack);							\
    }																					\
private:																				\
    returnType methodName ## Impl(argumentDefinitions)

namespace ME::Render
{
	class MEAPI Renderer
	{

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
			ME_ARGUMENTS(scene), 
			ME_ARGUMENTS(const ME::ECS::Scene& scene));
		ME_RENDERER_METHOD(void, EndScene, ME_ARGUMENT_VOID, ME_ARGUMENT_VOID);

		ME_RENDERER_METHOD(void, RenderMesh, 
			ME_ARGUMENTS(mesh, transform), 
			ME_ARGUMENTS(const ME::Core::Memory::Reference<ME::Assets::Mesh>& mesh, 
				        const ME::Core::Math::Matrix4x4& transform));

	public:
		inline static void SetRenderAPI(RenderAPI::API api);
		inline static RenderAPI::API GetRenderAPI();

	private:
		void ProcessQueuedMeshes();

	private:
		ME::Core::Memory::Reference<ME::Render::Pipeline> m_PrimaryPipeline;
		ME::Core::Memory::Reference<ME::Render::RenderPass> m_PrimaryRenderPass;
		// Buffers
		ME::Core::Memory::Reference<ME::Render::Texture2D> m_DepthBuffer;
	    ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_MeshTransforms;
		ME::Render::VertexBufferLayout m_VertexInputFormat;

	private:
	    ME::Core::Containers::UnorderedMap<uint64, ME::Core::Containers::Array<ME::Core::Math::Matrix4x4>> m_QueuedMeshes;
		//ME::Core::

	};

}
