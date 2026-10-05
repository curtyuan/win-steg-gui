#ifndef APC_INJECT_TEST_H
#define APC_INJECT_TEST_H

#include <windows.h>
#include "utils/file_logger.h"
#include "core/steg/payload/payload_strategy.h"

namespace test {

class APCInjectTest {
public:
    static bool run_apc_test();
    
private:
    static void CALLBACK apc_routine(ULONG_PTR param);
    static bool create_suspended_process(PROCESS_INFORMATION& pi);
    static bool test_apc_shellcode();
    static PVOID allocate_shellcode_in_process(HANDLE hProcess, const std::vector<uint8_t>& shellcode);
};

} // namespace test

#endif // APC_INJECT_TEST_H 