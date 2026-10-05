#ifndef APC_FUNCTION_TEST_H
#define APC_FUNCTION_TEST_H

#include <windows.h>

namespace test {

class APCFunctionTest {
public:
    static bool run_apc_test();
    static void log_numbers();

private:
    static HANDLE hApcThread;
};

} // namespace test

#endif // APC_FUNCTION_TEST_H 