
#include "Interop/CommonStruct.h"

static const float PI = 3.14159265359;
static const float INV_PI = 0.31830988618379067154;

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

SamplerState DefaultSampler;

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Color : COLOR;
    float2 UV : TEXCOORD;
    float4 Tangent : TANGENT;
};

struct PSInput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLDPOS;
    float3 Normal : NORMAL;
    float3 Color : COLOR;
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

struct LightResult
{
    float3 Diffuse;
    float3 Specular;

    LightResult operator+(LightResult rhs)
    {
        LightResult result;
        result.Diffuse = Diffuse + rhs.Diffuse;
        result.Specular = Specular + rhs.Specular;
        return result;
    }
};

struct BrdfData
{
    float3 Diffuse;
    float3 Specular;
    float Roughness;
};

template<
typename T>
T Square(T x)
{
    return x * x;
}

template<
typename T>
T Pow5(T x)
{
    T xx = x * x;
    return xx * xx * x;
}

// 0.08 is a max F0 we define for dielectrics which matches with Crystalware and gems (0.05 - 0.08)
// This means we cannot represent Diamond-like surfaces as they have an F0 of 0.1 - 0.2
float DielectricSpecularToF0(float specular)
{
    return 0.08f * specular;
}

//Note from Filament: vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
// F0 is the base specular reflectance of a surface
// For dielectrics, this is monochromatic commonly between 0.02 (water) and 0.08 (gems) and derived from a separate specular value
// For conductors, this is based on the base color we provided
float3 ComputeF0(float specular, float3 baseColor, float metalness)
{
    return lerp(DielectricSpecularToF0(specular).xxx, baseColor, metalness);
}

float3 ComputeDiffuseColor(float3 baseColor, float metalness)
{
    return baseColor * (1 - metalness);
}

BrdfData GetBrdfData(MaterialProperties material)
{
    BrdfData data;
    data.Diffuse = ComputeDiffuseColor(material.BaseColor, material.Metalness);
    data.Specular = ComputeF0(material.Specular, material.BaseColor, material.Metalness);
    data.Roughness = material.Roughness;
    return data;
}

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
    float4 baseColor = MaterialCB.BaseColorFactor * float4(input.Color, 1);
    if (MaterialCB.TextureMask & BaseColorTextureMask)
    {
        baseColor *= BaseColorTexture.Sample(DefaultSampler, input.UV);
    }
    
    properties.BaseColor = baseColor.rgb;
    properties.Opacity = baseColor.a;

    properties.Metalness = MaterialCB.MetalnessFactor;
    properties.Roughness = MaterialCB.RoughnessFactor;
    if (MaterialCB.TextureMask & RoughnessMetalnessTextureMask)
    {
        float4 roughnessMetalnessSample = RoughnessMetalnessTexture.Sample(DefaultSampler, input.UV);
        properties.Metalness *= roughnessMetalnessSample.b;
        properties.Roughness *= roughnessMetalnessSample.g;
    }
    properties.Emissive = MaterialCB.EmissiveFactor.rgb;
    if (MaterialCB.TextureMask & EmissiveTextureMask)
    {
        properties.Emissive *= EmissiveTexture.Sample(DefaultSampler, input.UV).rgb;
    }
    properties.Specular = 0.5f;

    properties.Normal = normalize(input.Normal);
    if (MaterialCB.TextureMask & NormalTextureMask)
    {
        float3 normalTS = NormalTexture.Sample(DefaultSampler, input.UV).rgb;
        float3x3 TBN = CreateTangentToWorld(properties.Normal, float4(normalize(input.Tangent.xyz), input.Tangent.w));
        properties.Normal = TangentSpaceNormalMapping(normalTS, TBN);
    }
    return properties;
}
 

PSInput VSMain(VertexInput vertex)
{
	PSInput result;
    PrimitiveData pd = PrimitiveBuffer[DrawCB.PrimitiveId];
    result.WorldPosition = mul(pd.LocalToWorld, float4(vertex.Position, 1.0f)).xyz;
    result.Position = mul(ViewCB.ViewProject, float4(result.WorldPosition, 1.0f));
    result.Normal = normalize(mul(pd.LocalToWorld, float4(vertex.Normal, 0)).xyz);
    result.Tangent = float4(normalize(mul(pd.LocalToWorld, float4(vertex.Tangent.xyz, 0)).xyz), vertex.Tangent.w);
    result.Color = vertex.Color;
    result.UV = vertex.UV;

	return result;
}

//Distance between rays is proportional to distance squared
//Extra windowing function to make light radius finite
//https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float RadialAttenuation(float3 L, float range)
{
    float distSq = dot(L, L);
    float distanceAttenuation = 1 / (distSq + 1);
    float windowing = Square(saturate(1 - Square(distSq * Square(rcp(range)))));
    return distanceAttenuation * windowing;
}

// Angle >= Umbra -> 0
// Angle < Penumbra -> 1
//Gradient between Umbra and Penumbra
float DirectionalAttenuation(float3 L, float3 direction, float cosUmbra, float cosPenumbra)
{
    float cosAngle = dot(-normalize(L), direction);
    float falloff = saturate((cosAngle - cosUmbra) / (cosPenumbra - cosUmbra));
    return falloff * falloff;
}

