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

PSInput VSMain(float4 position : POSITION, float2 color : COLOR)
{
	PSInput result;

	result.position = position + offset;
    result.color = float4(color,1, 1);

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}