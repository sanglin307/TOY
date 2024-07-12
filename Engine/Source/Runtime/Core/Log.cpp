#include "Private.h"

double LogUtil::_TimeDelta = 0;
std::ofstream LogUtil::_Ofstream;

std::mutex LogUtil::_LogMutex;
std::vector<std::string> LogUtil::_LogCache;

void LogUtil::Init()
{
	std::filesystem::path path = PathUtil::Logs();
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}
 
	std::string now = PlatformUtils::DateNowString();
	std::filesystem::path fileName = path / std::format("TOY_{}.txt", now);
	std::filesystem::path original = fileName;
	u32 try_count = 0;
	while (std::filesystem::exists(fileName))
	{
		fileName = path / std::format("TOY_{}_{}.txt", now, try_count);
		try_count++;
	}

	if (try_count > 0)
	{
		std::filesystem::copy_file(original, fileName);
	}

	_Ofstream.open(original, std::ios::out| std::ios::trunc);

}

void LogUtil::Destroy()
{
	std::lock_guard<std::mutex> guard(_LogMutex);
	Flush();
	_Ofstream.close();
}

void LogUtil::Update(double delta)
{
	constexpr static double LOG_UPDATE_DELTA = 1.0; // 1s

	_TimeDelta += delta;

	std::lock_guard<std::mutex> guard(_LogMutex);

	if (!_LogCache.size())
		return;

	if (_TimeDelta > LOG_UPDATE_DELTA)
	{
		_TimeDelta = 0;
		Flush();
	}
}

void LogUtil::Flush()
{
	for (const std::string& str : _LogCache)
	{
		_Ofstream << str;
	}
	_Ofstream.flush();
	_LogCache.clear();
}

void LogUtil::Log(const std::string& category, LogType type, const char* file, u32 line, const std::string& log)
{
	const char* tt = "Log";
	if (type == LogType::Fatal)
		tt = "Fatal";
	else if (type == LogType::Error)
		tt = "Error";

	std::string logString = std::format("[{}] [{}] {} : file: {}, line:{}, {}\n", category, tt, PlatformUtils::TimeNowString(), file,line,log);

	if (type == LogType::Error || type == LogType::Fatal)
	{
		PlatformUtils::OutputDebug(logString);
	}

	std::lock_guard<std::mutex> guard(_LogMutex);
	_LogCache.push_back(std::move(logString));
	if (type == LogType::Fatal)
	{
		Flush();
		abort();
	}
}