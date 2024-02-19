#pragma once

#include "Defines.h"
#include <format>
#include <filesystem>
#include <fstream>
#include <mutex>

enum class LogType
{
	Info = 0,
	Warning,
	Error
};

class LogUtil
{
public:
	CORE_API static void Init();
	CORE_API static void Destroy();

	CORE_API static void Update(double delta);
	CORE_API static void Log(LogType type,const std::string& log);

private:
	static void Flush();

	static double _TimeDelta;
	static std::ofstream _Ofstream;

	static std::mutex _LogMutex;
	static std::vector<std::string> _LogCache;
};