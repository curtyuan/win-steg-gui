#include <windows.h>
#include "../tests/stealth_execute_test.h"
#include "utils/file_logger.h"

int main() {
    utils::FileLogger::init();
    test::StealthExecuteTest::run_stealth_test();
    utils::FileLogger::cleanup();
    return 0;
}
