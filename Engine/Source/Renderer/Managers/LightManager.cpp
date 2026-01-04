#include "LightManager.hpp"

#include "Renderer/RenderCommand.hpp"

static ME::Render::DirectionalLight s_DummyDirectional = {};
static ME::Render::PointLight s_DummyPoint = {};
static ME::Render::SpotLight s_DummySpot = {};

namespace ME::Render
{
    LightManager::LightManager()
        : m_Lights(nullptr), m_MaxLightCount(ME_RENDER_MAX_LIGHT_COUNT), 
    m_DirectionalLightCount(0), m_PointLightCount(0), m_SpotLightCount(0)
    {
        Init();
    }

    LightManager::LightManager(uint64 maxLightCount)
        : m_Lights(nullptr), m_MaxLightCount(maxLightCount),
    m_DirectionalLightCount(0), m_PointLightCount(0), m_SpotLightCount(0)
    {
        Init();
    }

    LightManager::~LightManager()
    {   
        Shutdown();
    }

    DirectionalLight& LightManager::GetDirectionalLight(uint64 lightId)
    {
        if (lightId >= m_DirectionalLightCount)
        {
            ME_ERROR("Requested directional light with id {0}, does not exist!", lightId);
            return s_DummyDirectional;
        }
        return m_DirectionalLightArray[lightId];
    }

    PointLight& LightManager::GetPointLight(uint64 lightId)
    {
        if (lightId >= m_PointLightCount)
        {
            ME_ERROR("Requested point light with id {0}, does not exist!", lightId);
            return s_DummyPoint;
        }
        return m_PointLightArray[lightId];
    }

    SpotLight& LightManager::GetSpotLight(uint64 lightId)
    {
        if (lightId >= m_SpotLightCount)
        {
            ME_ERROR("Requested spot light with id {0}, does not exist!", lightId);
            return s_DummySpot;
        }
        return m_SpotLightArray[lightId];
    }

    void LightManager::Write()
    {
        m_Lights->Write(m_MaxLightCount * sizeof(DirectionalLight), 0, 0);
        m_Lights->Write(m_MaxLightCount * sizeof(PointLight), m_MaxLightCount * sizeof(DirectionalLight), 1);
        m_Lights->Write(m_MaxLightCount * sizeof(SpotLight), m_MaxLightCount * sizeof(DirectionalLight) + m_MaxLightCount * sizeof(PointLight), 2);
    }

    void LightManager::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer,
        ME::Core::Memory::Reference<ME::Render::Pipeline> pipeline)
    {
        m_Lights->Bind(commandBuffer, pipeline);
    }

    void LightManager::Init()
    {
        CreateBuffer();
    }

    void LightManager::Shutdown()
    {
        if (m_Lights != nullptr)
        {
            m_Lights->Shutdown();
            m_Lights = nullptr;
        }
    }

    uint64 LightManager::CreateDirectionalLight()
    {
        if (m_DirectionalLightCount >= m_MaxLightCount)
        {
            ME_ERROR("Max Directional Lights reached!");
            return ~0u;
        }
        m_DirectionalLightArray[m_DirectionalLightCount] = DirectionalLight{};
        return m_DirectionalLightCount++;
    }

    uint64 LightManager::CreatePointLight()
    {
        if (m_PointLightCount >= m_MaxLightCount)
        {
            ME_ERROR("Max Point Lights reached!");
            return ~0u;
        }
        m_PointLightArray[m_PointLightCount] = PointLight{};
        return m_PointLightCount++;
    }

    uint64 LightManager::CreateSpotLight()
    {
        if (m_SpotLightCount >= m_MaxLightCount)
        {
            ME_ERROR("Max Spot Lights reached!");
            return ~0u;
        }
        m_SpotLightArray[m_SpotLightCount] = SpotLight{};
        return m_SpotLightCount++;
    }

    uint64 LightManager::CreateLight(LightType lightType)
    {
        switch (lightType)
        {
        case LightType::Point:
            return CreatePointLight();
        case LightType::Directional:
            return CreateDirectionalLight();
        case LightType::Spot:
            return CreateSpotLight();
        }
        return ~0u;
    }

    uint64 LightManager::DestroyDirectionalLight(uint64 lightId)
    {
        if (lightId >= m_DirectionalLightCount) return ~0u;
        uint64 lastItemId = m_DirectionalLightCount - 1;
        m_DirectionalLightArray[lightId] = std::move(m_DirectionalLightArray[lastItemId]);
        --m_DirectionalLightCount;
        return lastItemId;
    }

    uint64 LightManager::DestroyPointLight(uint64 lightId)
    {
        if (lightId >= m_PointLightCount) return ~0u;
        uint64 lastItemId = m_PointLightCount - 1;
        m_PointLightArray[lightId] = std::move(m_PointLightArray[lastItemId]);
        --m_PointLightCount;
        return lastItemId;
    }

    uint64 LightManager::DestroySpotLight(uint64 lightId)
    {
        if (lightId >= m_SpotLightCount) return ~0u;
        uint64 lastItemId = m_SpotLightCount - 1;
        m_SpotLightArray[lightId] = std::move(m_SpotLightArray[lastItemId]);
        --m_SpotLightCount;
        return lastItemId;
    }

    uint64 LightManager::DestroyLight(uint64 lightId, LightType lightType)
    {
        switch (lightType) 
        {
            case LightType::Point:
                return DestroyPointLight(lightId);
            case LightType::Directional:
                return DestroyDirectionalLight(lightId);
            case LightType::Spot:
                return DestroySpotLight(lightId);
        }
        return ~0u;
    }

    void LightManager::CreateBuffer()
    {
        Render::ResourceBinding binding = {};
        binding.Stage = Render::ShaderStage::Vertex |
            Render::ShaderStage::Mesh |
            Render::ShaderStage::Task |
            Render::ShaderStage::Compute |
            Render::ShaderStage::Domain |
            Render::ShaderStage::Hull | 
            Render::ShaderStage::Pixel;
        binding.Type = Render::ResourceType::StorageBuffer;

        ResourceLayout layout;
        layout.PushBack(binding);
        layout.PushBack(binding);
        layout.PushBack(binding);

        StorageBufferSpecification specification = {};
        specification.MemoryType = MemoryType::RAM;
        specification.Size =    m_MaxLightCount * sizeof(DirectionalLight) +
                                m_MaxLightCount * sizeof(PointLight) + 
                                m_MaxLightCount * sizeof(SpotLight);
        specification.Set = 0;
        specification.Binding = 0;
        specification.SetIndex = ME::Render::RenderCommand::GetResourceHandler()->CreateResourceSet(layout);
        specification.ResourceBinding = binding;
        specification.DebugName = "Light buffer";
    
        m_Lights = ME::Render::StorageBuffer::Create(specification);

        SIZE_T dirLightSize = m_MaxLightCount * sizeof(DirectionalLight);
        SIZE_T pointLightSize = m_MaxLightCount * sizeof(PointLight);

        // Because data is on RAM, we don't have to Unmap the memory
        MappedBufferData data = m_Lights->Map();
        
        uint8_t* bytePtr = static_cast<uint8*>(data.Data);
        m_DirectionalLightArray = reinterpret_cast<DirectionalLight*>(bytePtr);
        m_PointLightArray = reinterpret_cast<PointLight*>(bytePtr + dirLightSize);
        m_SpotLightArray = reinterpret_cast<SpotLight*>(bytePtr + dirLightSize + pointLightSize);
    }
}
