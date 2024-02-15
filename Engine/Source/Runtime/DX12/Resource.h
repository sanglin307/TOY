#pragma once

#include "../Renderer/Resource.h"
#include "Defines.h"

struct DX12FormatInfo
{
	DXGI_FORMAT   DXFormat;
};

class DX12Util
{
public:
	static void Init();
	static DXGI_FORMAT TranslateFormat(GraphicFormat format);
private:
	static std::vector<DX12FormatInfo> _Formats;

};



class DX12SwapChain : public SwapChain
{
	friend class DX12Device;

public:

	virtual ~DX12SwapChain() { _Handle->Release(); _Handle = nullptr; }
	virtual u32 CurrentFrameIndex() override;
private:
	DX12SwapChain(const SwapChain::Config& config, IDXGISwapChain3* handle)
	{
		_Config = config;
		_Handle = handle;
	}

	IDXGISwapChain3* _Handle = nullptr;
};