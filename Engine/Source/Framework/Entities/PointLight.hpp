#pragma once
#include "ECS/World.hpp"
#include "ECS/Entity.hpp"
#include "Framework/Components/LightComponent.hpp"

namespace ME
{
    class PointLight : ME_ENTITY_INHERIT()
    {
        ME_ENTITY_DEFAULT(PointLight)
    public:
        ~PointLight() override = default;

    public:
        void OnCreated() override
        {
            AddComponent<Components::LightComponent>(Render::LightType::Point);

            Render::PointLight& light = Light();
            light.Diameter = 10.f;
            light.Position = Core::Math::Vector3D::ZeroVector;
            light.Brightness = 1.f;
            light.SetColor(Core::Math::Color3(0, 255, 255));
            light.SetEnabled(true);
        }

        inline Components::LightComponent& LightComponent() const
        {
            return GetComponent<Components::LightComponent>();
        }

        inline Render::PointLight& Light() const { return LightComponent().PointLight(); }

    public:
        inline void SetPosition(const Core::Math::Vector3D& position) const
        {
            Light().Position = position;
        }
    };
}
