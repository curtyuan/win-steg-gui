#include "apc_function_test.h"
#include "core/extract/execution/apc.h"
#include "utils/file_logger.h"

namespace test {

// Static member initialization
HANDLE APCFunctionTest::hApcThread = NULL;

void APCFunctionTest::log_numbers() {
    utils::FileLogger::log("[APC] 1");
    utils::FileLogger::log("[APC] 2");
    utils::FileLogger::log("[APC] 3");
    utils::FileLogger::log("[APC] 4");
}

bool APCFunctionTest::run_apc_test() {
    // Initialize logger
    if (!utils::FileLogger::init()) {
        return false;
    }

    utils::FileLogger::log("APC Test Started");

    // Queue APC with our custom function
    hApcThread = extract::APCExecutor::queue_apc_thread(log_numbers);
    if (!hApcThread) {
        utils::FileLogger::log("Failed to queue APC thread");
        return false;
    }

    // Resume thread to execute APC
    if (!extract::APCExecutor::resume_apc_thread(hApcThread)) {
        utils::FileLogger::log("Failed to resume APC thread");
        return false;
    }

    return true;
}

} // namespace test 