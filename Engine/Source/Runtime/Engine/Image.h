#pragma once


struct Image
{
	~Image()
	{
		if (!Data)
			std::free(Data);
	}
	u32 Width;
	u32 Height;
	u32 Channel;
	u8* Data = nullptr;
	u32 Size;
	PixelFormat Format;
};

class ImageReader
{
public:
	ENGINE_API static ImageReader& Instance();
	ENGINE_API Image* Load(const std::string& fileName);
	void Init();
	void Destroy();

private:
	ImageReader() = default;
	ImageReader(const ImageReader& rhs) = delete;
	ImageReader(ImageReader&& rhs) = delete;
	ImageReader& operator=(const ImageReader& rhs) = delete;
	ImageReader& operator=(ImageReader&& rhs) = delete;

private:
	std::unordered_map<std::string, Image*> _ImageCache;
};
