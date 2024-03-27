#pragma once

#ifdef WINDOWS
#define DX12_API __declspec(dllexport) 
#endif

DX12_API RenderDevice* CreateDX12Device();

