#include <Core.hpp>
#include <Core/Math/Math.hpp>

using namespace ME;
using namespace Core;

int main()
{
    Utility::Logger::Init();

    auto q = Math::Quaternion::FromEulerAnglesYXZ(0, Math::ToRadians(90.f), 0);
    ME_INFO("Rotated (0,0,1): {}", q.RotateVector({ 0, 0, 1 }));

    Utility::Logger::Shutdown();
}