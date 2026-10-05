#include "stealth_execute_test.h"
#include "core/commands/action.h"
#include "utils/file_logger.h"
#include <FL/Fl.H>

namespace test {

bool StealthExecuteTest::run_stealth_test() {
    // Initialize logger
    if (!utils::FileLogger::init()) {
        return false;
    }

    utils::FileLogger::log("Stealth Execute Test Started");

    // Initialize APC with preparation function
    HANDLE hPrepThread = command::Action::apc_init();
    if (!hPrepThread) {
        utils::FileLogger::log("Failed to initialize APC");
        return false;
    }

    // Initialize GUI and set up trigger
    if (!command::Action::gui_init_trigger()) {
        utils::FileLogger::log("Failed to initialize GUI");
        command::Action::cleanup_all();
        return false;
    }

    // Run FLTK event loop
    Fl::run();

    // Cleanup all resources
    command::Action::cleanup_all();
    utils::FileLogger::cleanup();

    return true;
}

} // namespace test
