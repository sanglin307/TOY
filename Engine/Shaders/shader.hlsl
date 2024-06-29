cbuffer SceneConstantBuffer
{
	float4 offset;
	float4 padding[15];
};

struct PSInput
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
	float4 color : COLOR;
};

Texture2D texture1;
Texture2D texture2;
SamplerState g_sampler;

PSInput VSMain(float3 position : POSITION, float2 uv :TEXCOORD, float3 color : COLOR)
{
	PSInput result;

    result.position = float4(position, 1) + offset;
    result.color = float4(color, 1);
    result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 c1 = texture1.Sample(g_sampler, input.uv);
    float4 c2 = texture2.Sample(g_sampler, input.uv);
    return lerp(c1, c2, 0.8);
}