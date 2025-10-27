#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>

#include "Renderer/Base/Buffer.hpp"
#include "Renderer/Base/Framebuffer.h"
#include "Renderer/Base/CommandBuffer.h"
#include "Renderer/Base/Texture.h"
#include "Renderer/Base/RenderPass.h"
#include "Renderer/Base/Shader.h"
#include "Renderer/Base/Pipeline.h"


namespace ME::Render
{
	class MEAPI RenderResourcesTracker
	{
	public:
		RenderResourcesTracker();
		~RenderResourcesTracker();

	public:
		inline static RenderResourcesTracker& Get()
		{
			static RenderResourcesTracker instance;
			return instance;
		}

	public:
		void ShutdownAll();
		void AddItem(const ME::Core::Memory::Reference<VertexBuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<IndexBuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<Uniform>& buffer);
		void AddItem(const ME::Core::Memory::Reference<StorageBuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<IndirectBuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<Texture>& texture);
		void AddItem(const ME::Core::Memory::Reference<Framebuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<CommandBuffer>& buffer);
		void AddItem(const ME::Core::Memory::Reference<Shader>& shader);
		void AddItem(const ME::Core::Memory::Reference<Pipeline>& pipeline);
		void AddItem(const ME::Core::Memory::Reference<RenderPass>& pass);

	private:
		// For future
		//void SavePipelines();
		template<typename T>
		void Shutdown(const ME::Core::Containers::Array<ME::Core::Memory::Reference<T>>& objects)
		{
			for (const auto& element : objects)
			{
				--m_TotalObjectCount;
				if (element == nullptr)
					continue;
				element->Shutdown();
			}
		}

	private:
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Shader>> m_Shaders;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<VertexBuffer>> m_VertexBuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<IndexBuffer>> m_IndexBuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Uniform>> m_Uniforms;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<StorageBuffer>> m_StorageBuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<IndirectBuffer>> m_IndirectBuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Texture>> m_Textures;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<CommandBuffer>> m_CommandBuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Framebuffer>> m_Framebuffers;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<RenderPass>> m_RenderPasses;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<Pipeline>> m_Pipelines;

	private:
		std::atomic<uint64> m_TotalObjectCount;

	};
}
