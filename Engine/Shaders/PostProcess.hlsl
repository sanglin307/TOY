#include "Common.hlsl"
#include "Interop/CommonStruct.h"

static const float4 FullScreenVertsPos[3] = { float4(-1, 1, 0, 1), float4(3, 1, 0, 1), float4(-1, -3, 0, 1) };
static const float2 FullScreenVertsUVs[3] = { float2(0, 0), float2(2, 0), float2(0, 2) };

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

VertexOutput VSMain(uint vertexId : SV_VertexID)
{
    VertexOutput Output;
    Output.Position = FullScreenVertsPos[vertexId];
    Output.UV = FullScreenVertsUVs[vertexId];
    return Output;
}