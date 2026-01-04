#pragma once
#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/Array.hpp>

#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
    constexpr uint64 ME_RENDER_MAX_LIGHT_COUNT = 10000;

    enum class LightType : uint8
    {
        Point = 0, Directional, Spot
    };

    struct Light
    {
    private:
        // Color : 0, 0, 0 and Enabled = true
        uint32 ColorAndEnabled = 0x000000FF;
    public:
        float32 Brightness;

        void SetEnabled(bool value)
        {
            ColorAndEnabled = (ColorAndEnabled & 0xFFFFFF00) | static_cast<uint8>(value);
        }

        void SetColor(ME::Core::Math::Color3 color)
        {
            uint32 r = static_cast<uint32>(color.r) << 24;
            uint32 g = static_cast<uint32>(color.g) << 16;
            uint32 b = static_cast<uint32>(color.b) << 8;

            ColorAndEnabled = (ColorAndEnabled & 0x000000FF) | r | g | b;
        }
    };

    struct alignas(16) DirectionalLight : Light
    {
        uint32 Padding1[2];
        ME::Core::Math::Vector3D Direction;
        uint32 Padding2;
    };

    struct alignas(16) PointLight : Light
    {
        float32 Diameter;
        uint32 Padding1;
        ME::Core::Math::Vector3D Position;
        uint32 Padding2;
    };

    struct alignas(16) SpotLight : Light
    {
        float32 Angle;
        float32 Length;
        ME::Core::Math::Vector3D Position;
        uint32 Padding1;
        ME::Core::Math::Vector3D Direction;
        uint32 Padding2;
    };

    class MEAPI LightManager
    {
    public:
        LightManager();
        /**
         * LightManager constructor
         * @param maxLightCount Max light count per type
         */
        LightManager(uint64 maxLightCount);
        ~LightManager();

    public:
        /**
         * Shutdown method
         */
        void Shutdown();

    public:
        /**
        * Creates a DirectionalLight
        * @return Light id
        */
        uint64 CreateDirectionalLight();
        /**
        * Creates a PointLight
        * @return Light id
        */
        uint64 CreatePointLight();
        /**
         * Creates a SpotLight
         * @return Light id 
         */
        uint64 CreateSpotLight();

        /**
         * Creates a light with given type
         * @param lightType Light type 
         * @return Light id
         */
        uint64 CreateLight(LightType lightType);
        
        /**
        * Destroys the DirectionalLight
        * @param lightId Deletable light ID
        * @return Replaced light ID, where ID must be updated
        */
        uint64 DestroyDirectionalLight(uint64 lightId);
        /**
        * Destroys the PointLight
        * @param lightId Deletable light ID
        * @return Replaced light ID, where ID must be updated
        */
        uint64 DestroyPointLight(uint64 lightId);
        /**
        * Destroys the SpotLight
        * @param lightId Deletable light ID
        * @return Replaced light ID, where ID must be updated
        */
        uint64 DestroySpotLight(uint64 lightId);

        /**
         * Destroys the light
         * @param lightId Deletable light ID
         * @param lightType Light type of light ID
         * @return Replaced light ID, where ID must be updated
         */
        uint64 DestroyLight(uint64 lightId, LightType lightType);

    public:
        /**
         * MaxLightCount getter
         * @return Max light count per light type
         */
        uint64 GetMaxLightCount() const { return m_MaxLightCount; }
        
        /**
         * DirectionalLightCount getter
         * @return DirectionalLight count
         */
        uint64 GetDirectionalLightCount() const { return m_DirectionalLightCount; }
        
        /**
         * DirectionalLightCount getter
         * @return PointLight count
         */
        uint64 GetPointLightCount() const { return m_PointLightCount; }
        
        /**
         * DirectionalLightCount getter
         * @return SpotLight count
         */
        uint64 GetSpotLightCount() const { return m_SpotLightCount; }
        
        /**
        * DirectionalLight getter
        * @param lightId DirectionalLight's ID
        * @return DirectionalLight
        */
        DirectionalLight& GetDirectionalLight(uint64 lightId);

        /**
         * PointLight getter
         * @param lightId PointLight ID
         * @return PointLight
         */
        PointLight& GetPointLight(uint64 lightId);

        /**
        * SpotLight getter
        * @param lightId SpotLight's ID
        * @return SpotLight
        */
        SpotLight& GetSpotLight(uint64 lightId);

        /**
         * Light getter
         * @tparam lightType Light type
         * @param lightId Light id
         * @return Light
         */
        template <typename lightType>
        lightType& GetLight(uint64 lightId);

        /**
         * Lights getter
         * @return Light buffer
         */
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> GetLights() { return m_Lights; }

    public:
        // Buffer methods

        /**
         * Write buffers
         */
        void Write();

        /**
         * Bind the light buffer
         * @param commandBuffer CommandBuffer to write
         * @param pipeline Pipeline to use
         */
        void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<ME::Render::Pipeline> pipeline);

    private:
        void Init();
        void CreateBuffer();

    private:
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> m_Lights;
    
        DirectionalLight* m_DirectionalLightArray;
        PointLight* m_PointLightArray;
        SpotLight* m_SpotLightArray;

        uint64 m_MaxLightCount;
        
        uint64 m_DirectionalLightCount;
        uint64 m_PointLightCount;
        uint64 m_SpotLightCount;
    };

    template <typename lightType>
    lightType& LightManager::GetLight(uint64 lightId)
    {
        static_assert(std::is_base_of_v<Light, lightType>,
            "lightType must be a class derived from Light!");

        if constexpr (std::is_same_v<lightType, DirectionalLight>)
            return GetDirectionalLight(lightId);
        else if constexpr (std::is_same_v<lightType, PointLight>)
            return GetPointLight(lightId);
        else if constexpr (std::is_same_v<lightType, SpotLight>)
            return GetSpotLight(lightId);
        else
            static_assert(false, "Unsupported light type passed to GetLight!");
    }
}
