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

Texture2D g_texture;
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
    float4 color = g_texture.Sample(g_sampler, input.uv);
    color = lerp(input.color, color,0.5);
	return color;
}