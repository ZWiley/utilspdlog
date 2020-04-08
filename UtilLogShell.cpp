#include "UtilLogShell.h"

__declspec(noinline) void* GetCurrentAddress(void) {
    return _ReturnAddress();
}
CLogUtil::CLogUtil()
{
    check_dir("logs");

    strcat(szLogFileName, "logs/");
    char szExe[MAX_PATH] = { 0x0 };
    HMODULE dll;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR) GetCurrentAddress(), &dll);
    GetModuleFileNameA(dll, szExe, MAX_PATH);
    const char* pszExeName = strrchr(szExe, '\\');
    int len = strlen(pszExeName);
    strcat(szLogFileName, pszExeName + 1);
    strcat(szLogFileName, ".log");
    m_logger = nullptr;
}

CLogUtil::~CLogUtil(){
    spdlog::drop_all();
}

std::shared_ptr<spdlog::logger> CLogUtil::getLogger() {
    static bool once = [&](){
        m_logger = spdlog::daily_logger_mt("daily_logger", szLogFileName);

#ifndef NDEBUG
        m_logger->set_level(spdlog::level::trace);
#else
        m_logger->set_level(spdlog::level::err);
#endif
        m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread id: %t] %v");
        m_logger->flush_on(spdlog::level::err);
        return true;
    }();
    return m_logger;
}

CLogUtil GlobalLog;
