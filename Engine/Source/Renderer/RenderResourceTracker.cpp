#include "RenderResourceTracker.hpp"
#include "RenderCommand.hpp"

namespace ME::Render
{
    RenderResourceTracker::RenderResourceTracker()
        : m_Phase1Objects({}), m_Phase2Objects({}), m_Phase3Objects({}), m_TotalObjectCount(0) {}

    RenderResourceTracker::~RenderResourceTracker()
    {
        if (m_TotalObjectCount > 0) ShutdownAll();
    }

    inline void RenderResourceTracker::Add(const ME::Core::Memory::Reference<RenderObject>& object)
    {
        switch (object->GetType())
        {
            // Phase 3 objects
        case RenderObjectType::RenderPass:
        {
            m_Phase3Objects.EmplaceBack(object);
            break;
        }

        // Phase 2 objects
        case RenderObjectType::Framebuffer:
        case RenderObjectType::Shader:
        {
            m_Phase2Objects.EmplaceBack(object);
            break;
        }
        // Phase 1 objects
        case RenderObjectType::Pipeline:
        case RenderObjectType::CommandBuffer:
        case RenderObjectType::Uniform:
        case RenderObjectType::StorageBuffer:
        case RenderObjectType::VertexBuffer:
        case RenderObjectType::IndexBuffer:
        case RenderObjectType::IndirectBuffer:
        case RenderObjectType::Texture1D:
        case RenderObjectType::Texture2D:
        case RenderObjectType::Texture3D:
        {
            m_Phase1Objects.EmplaceBack(object);
            break;
        }
        default:
            break;
        }
    }

    void RenderResourceTracker::ShutdownAll()
    {
        for (auto& object : m_Phase1Objects)
        {
            object->Shutdown();
            --m_TotalObjectCount;
        }
        for (auto& object : m_Phase2Objects)
        {
            object->Shutdown();
            --m_TotalObjectCount;
        }
        for (auto& object : m_Phase3Objects)
        {
            object->Shutdown();
            --m_TotalObjectCount;
        }
    }
}
