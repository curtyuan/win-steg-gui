#include "utils/debug_print.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <sstream>

namespace debug {

void DebugPrint(const std::string& message) {
    #ifdef _DEBUG
    std::cout << "[DEBUG] " << message << std::endl;
    std::cout.flush();
    OutputDebugStringA(("[DEBUG] " + message + "\n").c_str());
    #endif
}

std::string normalizePath(const std::string& path) {
    std::string normalizedPath = path;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '\\', '/');
    return normalizedPath;
}

std::vector<std::string> listFiles(const std::string& directory) {
    std::vector<std::string> files;
    std::string normalizedDirectory = normalizePath(directory);
    std::string path = normalizedDirectory + "/*";

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "[listFiles()] Error: Failed to list files in directory: " << directory << "\n";
        return files;
    }
    
    do {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            files.push_back(findData.cFileName);
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    return files;
}

std::string getExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    return exePath.substr(0, exePath.find_last_of("\\/"));
}

std::string resolveResourcePath(const std::string& relativePath) {
    // First try relative to working directory
    DWORD attrib = GetFileAttributesA(relativePath.c_str());
    if (attrib != INVALID_FILE_ATTRIBUTES) {
        return relativePath;
    }

    // If not found, try relative to executable location
    std::string exePath = getExecutablePath();
    std::string fullPath = exePath + "/" + relativePath;
    return normalizePath(fullPath);
}

void debugListFiles(const std::string& directory) {
    std::string resolvedPath = resolveResourcePath(directory);
    std::cout << "[debugListFiles()] Trying directory: " << resolvedPath << std::endl;
    
    std::vector<std::string> files = listFiles(resolvedPath);

    if (files.empty()) {
        std::cout << "[debugListFiles()] No files found or unable to read directory." << std::endl;
    } else {
        for (const auto& file : files) {
            std::cout << file << std::endl;
        }
        std::cout << "[debugListFiles()] Total " << files.size() << " files found." << std::endl;
    }
}

void print_bytes_info(const std::string& label, const std::vector<uint8_t>& data) {
    std::cout << label << " size: " << data.size() << " bytes" << std::endl;
    std::cout << "First 16 bytes: ";
    const size_t max_display = 16;
    const size_t display_size = (data.size() < max_display) ? data.size() : max_display;
    
    for (size_t i = 0; i < display_size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

} // namespace debug