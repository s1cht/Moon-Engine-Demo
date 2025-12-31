#include "RenderCore.hpp"

namespace ME::Render
{
    SIZE_T SizeOfShaderType(ME::Render::ShaderType type)
    {
        switch (type)
        {
        case ME::Render::ShaderType::None:
            return 0;
        case ME::Render::ShaderType::Bool:
            return sizeof(bool);
        case ME::Render::ShaderType::Float:
        case ME::Render::ShaderType::Int:
        case ME::Render::ShaderType::Uint:
            return sizeof(uint32);
        case ME::Render::ShaderType::Float2:
        case ME::Render::ShaderType::Int2:
        case ME::Render::ShaderType::Uint2:
            return sizeof(uint32) * 2;
        case ME::Render::ShaderType::Float3:
        case ME::Render::ShaderType::Int3:
        case ME::Render::ShaderType::Uint3:
            return sizeof(uint32) * 3;
        case ME::Render::ShaderType::Float4:
        case ME::Render::ShaderType::Int4:
        case ME::Render::ShaderType::Uint4:
            return sizeof(uint32) * 4;
        }

        ME_ASSERT(false, "Unknown type");
        return 0;
    }
}
