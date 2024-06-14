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
		image->Size = image->Width * image->Height * image->Channel;
		image->Data = (u8*)std::malloc(image->Size);
		std::memcpy(image->Data, imageData, image->Size);
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