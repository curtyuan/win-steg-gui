#ifndef CREATE_PROCESS_TEST_H
#define CREATE_PROCESS_TEST_H

#include <string>
#include <vector>
#include "core/extract/execution/executor.h"
#include "utils/debug_print.h"

namespace test {

bool test_create_process(const std::string& image_path, const std::string& key_path);
void run_create_process_test();

} // namespace test

#endif // CREATE_PROCESS_TEST_H 