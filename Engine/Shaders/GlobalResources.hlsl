#pragma once

#include "Interop/CommonStruct.h"

// root descriptor table use space 0
// root descriptor use space 1
// root constant use space 2
ConstantBuffer<ViewInfo> ViewCB : register(b0, space0);
ConstantBuffer<MaterialData> MaterialCB : register(b1, space0);

Texture2D BaseColorTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D RoughnessMetalnessTexture : register(t2);
Texture2D EmissiveTexture : register(t3);

SamplerState TestSampler : register(s0);