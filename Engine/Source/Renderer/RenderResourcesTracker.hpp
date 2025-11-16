#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>

#include "Core/Containers/Map.hpp"
#include "Renderer/Base/Buffer.hpp"
#include "Renderer/Base/Framebuffer.hpp"
#include "Renderer/Base/CommandBuffer.hpp"
#include "Renderer/Base/Texture.hpp"
#include "Renderer/Base/RenderPass.hpp"
#include "Renderer/Base/Shader.hpp"
#include "Renderer/Base/Pipeline.hpp"

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
		void Shutdown(const ME::Core::Array<ME::Core::Memory::Reference<T>>& objects)
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
		std::atomic<uint64> m_TotalObjectCount;
	};
}
