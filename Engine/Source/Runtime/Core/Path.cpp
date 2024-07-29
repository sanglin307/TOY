#include "Private.h"

static std::filesystem::path bin_path = std::filesystem::current_path();
static std::filesystem::path log_path = bin_path / "Logs";
static std::filesystem::path config_path = bin_path.parent_path().parent_path() / "Engine" / "Config";
static std::filesystem::path shader_path = bin_path.parent_path().parent_path() / "Engine" / "Shaders";
static std::filesystem::path shader_out_path = bin_path.parent_path().parent_path() / "Engine" / "Shaders" / "Output";
static std::filesystem::path images_path = bin_path.parent_path().parent_path() / "Engine" / "Assets" / "Images";
static std::filesystem::path glTFs_path = bin_path.parent_path().parent_path() / "Engine" / "Assets" / "glTFs";
static std::filesystem::path nv_aftermath_path = bin_path / "NVAftermath";

const std::filesystem::path& PathUtil::Bin()
{
	return bin_path;
}

const std::filesystem::path& PathUtil::NvAftermathOutput()
{
	return nv_aftermath_path;
}

const std::filesystem::path& PathUtil::Logs()
{
	return log_path;
}

const std::filesystem::path& PathUtil::Config()
{
	return config_path;
}

const std::filesystem::path& PathUtil::Shaders()
{
	return shader_path;
}

const std::filesystem::path& PathUtil::ShaderOutput()
{
	return shader_out_path;
}

const std::filesystem::path& PathUtil::Images()
{
	return images_path;
}

const std::filesystem::path& PathUtil::glTFs()
{
	return glTFs_path;
}