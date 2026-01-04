#include "DefferedCommon.hlsl"

DefferedVertexOutput VSMain(uint vertexId : SV_VertexID)
{
    const float2 positions[4] =
    {
        float2(-1.f, -1.f), float2(-1.f, 1.f),
	    float2(1.f, -1.f), float2(1.f, 1.f)
    };
    const float2 texPositions[4] =
    {
        float2(0.f, 0.f), float2(0.f, 1.f),
	    float2(1.f, 0.f), float2(1.f, 1.f)
    };

    DefferedVertexOutput result;
    result.Position = float4(positions[vertexId], 0.f, 1.f);
    result.ScreenPosition = texPositions[vertexId];

    return result;
}
