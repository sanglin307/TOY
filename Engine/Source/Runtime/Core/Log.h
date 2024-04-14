#pragma once

enum class LogType
{
	Info = 0,
	Error,
	Fatal
};

class LogUtil
{
public:
	CORE_API static void Init();
	CORE_API static void Destroy();

	CORE_API static void Update(double delta);
	CORE_API static void Log(const std::string& category, LogType type, const char* file, u32 line, const std::string& log);

private:
	static void Flush();

	static double _TimeDelta;
	static std::ofstream _Ofstream;

	static std::mutex _LogMutex;
	static std::vector<std::string> _LogCache;
};

#define LOG_INFO(Category,Str)  LogUtil::Log(#Category,LogType::Info, __FILE__,__LINE__, Str)
#define LOG_ERROR(Category,Str)  LogUtil::Log(#Category,LogType::Error,__FILE__,__LINE__,Str)
#define LOG_FATAL(Category,Str) LogUtil::Log(#Category,LogType::Fatal,__FILE__,__LINE__,Str)


#define check(x) (!(x) && (LOG_FATAL(ASSERT,#x),true))