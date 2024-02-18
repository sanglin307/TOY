#pragma once

#include "Defines.h"
#include <format>
#include <filesystem>
#include <fstream>

enum class LogType
{
	Info = 0,
	Warning,
	Error
};

class LogUtil
{
public:
	static CORE_API void Init();
	static CORE_API void Destroy();

	static CORE_API void Update(double delta);
	static CORE_API void Log(LogType type,const std::string& log);

private:
	static void Flush();

	static double _TimeDelta;
	static std::ofstream _Ofstream;
	static std::vector<std::string> _LogCache;
};