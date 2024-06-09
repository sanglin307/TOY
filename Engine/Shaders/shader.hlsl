cbuffer SceneConstantBuffer
{
	float4 offset;
	float4 padding[15];
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float3 position : POSITION, float3 color : COLOR)
{
	PSInput result;

    result.position = float4(position, 1) + offset;
    result.color = float4(color, 1);

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}