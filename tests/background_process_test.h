#ifndef BACKGROUND_PROCESS_TEST_H
#define BACKGROUND_PROCESS_TEST_H

#include <string>
#include <vector>
#include <windows.h>
#include "core/extract/execution/executor.h"
#include "utils/debug_print.h"

namespace test {

bool test_background_process(const std::string& image_path, const std::string& key_path);
void run_background_process_test();

} // namespace test

#endif // BACKGROUND_PROCESS_TEST_H 