float GetAttenuation(LightData light, float3 worldPosition, out float3 L)
{
    float attenuation = 1.0f;
    L = -light.Direction;
    if (light.Type == LightType_Point || light.Type == LightType_Spot )
    {
        L = light.Position - worldPosition;
        attenuation *= RadialAttenuation(L, light.Range);
        if (light.Type == LightType_Spot)
        {
            attenuation *= DirectionalAttenuation(L, light.Direction, light.SpotlightAngles.y, light.SpotlightAngles.x);
        }

        float distSq = dot(L, L);
        L *= rsqrt(distSq);

    }
 
    return attenuation;
}



/* FRESNEL */

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float3 F_Schlick(float3 f0, float VdotH)
{
    float Fc = Pow5(1.0f - VdotH);
    return Fc + (1.0f - Fc) * f0;
}

float3 F_Schlick(float3 f0, float3 f90, float VdotH)
{
    float Fc = Pow5(1.0f - VdotH);
    return f90 * Fc + (1.0f - Fc) * f0;
}

/* NORMAL DISTRIBUTION FUNCTIONS D */

// GGX / Trowbridge-Reitz
// Note the division by PI here
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float a2, float NdotH)
{
    float d = (NdotH * a2 - NdotH) * NdotH + 1;
    return a2 / (PI * d * d);
}

/* DIFFUSE */

// Diffuse BRDF: Lambertian Diffuse
float3 Diffuse_Lambert(float3 diffuseColor)
{
    return diffuseColor * INV_PI;
}

// Appoximation of joint Smith term for GGX
// Returned value is G2 / (4 * NdotL * NdotV). So predivided by specular BRDF denominator
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox(float a2, float NdotV, float NdotL)
{
    float Vis_SmithV = NdotL * (NdotV * (1 - a2) + a2);
    float Vis_SmithL = NdotV * (NdotL * (1 - a2) + a2);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

LightResult DefaultLitBxDF(float3 specularColor, float specularRoughness, float3 diffuseColor, half3 Normal, half3 V, half3 L, float falloff)
{
    LightResult lighting = (LightResult) 0;
    if (falloff <= 0.0f)
    {
        return lighting;
    }

    float NdotL = saturate(dot(Normal, L));
    if (NdotL == 0.0f)
    {
        return lighting;
    }

    float3 H = normalize(V + L);
    float NdotV = saturate(abs(dot(Normal, V)) + 1e-5); // Bias to avoid artifacting
    float NdotH = saturate(dot(Normal, H));
    float VdotH = saturate(dot(V, H));

	// Generalized microfacet Specular BRDF
    float a = Square(specularRoughness);
    float a2 = clamp(Square(a), 0.0001f, 1.0f);
    float D = D_GGX(a2, NdotH);
    float Vis = Vis_SmithJointApprox(a2, NdotV, NdotL);
    float3 F = F_Schlick(specularColor, VdotH);
    lighting.Specular = (falloff * NdotL) * (D * Vis) * F;

	// Diffuse BRDF
    lighting.Diffuse = (falloff * NdotL) * Diffuse_Lambert(diffuseColor);

    return lighting;
}


LightResult DoLight(LightData light, float3 specularColor, float3 diffuseColor, float Roughness, float3 Normal, float3 V, float3 worldPosition, float linearDepth)
{
    LightResult result = (LightResult) 0;

    float3 L;
    float attenuation = GetAttenuation(light, worldPosition, L);
    if (attenuation <= 0.0f)
        return result;
 
    result = DefaultLitBxDF(specularColor, Roughness, diffuseColor, Normal, V, L, attenuation);

    float3 color = light.Color;
    result.Diffuse *= color * light.Intensity;
    result.Specular *= color * light.Intensity;

    return result;
}

LightResult DoLight(float3 specularColor, float Roughness, float3 diffuseColor, float3 Normal, float3 V, float3 worldPosition, float linearDepth)
{
    LightResult totalResult = (LightResult) 0;
    for (uint lightIndex = 0; lightIndex < ViewCB.LightNum; ++lightIndex)
    {
        LightData light = LightsBuffer[lightIndex];
        totalResult = totalResult + DoLight(light, specularColor, diffuseColor, Roughness, Normal, V, worldPosition, linearDepth);
    }
    return totalResult;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    MaterialProperties surface = EvaluateMaterial(input);
    BrdfData brdfData = GetBrdfData(surface);

    float3 V = normalize(ViewCB.ViewLocation - input.WorldPosition);
 
    LightResult lighting = DoLight(brdfData.Specular, brdfData.Roughness, brdfData.Diffuse, surface.Normal, V, input.WorldPosition, input.Position.w);

    float3 outRadiance = 0;
    outRadiance += Diffuse_Lambert(brdfData.Diffuse);
    outRadiance += lighting.Diffuse + lighting.Specular;
    outRadiance += surface.Emissive;
 
    return float4(outRadiance, surface.Opacity);

}