#include "Log.h"
#include "Platform.h"

std::vector<std::string> LogUtil::_LogCache;
std::ofstream LogUtil::_Ofstream;
double LogUtil::_TimeDelta = 0.0;


void LogUtil::Init()
{
	std::filesystem::path path("./Logs");
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directory(path);
	}

	char buf[32];
	PlatformUtils::TimeNow(std::string_view(buf, 32));
	std::string fileName = std::format("{}/TOY_{}.txt", path.string(), buf);
	while (std::filesystem::exists(fileName))
	{
		static u32 count = 1;
		fileName = std::format("{}/TOY_{}_{}.txt", path.string(), buf, count);
		count++;
	}

	_Ofstream.open(fileName, std::ios::out);

}

void LogUtil::Destroy()
{
	Flush();
	_Ofstream.close();
}

void LogUtil::Update(double delta)
{
	constexpr static const double LOG_UPDATE_DELTA = 1.0; // 1s

	_TimeDelta += delta;

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

	_LogCache.clear();
}

void LogUtil::Log(LogType type, const std::string& log)
{
	char buf[32];
	PlatformUtils::TimeNow(std::string_view(buf, 32));

	const char* tt = "Log";
	if (type == LogType::Warning)
		tt = "Warning";
	else if (type == LogType::Error)
		tt = "Error";

	_LogCache.push_back(std::format("[{}] {} : {}\n", tt, buf, log));
}