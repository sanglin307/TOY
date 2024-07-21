
#include "Interop/CommonStruct.h"

// root descriptor table use space 0
// root descriptor use space 1
// root constant use space 2
ConstantBuffer<ViewInfo> ViewCB;
ConstantBuffer<MaterialData> MaterialCB;
ConstantBuffer<DrawData> DrawCB;

Texture2D BaseColorTexture;
Texture2D NormalTexture;
Texture2D RoughnessMetalnessTexture;
Texture2D EmissiveTexture;
StructuredBuffer<LightData> LightsBuffer;
StructuredBuffer<PrimitiveData> PrimitiveBuffer;

SamplerState TestSampler;

struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
    float2 UV : TEXCOORD;
    float4 Tangent : TANGENT;
};

struct PSInput
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
    float2 UV : TEXCOORD;
    float4 Tangent : TANGENT;
};

struct MaterialProperties
{
    float3 BaseColor;
    float3 Normal;
    float Metalness;
    float3 Emissive;
    float Roughness;
    float Opacity;
    float Specular;
};

// Unpacks a 2 channel BC5 normal to xyz
float3 UnpackBC5Normal(float2 packedNormal)
{
    return float3(packedNormal, sqrt(1 - saturate(dot(packedNormal.xy, packedNormal.xy))));
}

float3x3 CreateTangentToWorld(float3 normal, float4 tangent)
{
    float3 T = tangent.xyz;
    float3 B = cross(normal, T) * tangent.w;
    float3x3 TBN = float3x3(T, B, normal);
    return TBN;
}

float3 TangentSpaceNormalMapping(float3 sampledNormal, float3x3 TBN)
{
	// Flip Y
	//sampledNormal.y = 1 - sampledNormal.y;

    float3 normal = UnpackBC5Normal(sampledNormal.xy);
    normal.xy = sampledNormal.xy * 2.0f - 1.0f;
    normal = normalize(normal);
    return mul(normal, TBN);
}

MaterialProperties EvaluateMaterial(PSInput input)
{
    MaterialProperties properties;
    float4 baseColor = MaterialCB.BaseColorFactor * float4(input.color, 1);
    if (MaterialCB.TextureMask & BaseColorTextureMask)
    {
        baseColor *= BaseColorTexture.Sample(TestSampler, input.UV);
    }
    
    properties.BaseColor = baseColor.rgb;
    properties.Opacity = baseColor.a;

    properties.Metalness = MaterialCB.MetalnessFactor;
    properties.Roughness = MaterialCB.RoughnessFactor;
    if (MaterialCB.TextureMask & RoughnessMetalnessTextureMask)
    {
        float4 roughnessMetalnessSample = RoughnessMetalnessTexture.Sample(TestSampler, input.UV);
        properties.Metalness *= roughnessMetalnessSample.b;
        properties.Roughness *= roughnessMetalnessSample.g;
    }
    properties.Emissive = MaterialCB.EmissiveFactor.rgb;
    if (MaterialCB.TextureMask & EmissiveTextureMask)
    {
        properties.Emissive *= EmissiveTexture.Sample(TestSampler, input.UV).rgb;
    }
    properties.Specular = 0.5f;

    properties.Normal = normalize(input.normal);
    if (MaterialCB.TextureMask & NormalTextureMask)
    {
        float3 normalTS = NormalTexture.Sample(TestSampler, input.UV).rgb;
        float3x3 TBN = CreateTangentToWorld(properties.Normal, float4(normalize(input.Tangent.xyz), input.Tangent.w));
        properties.Normal = TangentSpaceNormalMapping(normalTS, TBN);
    }
    return properties;
}
 

PSInput VSMain(VertexInput vertex)
{
	PSInput result;
    PrimitiveData pd = PrimitiveBuffer[DrawCB.PrimitiveId];
    result.position = mul(pd.LocalToWorld, float4(vertex.position, 1.0f));
    result.position = mul(ViewCB.ViewProject, float4(result.position));
    result.normal = vertex.normal;
    result.color = vertex.color;
    result.UV = vertex.UV;
    result.Tangent = vertex.Tangent;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    MaterialProperties surface = EvaluateMaterial(input);
    
    float3 color = float3(0, 0, 0);
    for (uint i = 0; i < ViewCB.LightNum;i++)
    {
        LightData l = LightsBuffer[i];
        color.xyz += surface.BaseColor * l.Color;
    }
    
    return float4(color, 1);
}