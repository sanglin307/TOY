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
    uint FrameIndex;
};