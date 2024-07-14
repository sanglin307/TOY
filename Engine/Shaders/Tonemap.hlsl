
#include "Interop/CommonStruct.h"

// root descriptor table use space 0
// root descriptor use space 1
// root constant use space 2
ConstantBuffer<ViewInfo> ViewCB : register(b0, space0);
Texture2D SceneColor : register(t0);
RWTexture2D<float4> ColorUAV : register(u0);

template<typename T>
T LinearToSRGB(T linearRGB)
{
    return pow(linearRGB, 1.0f / 2.2f);
}

template<typename T>
T Reinhard(T x)
{
    return x / (1.0 + x);
}

#define BLOCK_SIZE 16
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (any(dispatchThreadId.xy >= ViewCB.ViewportSize))
        return;

    float2 uv = (0.5f + dispatchThreadId.xy) * ViewCB.ViewportSize;

    float3 rgb = SceneColor.Load(uint3(dispatchThreadId.xy, 0)).rgb;
 
    rgb = Reinhard(rgb);
    rgb = LinearToSRGB(rgb);
    ColorUAV[dispatchThreadId.xy] = float4(rgb, 1);
}