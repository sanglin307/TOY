#pragma once

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
	CORE_API static void Log(const std::string& category, LogType type,const std::string& log);

private:
	static void Flush();

	static double _TimeDelta;
	static std::ofstream _Ofstream;

	static std::mutex _LogMutex;
	static std::vector<std::string> _LogCache;
};

#define TOY_Log(Category,Str)  LogUtil::Log(#Category,LogType::Info,Str)
#define TOY_Warning(Category,Str) LogUtil::Log(#Category,LogType::Warning,Str)
#define TOY_Error(Category,Str)  LogUtil::Log(#Category,LogType::Error,Str)