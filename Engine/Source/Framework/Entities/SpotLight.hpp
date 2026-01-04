#pragma once
#include "ECS/World.hpp"
#include "ECS/Entity.hpp"
#include "Framework/Components/LightComponent.hpp"

namespace ME
{
    class SpotLight : ME_ENTITY_INHERIT()
    {
        ME_ENTITY_DEFAULT(SpotLight)
    public:
        ~SpotLight() override = default;

    public:
        void OnCreated() override
        {
            AddComponent<Components::LightComponent>(Render::LightType::Spot);

            Render::SpotLight& light = Light();
            light.Length = 10.f;
            light.Angle = ME::Core::Math::ToRadians(45.f);
            light.Position = Core::Math::Vector3D::ZeroVector;
            light.Brightness = 1.f;
            light.Direction = ME::Core::Math::Vector3D(1.f, 0.f, 0.f);
            light.Position = ME::Core::Math::Vector3D(0.f, 0.f, 0.f);
            light.SetColor(Core::Math::Color3(255, 255, 255));
            light.SetEnabled(true);
        }

        inline Components::LightComponent& LightComponent() const
        {
            return GetComponent<Components::LightComponent>();
        }

        inline Render::SpotLight& Light() const { return LightComponent().SpotLight(); }

    public:
        inline void SetPosition(const Core::Math::Vector3D & position) const
        {
            Light().Position = position;
        }
    };
}
