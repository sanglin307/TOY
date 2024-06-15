#include "Private.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageReader& ImageReader::Instance()
{
	static ImageReader Inst;
	return Inst;
}

void ImageReader::Init()
{

}

void ImageReader::Destroy()
{
	for (auto iter = std::begin(_ImageCache); iter != std::end(_ImageCache); iter++)
	{
		delete iter->second;
	}
}

Image* ImageReader::Load(const std::string& file)
{
	auto iter = _ImageCache.find(file);
	if (iter != _ImageCache.end())
		return iter->second;

	std::filesystem::path imageFilePath = PathUtil::Images() / file;
	if (!std::filesystem::exists(imageFilePath))
	{
		LOG_ERROR(ImageReader, std::format("file {} don't exist!", imageFilePath.string()));
		return nullptr;
	}

	std::ifstream imageFile(imageFilePath, std::ios::in | std::ios::binary | std::ios::ate);
	if (imageFile.is_open())
	{
		std::streampos pos = imageFile.tellg();
		std::streamoff size = pos - std::streampos();
		void* data = std::malloc(size);
		imageFile.seekg(0);
		imageFile.read((char*)data, size);
		imageFile.close();

		Image* image = new Image;
		u8* imageData = stbi_load_from_memory((u8*)data, (int)size, (int*)&(image->Width), (int*)&(image->Height), (int*)&(image->Channel), STBI_default);
		if (image->Channel == 3)
		{
			// convert to 4 
			image->Size = image->Width * image->Height * 4;
			image->Data = (u8*)std::malloc(image->Size);
			u32 readpos = 0;
			for (u32 i = 0; i < image->Size; i+=4,readpos+=3)
			{
				image->Data[i] = imageData[readpos];
				image->Data[i+1] = imageData[readpos+1];
				image->Data[i+2] = imageData[readpos+2];
				image->Data[i+3] = 0;
			}
			image->Format = PixelFormat::R8G8B8A8_UNORM;
		}
		else
		{
			check(image->Channel <= 4);
			image->Size = image->Width * image->Height * image->Channel;
			image->Data = (u8*)std::malloc(image->Size);
			std::memcpy(image->Data, imageData, image->Size);
			if(image->Channel == 1)
				image->Format = PixelFormat::R8_UNORM;
			else if(image->Channel == 2)
				image->Format = PixelFormat::R8G8_UNORM;
			else if(image->Channel == 4)
				image->Format = PixelFormat::R8G8B8A8_UNORM;
		}
	
		stbi_image_free(imageData);
		
		_ImageCache[file] = image;
		return image;
	}
	else
	{
		LOG_ERROR(ImageReader, std::format("file {} open failed!", imageFilePath.string()));
		return nullptr;
	}

	return nullptr;
	
}