#pragma once
#include "MathCore.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Tuple.hpp"

namespace ME::Core::Math
{
    struct COREAPI Color3
    {
    public:
        union
        {
            struct { uint8 r, g, b; };
            struct { uint8 R, G, B; };
            uint8 RGB[3];
        };

    public:
        static const Color3 White;
        static const Color3 Black;
        static const Color3 Red;
        static const Color3 Green;
        static const Color3 Blue;

    public:
        Color3();
        Color3(uint8 red, uint8 green, uint8 blue);

        ME::Core::String ToHex() const;
        ME::Core::Tuple<float32, float32, float32> ToHSV() const;
        Color3 Lerp(Color3 target, float32 alpha) const;

    public:
        static Color3 FromHSV(uint16 hue, uint8 saturation, uint8 value);
        static Color3 FromHex(const ME::Core::String& hex);
    };

    struct COREAPI Color4
    {
    public:
        union 
        {
            struct { uint8 r, g, b, a; };
            struct { uint8 R, G, B, A; };
            uint8 RGBA[4];
        };

    public:
        Color4();
        Color4(uint8 r, uint8 g, uint8 b, uint8 a);

        ME::Core::String ToHex() const;
        ME::Core::Tuple<float32, float32, float32> ToHSV() const;
        Color4 Lerp(Color4 target, float32 alpha) const;

    };
}