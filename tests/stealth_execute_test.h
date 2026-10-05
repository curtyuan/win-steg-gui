#ifndef STEALTH_EXECUTE_TEST_H
#define STEALTH_EXECUTE_TEST_H

#include <windows.h>

namespace test {

class StealthExecuteTest {
public:
    static bool run_stealth_test();

private:
    static HANDLE hPrepThread;  // Handle to preparation thread
};

} // namespace test

#endif // STEALTH_EXECUTE_TEST_H
