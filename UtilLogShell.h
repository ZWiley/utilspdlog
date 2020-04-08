#ifndef LOGUTIL_H
#define LOGUTIL_H
#ifndef _CHECK_DIR_
#define _CHECK_DIR_

#include <memory>

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
    int nRet = 0;
    if ((nRet = _access(dir, 0)) == -1)
    {
#ifdef _WIN32
        int flag = _mkdir(dir);
#endif
#ifdef __linux__
        int flag = mkdir(dir.c_str(), 0777);
#endif
        return (flag == 0);
    }
    return true;
};
#endif

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

#ifndef suffix
#define suffix(msg) std::string("[").append(__FILENAME__).append(" ").append(__FUNCTION__)\
        .append("() Line: ").append(std::to_string(__LINE__)).append("] ").append(msg)\
        .append(".").c_str()
#endif

#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#include "spdlog/spdlog.h"


class CLogUtil
{
public:
    CLogUtil();
    ~CLogUtil();
    std::shared_ptr<spdlog::logger> getLogger();
    char szLogFileName[MAX_PATH];

private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#define __LOG__(LVL, msg, ...) do {\
    GlobalLog.getLogger()->LVL(suffix(msg), __VA_ARGS__);\
    } while(0)

#define LOG_RUN(msg, ...) __LOG__(info, msg, __VA_ARGS__)
#define LOG_TRACE(msg, ...) __LOG__(trace, msg, __VA_ARGS__)
#define LOG_DEBUG(msg, ...) __LOG__(debug, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...) __LOG__(warn, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...) __LOG__(error, msg, __VA_ARGS__)
#define LOG_CRITICAL(msg, ...) __LOG__(critical, msg, __VA_ARGS__)

extern CLogUtil GlobalLog;
#endif
