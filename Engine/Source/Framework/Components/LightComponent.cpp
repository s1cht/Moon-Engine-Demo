#include "LightComponent.hpp"
#include "Application/Application.hpp"

namespace ME::Components
{
    LightComponent::~LightComponent()
    {
        uint64 idToUpdate = ~0u;
        switch (m_Type)
        {
            case Render::LightType::Point: idToUpdate = Render::RenderCommand::GetLightManager()->DestroyPointLight(m_LightId); break;
            case Render::LightType::Directional: idToUpdate = Render::RenderCommand::GetLightManager()->DestroyDirectionalLight(m_LightId); break;
            case Render::LightType::Spot: idToUpdate = Render::RenderCommand::GetLightManager()->DestroySpotLight(m_LightId); break;
        }

        if (Application::GetWorld()->Destroyed()) return;

        UpdateOtherComponentsLightIDs(idToUpdate, m_LightId);
    }

    void LightComponent::ChangeType(Render::LightType type)
    {
        if (type == m_Type) return;

        uint64 newTypeLightId = ~0u;
        switch (type)
        {
            case Render::LightType::Point:       newTypeLightId = Render::RenderCommand::GetLightManager()->CreatePointLight(); break;
            case Render::LightType::Directional: newTypeLightId = Render::RenderCommand::GetLightManager()->CreateDirectionalLight(); break;
            case Render::LightType::Spot:        newTypeLightId = Render::RenderCommand::GetLightManager()->CreateSpotLight(); break;
        }

        if (newTypeLightId == ~0u)
        {
            ME_ERROR("Can't change light type: Limit reached for the target type!");
            return;
        }

        uint64 idThatWasMoved = ~0u;
        switch (m_Type)
        {
            case Render::LightType::Point:       idThatWasMoved = Render::RenderCommand::GetLightManager()->DestroyPointLight(m_LightId); break;
            case Render::LightType::Directional: idThatWasMoved = Render::RenderCommand::GetLightManager()->DestroyDirectionalLight(m_LightId); break;
            case Render::LightType::Spot:        idThatWasMoved = Render::RenderCommand::GetLightManager()->DestroySpotLight(m_LightId); break;
        }

        if (idThatWasMoved != ~0u && idThatWasMoved != m_LightId)
        {
            UpdateOtherComponentsLightIDs(idThatWasMoved, m_LightId);
        }
     
        m_Type = type;
        m_LightId = newTypeLightId;
    }

    void LightComponent::UpdateOtherComponentsLightIDs(uint64 oldId, uint64 newId) const
    {
        auto lightEntities = Application::GetWorld()->View<LightComponent>();
        for (auto& ent : lightEntities)
        {
            LightComponent& comp = ent->GetComponent<LightComponent>();
            if (m_Type != comp.m_Type || oldId != comp.GetLightID()) continue;
            comp.SetLightID(newId);
            break;
        }
    }
}