#ifndef HIJACK_TEST_H
#define HIJACK_TEST_H

#include <string>
#include <vector>
#include <windows.h>
#include "utils/file_logger.h"

namespace test {

// Dummy function for sacrificial thread
VOID DummyFunction();

// Thread hijacking function
BOOL RunViaThreadHijacking(HANDLE hThread, PBYTE pShellcode, SIZE_T sShellcodeSize);

// Test functions
bool test_thread_hijack();
void run_hijack_test();

} // namespace test

#endif // HIJACK_TEST_H 