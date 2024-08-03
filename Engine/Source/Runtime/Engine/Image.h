#pragma once

struct Image
{
	~Image()
	{
		if (!Data)
			std::free(Data);
	}
	std::string Name;
	u32 Width;
	u32 Height;
	u32 Channel;
	u8* Data = nullptr;
	u32 Size;
	PixelFormat Format;
	ImageCompressType CompressType = ImageCompressType::None;
};

class ImageReader
{
public:
	ENGINE_API static ImageReader& Instance();
	ENGINE_API Image* LoadFromFile(const std::string& fileName);
	ENGINE_API Image* LoadFromMemory(u8* data, u64 size);
	void Init();
	void Destroy();

private:
	ImageReader() = default;
	ImageReader(const ImageReader& rhs) = delete;
	ImageReader(ImageReader&& rhs) = delete;
	ImageReader& operator=(const ImageReader& rhs) = delete;
	ImageReader& operator=(ImageReader&& rhs) = delete;

private:
	std::unordered_map<std::string, Image*> _ImageFileCache;
	std::unordered_map<u64, Image*> _ImageDataCache;
};
