#pragma once

#define CONCAT_IMPL( x, y ) x##y

#ifndef __cplusplus
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define PAD uint MACRO_CONCAT(padding, __COUNTER__);
#else
#define PAD 
#endif


struct ViewInfo
{
    float4x4 View;
    float4x4 ViewInverse;
    float4x4 Project;
    float4x4 ProjectInverse;
    float4x4 ViewProject;
    float4x4 ViewProjectInverse;
    float3   ViewLocation;
    PAD
    uint2 ViewportSize;
    PAD
    PAD
    uint FrameIndex;
    uint LightNum;
};

struct DrawData
{
    uint PrimitiveId;
};

#define BaseColorTextureMask  0x1
#define NormalTextureMask     0x2
#define RoughnessMetalnessTextureMask 0x4
#define EmissiveTextureMask   0x8

struct MaterialData
{
    float4                  BaseColorFactor;
    float4                  EmissiveFactor;
    float                   MetalnessFactor;
    float                   RoughnessFactor;
    float                   AlphaCutoff;
    uint                    TextureMask;
};

#define LightType_Directional 0
#define LightType_Point       1
#define LightType_Spot        2

struct LightData
{
    float3  Position;
    PAD
    float3  Color;
    PAD
    float3  Direction;
    PAD
    float2  SpotlightAngles;
    PAD
    PAD
    uint    Type;
    float   Intensity;
    float   Range;
    PAD
};

struct PrimitiveData
{
    float4x4 LocalToWorld;
};

struct ProceduralSky
{
    float3   SunDirection;
    PAD
    float Rayleigh;
    float Turbidity;
    float MieCoefficient;

    float Luminance;
    float MieDirectionalG;
};