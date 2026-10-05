#include "core/extract/execution/apc.h"
#include "utils/file_logger.h"

namespace extract {

// Static member initialization
HANDLE APCExecutor::g_hThreadReady = NULL;
HANDLE APCExecutor::g_hThreadExit = NULL;
bool APCExecutor::g_apcExecuted = false;
APCExecutor::APCFunction APCExecutor::g_apcFunction;

HANDLE APCExecutor::queue_apc_thread(APCFunction func) {
    // Store the function to execute
    g_apcFunction = func;
    g_apcExecuted = false;

    // Create synchronization events
    g_hThreadReady = CreateEventA(NULL, TRUE, FALSE, NULL);
    g_hThreadExit = CreateEventA(NULL, TRUE, FALSE, NULL);
    
    if (!g_hThreadReady || !g_hThreadExit) {
        utils::FileLogger::log("[APC] Failed to create events");
        if (g_hThreadReady) CloseHandle(g_hThreadReady);
        if (g_hThreadExit) CloseHandle(g_hThreadExit);
        return NULL;
    }

    // Create thread in suspended state
    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, alertable_thread_proc, NULL, CREATE_SUSPENDED, &threadId);
    if (!hThread) {
        utils::FileLogger::log("[APC] Failed to create thread");
        CloseHandle(g_hThreadReady);
        CloseHandle(g_hThreadExit);
        return NULL;
    }

    // Queue APC
    if (!QueueUserAPC(apc_proc, hThread, reinterpret_cast<ULONG_PTR>(&g_apcFunction))) {
        utils::FileLogger::log("[APC] Failed to queue APC");
        TerminateThread(hThread, 1);
        CloseHandle(hThread);
        CloseHandle(g_hThreadReady);
        CloseHandle(g_hThreadExit);
        return NULL;
    }

    return hThread;
}

bool APCExecutor::resume_apc_thread(HANDLE hThread) {
    if (!hThread) return false;

    // Resume thread
    ResumeThread(hThread);

    // Wait for thread to be ready
    if (WaitForSingleObject(g_hThreadReady, 5000) != WAIT_OBJECT_0) {
        utils::FileLogger::log("[APC] Thread failed to start");
        TerminateThread(hThread, 1);
        CloseHandle(hThread);
        CloseHandle(g_hThreadReady);
        CloseHandle(g_hThreadExit);
        return false;
    }

    // Wait for APC to execute
    Sleep(1000);

    // Signal thread to exit and cleanup
    SetEvent(g_hThreadExit);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(g_hThreadReady);
    CloseHandle(g_hThreadExit);

    return true;
}

DWORD WINAPI APCExecutor::alertable_thread_proc(LPVOID param) {
    // Signal that thread is ready
    SetEvent(g_hThreadReady);
    
    // Log thread started
    utils::FileLogger::log("[APC] Thread Started");
    
    // Enter alertable state loop
    while (WaitForSingleObjectEx(g_hThreadExit, 100, TRUE) == WAIT_TIMEOUT) {
        if (!g_apcExecuted) {
            SleepEx(100, TRUE);  // TRUE makes the wait alertable
        } else {
            Sleep(100);  // Regular sleep if APC has executed
        }
    }
    
    return 0;
}

void CALLBACK APCExecutor::apc_proc(ULONG_PTR param) {
    if (!g_apcExecuted) {
        utils::FileLogger::log("[APC] Executing APC function");
        
        // Execute the stored function
        if (g_apcFunction) {
            g_apcFunction();
            g_apcExecuted = true;
            utils::FileLogger::log("[APC] APC Executed Successfully!");
        }
    }
}

} // namespace extract