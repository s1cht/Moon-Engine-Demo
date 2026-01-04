#include "Color.hpp"

namespace ME::Core::Math
{
    Color3::Color3() : R(0), G(0), B(0)
    {
    }

    Color3::Color3(uint8 red, uint8 green, uint8 blue)
        : R(red), G(green), B(blue)
    {
    }

    ME::Core::String Color3::ToHex() const
    {
        return ME::Core::Utils::ConvertNumberToHex(R) + ME::Core::Utils::ConvertNumberToHex(G) + ME::Core::Utils::ConvertNumberToHex(B);
    }

    ME::Core::Tuple<float32, float32, float32> Color3::ToHSV() const
    {
        return {};
    }

    Color3 Color3::Lerp(Color3 target, float32 alpha) const
    {
        return {};
    }

    Color3 Color3::FromHSV(uint16 hue, uint8 saturation, uint8 value)
    {
        return {};
    }

    Color3 Color3::FromHex(const ME::Core::String& hex)
    {
        return {};
    }

    Color4::Color4()
    {
    }

    Color4::Color4(uint8 r, uint8 g, uint8 b, uint8 a)
    {
    }

    ME::Core::String Color4::ToHex() const
    {
        return {};
    }

    ME::Core::Tuple<float32, float32, float32> Color4::ToHSV() const
    {
        return {};
    }

    Color4 Color4::Lerp(Color4 target, float32 alpha) const
    {
        return Color4();
    }
}