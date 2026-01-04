#pragma once
#include "ECS/Managers/ComponentManager.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Assets/Mesh.hpp"
#include "Renderer/Managers/LightManager.hpp"

namespace ME::Components
{
    struct LightComponent : ECS::Components::Component
    {
        ME_COMPONENT_CONSTRUCTOR(LightComponent), m_Type(ME::Render::LightType::Point)
        {
        }

        ME_COMPONENT_CONSTRUCTOR(LightComponent, Render::LightType lightType), m_Type(lightType)
        {
        }
        
        MEAPI ~LightComponent() override;

        Render::PointLight& PointLight() { if (m_LightId >= ~0u) Init(m_Type); return Render::RenderCommand::GetLightManager()->GetPointLight(m_LightId); }
        Render::SpotLight& SpotLight() { if (m_LightId >= ~0u) Init(m_Type); return Render::RenderCommand::GetLightManager()->GetSpotLight(m_LightId); }
        Render::DirectionalLight& DirectionalLight() { if (m_LightId >= ~0u) Init(m_Type); return Render::RenderCommand::GetLightManager()->GetDirectionalLight(m_LightId); }

        MEAPI void ChangeType(Render::LightType type);

        uint64 GetLightID() const { return m_LightId; }
        Render::LightType GetType() const { return m_Type; }
    
    private:
        void Init(Render::LightType type)
        {
            m_Type = type;
            m_LightId = ~0u;
            switch (m_Type)
            {
                case Render::LightType::Point: m_LightId = Render::RenderCommand::GetLightManager()->CreatePointLight(); break;
                case Render::LightType::Directional: m_LightId = Render::RenderCommand::GetLightManager()->CreateDirectionalLight(); break;
                case Render::LightType::Spot: m_LightId = Render::RenderCommand::GetLightManager()->CreateSpotLight(); break;
            }
            if (m_LightId >= ~0u)
                ME_ERROR("LightComponent can not be created due to the light limit!");
        }

        void SetLightID(uint64 lightId) { m_LightId = lightId; }
        MEAPI void UpdateOtherComponentsLightIDs(uint64 oldId, uint64 newId) const;

    private:
        Render::LightType m_Type;
        uint64 m_LightId;
    };
}
