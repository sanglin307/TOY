
#include "Interop/CommonStruct.h"

ConstantBuffer<ViewInfo> ViewCB;

struct PSInput
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

Texture2D texture1;
Texture2D texture2;
SamplerState g_sampler;

PSInput VSMain(float3 position : POSITION, float3 normal :NORMAL)
{
	PSInput result;

    result.position = mul(ViewCB.ViewProject, float4(position, 1.0f));
    result.normal = normal;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 color = (input.normal + 1) / 2;
    //color.z = (ViewCB.FrameIndex % 10) / 10.f;
    return float4(color,1);
}