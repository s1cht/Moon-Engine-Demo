#include "Block.hpp"

namespace ME
{
    inline void Block::SetPosition(Core::Math::Vector3D position)
    {
        Transform().Transform = Core::Math::Transform(position);
    }

    inline void Block::SetSize(Core::Math::Vector3D size)
    {
        Transform().Transform = Core::Math::Transform::FromScale(size);
    }

    inline void Block::SetOrientation(Core::Math::Vector3D orientation)
    {
        Transform().Transform = Core::Math::Transform::FromScale(orientation);
    }
}
