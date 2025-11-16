#include "RenderResourcesTracker.hpp"
#include "RenderCommand.hpp"

namespace ME::Render
{
	RenderResourcesTracker::RenderResourcesTracker()
	    :
        //m_Shaders({}),
		//m_VertexBuffers({}),
		//m_IndexBuffers({}),
		//m_Uniforms({}),
		//m_StorageBuffers({}),
		//m_IndirectBuffers({}),
		//m_Textures({}),
		//m_CommandBuffers({}),
		//m_Framebuffers({}),
		//m_Pipelines({}),
	    m_TotalObjectCount(0)
	{
	}

	RenderResourcesTracker::~RenderResourcesTracker()
	{
		if (m_TotalObjectCount > 0)
			ShutdownAll();
	}

	void RenderResourcesTracker::ShutdownAll()
	{
        //Shutdown(m_Pipelines);
        //Shutdown(m_RenderPasses);
        //Shutdown(m_Framebuffers);
        //Shutdown(m_CommandBuffers);
        //Shutdown(m_Textures);
        //Shutdown(m_IndirectBuffers);
        //Shutdown(m_StorageBuffers);
        //Shutdown(m_Uniforms);
        //Shutdown(m_IndexBuffers);
        //Shutdown(m_VertexBuffers);
        //Shutdown(m_Shaders);
	}

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<VertexBuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_VertexBuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<IndexBuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_IndexBuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<Uniform>& buffer)
    {
        ++m_TotalObjectCount;
        //m_Uniforms.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<StorageBuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_StorageBuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<IndirectBuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_IndirectBuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<Texture>& texture)
    {
        ++m_TotalObjectCount;
        //m_Textures.EmplaceBack(texture);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<Framebuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_Framebuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<CommandBuffer>& buffer)
    {
        ++m_TotalObjectCount;
        //m_CommandBuffers.EmplaceBack(buffer);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<Shader>& shader)
    {
        ++m_TotalObjectCount;
        //m_Shaders.EmplaceBack(shader);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<Pipeline>& pipeline)
    {
        ++m_TotalObjectCount;
        //m_Pipelines.EmplaceBack(pipeline);
    }

    void RenderResourcesTracker::AddItem(const ME::Core::Memory::Reference<RenderPass>& pass)
    {
        ++m_TotalObjectCount;
        //m_RenderPasses.EmplaceBack(pass);
    }
}
////