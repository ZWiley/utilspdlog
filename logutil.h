#ifndef LOG_UTIL_H
#define LOG_UTIL_H
//  static function check dir
// Check only one layer
#ifndef _CHECK_DIR_
#define _CHECK_DIR_

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#endif
#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

static bool check_dir(const char* dir)
{
	if (_access(dir, 0) == -1)
	{
#ifdef WIN32
		int flag = _mkdir(dir);
#endif
#ifdef linux
		int flag = mkdir(dir.c_str(), 0777);
#endif
		return (flag == 0);
	}
	return true;
};
#endif



//*****************************************************
//注意：
//文件名 __FILE__ ,函数名 __func__ ，行号__LINE__ 是编译器实现的
//并非C++头文件中定义的
//前两个变量是string类型，且__LINE__是整形，所以需要转为string类型
//******************************************************

//整数类型文件行号 ->转换为string类型  std::to_string 处理
#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

//定义一个在日志后添加 文件名 函数名 行号 的宏定义
#ifndef suffix
#define suffix(msg) std::string("[").append(__FILENAME__).append(" ").append(__FUNCTION__)\
        .append("() Line: ").append(std::to_string(__LINE__)).append("] ").append(msg)\
        .append(".").c_str()
#endif

//在  spdlog.h   之前定义，才有效
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

//引入 log 头文件
#include "spdlog/spdlog.h"

// spdlog 封装
#ifndef _MY_LOG_WRAPPER_H_
#define _MY_LOG_WRAPPER_H_

class CLogUtil
{
public:
	CLogUtil()
	{
		check_dir("logs");
		std::vector<spdlog::sink_ptr> sinks;
		//设置为异步日志，同步纯文件日志 3W/s，控制台日志 3k/s
		//spdlog::set_async_mode(32768);  // 必须为 2 的幂
#ifdef _CONSOLE
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#endif
		//sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily.log", 0, 0));
		sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(GetLogFileName(), 0, 0));
		m_logger = std::make_shared<spdlog::logger>("both", begin(sinks), end(sinks));
		spdlog::register_logger(m_logger);

		// 设置日志记录级别
#ifdef _DEBUG
		m_logger->set_level(spdlog::level::trace);
#else
		nml_logger->set_level(spdlog::level::err);
#endif
		//设置 logger 格式
		m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread id: %t] %v");
		//设置当触发err或更严重的错误时立刻刷新日志到磁盘
		m_logger->flush_on(spdlog::level::err);
	};

	inline ~CLogUtil()
	{
		spdlog::drop_all();
	};

	inline std::shared_ptr<spdlog::logger> getLogger() { return m_logger; }

	char* GetLogFileName(){
		static char szLogFileName[MAX_PATH] = { 0x0 };
		strcat_s(szLogFileName, "logs/");
		char szExe[MAX_PATH] = { 0x0 };
		GetModuleFileNameA(NULL, szExe, MAX_PATH);
		const char* pszExeName = strrchr(szExe, '\\');
		// \xxx.exe
		int len = strlen(pszExeName);
		strcat_s(szLogFileName, pszExeName + 1);
		strcat_s(szLogFileName, ".log");
		return szLogFileName;
	}

	//目前只有一个 logger 需要增加交易 logger
	std::shared_ptr<spdlog::logger> m_logger;
};
#endif

//如果没有生成对象，则生成并定义相关宏
//否则标记其为外部对象，直接使用
//可在整个工程里面  每个 cpp 里面包含该头文件
#ifndef _MY_OUTLOG_OBJECT_
#define _MY_OUTLOG_OBJECT_
CLogUtil OutLog;
#define LOG_INFO(msg, ...) OutLog.getLogger()->info(suffix(msg), __VA_ARGS__)
#define LOG_TRACE(msg, ...) OutLog.getLogger()->trace(suffix(msg), __VA_ARGS__)
#define LOG_DEBUG(msg, ...) OutLog.getLogger()->debug(suffix(msg), __VA_ARGS__)
#define LOG_WARN(msg, ...) OutLog.getLogger()->warn(suffix(msg), __VA_ARGS__)
#define LOG_ERROR(msg, ...) OutLog.getLogger()->error(suffix(msg), __VA_ARGS__)
#define LOG_CRITICAL(msg, ...) OutLog.getLogger()->critical(suffix(msg), __VA_ARGS__)
#endif
#endif
