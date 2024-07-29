#pragma once

SamplerState RootSampler_Point : register(s0, space3);
SamplerState RootSampler_Point_MAGLinear : register(s1, space3);
SamplerState RootSampler_Point_MAGMIPLinear : register(s2, space3);
SamplerState RootSampler_Point_MINLinear : register(s3, space3);
SamplerState RootSampler_Point_MINMAGLinear : register(s4, space3);
SamplerState RootSampler_Linear : register(s5, space3);
SamplerState RootSampler_Anisotropic : register(s6, space3);