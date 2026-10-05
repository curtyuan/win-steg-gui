#ifndef CORE_EXTRACT_EXECUTION_THREAD_HIJACK_H_
#define CORE_EXTRACT_EXECUTION_THREAD_HIJACK_H_

#include <vector>
#include <windows.h>
#include <FL/Fl_Window.H>

namespace extract {

// Public API for thread hijacking
bool execute_thread_hijack(HANDLE hThread, const void* data, size_t size);
HANDLE suspend_thread_load_image(Fl_Window* window);

// Internal functions
BOOL HijackThread(HANDLE hThread, PBYTE pShellcode, SIZE_T sShellcodeSize);
DWORD GetMainThreadId();

} // namespace extract

#endif // CORE_EXTRACT_EXECUTION_THREAD_HIJACK_H_ 