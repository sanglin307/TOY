#pragma once

class DX12Util
{
public:
	static void Init();
	static DXGI_FORMAT TranslateFormat(PixelFormat format);
	static D3D12_COMMAND_LIST_TYPE TranslateCommandType(const CommandType t);

private:
	static std::vector<DX12FormatInfo> _Formats;

};
