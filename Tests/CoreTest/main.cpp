#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Platform/Platform.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Misc/Rect2D.hpp>

constexpr char8 IN_FILE[] = TEXT("coords.txt");
constexpr char8 OUT_FILE[] = TEXT("rez.txt");

using Res = ME::Core::Math::Resolution2D<uint32>;

Res Intersecting(Res a1, Res a2, Res b1, Res b2)
{
    Res result = { 0, 0 };
    bool intersectingX = false;
    bool intersectingY = false;
    if (b1.x < a2.x && a2.x < b2.x)
    {
        result.x = a2.x - b1.x;
        intersectingX = true;
    }
    if (a1.y < b1.y && b1.y < a2.y)
    {
        result.y = b2.y - a1.y;
        intersectingY = true;
    }

    if (!intersectingX || !intersectingY)
        return Res(0, 0);

    return result;
}

uint32 Intersections(
    Res a1, Res a2,
    Res b1, Res b2,
    Res c1, Res c2)
{
    uint32 figureCount = 1;


}

#define READ_DATA(in, val, temp)                            \
in->Read(temp, ME::Core::IO::StringReadMode::Word);         \
val = ME::Core::StringToInt(temp, nullptr);     \

int main() {
    ME::Core::Utils::Logger::Init();

    ME::Core::Memory::Reference<ME::Core::IO::File> in = ME::Core::IO::POpenFile(
        IN_FILE, ME::Core::IO::FileReadMode::Read);

    if (!in->IsOpen())
        return 1;

    Res a1, a2;
    Res b1, b2;
    Res c1, c2;

    ME::Core::String temp;

    READ_DATA(in, a1.x, temp);
    READ_DATA(in, a1.y, temp);
    READ_DATA(in, a2.x, temp);
    READ_DATA(in, a2.y, temp);
    
    READ_DATA(in, b1.x, temp);
    READ_DATA(in, b1.y, temp);
    READ_DATA(in, b2.x, temp);
    READ_DATA(in, b2.y, temp);
    
    READ_DATA(in, c1.x, temp);
    READ_DATA(in, c1.y, temp);
    READ_DATA(in, c2.x, temp);
    READ_DATA(in, c2.y, temp);

        

    ME::Core::Utils::Logger::Shutdown();
    return 0;
}
