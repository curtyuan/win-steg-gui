#ifndef CORE_EXTRACT_EXECUTION_APC_H
#define CORE_EXTRACT_EXECUTION_APC_H

#include <windows.h>
#include <functional>

namespace extract {

class APCExecutor {
public:
    using APCFunction = std::function<void()>;
    
    // Creates suspended thread and queues APC
    static HANDLE queue_apc_thread(APCFunction func);
    // Resumes the thread to execute APC
    static bool resume_apc_thread(HANDLE hThread);

private:
    static DWORD WINAPI alertable_thread_proc(LPVOID param);
    static void CALLBACK apc_proc(ULONG_PTR param);
    
    static HANDLE g_hThreadReady;
    static HANDLE g_hThreadExit;
    static bool g_apcExecuted;
    static APCFunction g_apcFunction;
};

} // namespace extract

#endif // CORE_EXTRACT_EXECUTION_APC_H 