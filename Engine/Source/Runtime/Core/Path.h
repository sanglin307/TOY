#pragma once

#include "Defines.h"
#include "Types.h"
#include "filesystem"


class CORE_API PathUtil
{
public:
	static const std::filesystem::path& Bin();
	static const std::filesystem::path& Logs();
	static const std::filesystem::path& Shaders();
};