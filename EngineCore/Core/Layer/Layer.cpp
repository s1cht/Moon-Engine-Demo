#include "Layer.h"

import Pawn.Core.Container.String;

namespace Pawn::Core
{
    Layer::Layer(const Pawn::Core::Containers::String& name)
        : m_DebugName(name)
    {
    }
    Layer::~Layer()
    {
    }
}