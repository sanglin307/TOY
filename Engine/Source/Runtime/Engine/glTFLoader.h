#pragma once

class glTFLoader
{
public:
	static glTFLoader& Instance();
	void Load(std::string_view path, std::vector<Layer*>& newLayers);

private:
	glTFLoader() = default;
	glTFLoader(const glTFLoader& rhs) = delete;
	glTFLoader(glTFLoader&& rhs) = delete;
	glTFLoader& operator=(const glTFLoader& rhs) = delete;
	glTFLoader& operator=(glTFLoader&& rhs) = delete;

};