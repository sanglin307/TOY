cbuffer ViewInfo
{
    float4x4 View;
    float4x4 ViewInverse;
    float4x4 Project;
    float4x4 ProjectInverse;
    float4x4 ViewProject;
    float4x4 ViewProjectInverse;
    float4x4 ViewLocation;
    uint FrameIndex;
};

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

    result.position = mul(ViewProject, float4(position, 1.0f));
    result.normal = normal;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(input.position.xyz,1);
}