#include "utils/file_logger.h"
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <fstream>
#include <ctime>

namespace utils {

// Static member initialization
HANDLE FileLogger::g_mutex = NULL;
bool FileLogger::g_initialized = false;

bool FileLogger::init_mutex() {
    if (g_mutex == NULL) {
        g_mutex = CreateMutexA(NULL, FALSE, "Global\\LoggerMutex");
        return (g_mutex != NULL);
    }
    return true;
}

void FileLogger::cleanup() {
    if (g_mutex != NULL) {
        CloseHandle(g_mutex);
        g_mutex = NULL;
    }
    g_initialized = false;
}

std::string FileLogger::get_timestamp() {
    std::time_t now = std::time(nullptr);
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return std::string(buffer);
}

bool FileLogger::init() {
#ifdef _DEBUG
    if (!init_mutex()) {
        return false;
    }

    if (!g_initialized) {
        // Wait for mutex
        if (WaitForSingleObject(g_mutex, INFINITE) != WAIT_OBJECT_0) {
            return false;
        }

        try {
            // Create directories if they don't exist
            if (!CreateDirectoryA("assets", NULL)) {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS) {
                    ReleaseMutex(g_mutex);
                    return false;
                }
            }
            
            if (!CreateDirectoryA("assets/log", NULL)) {
                DWORD error = GetLastError();
                if (error != ERROR_ALREADY_EXISTS) {
                    ReleaseMutex(g_mutex);
                    return false;
                }
            }

            // Create or open log file in truncate mode
            std::ofstream file("assets/log/log.txt", std::ios::out | std::ios::trunc);
            if (!file) {
                ReleaseMutex(g_mutex);
                return false;
            }
            
            // Write initial timestamp
            file << "=== Log started at " << get_timestamp() << " ===\n";
            g_initialized = true;
        }
        catch (...) {
            ReleaseMutex(g_mutex);
            return false;
        }

        ReleaseMutex(g_mutex);
    }
#endif
    return true;
}

#ifdef _DEBUG
void FileLogger::log(const std::string& message) {
    if (!g_initialized && !init()) {
        return;
    }

    if (!g_mutex) {
        return;
    }
    
    write_to_file(message + "\n");
}

void FileLogger::log(const char* message) {
    log(std::string(message));
}

void FileLogger::write_to_file(const std::string& formatted_message) {
    DWORD waitResult = WaitForSingleObject(g_mutex, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        return;
    }
    
    try {
        std::ofstream file("assets/log/log.txt", std::ios::app);
        if (file) {
            file << formatted_message;
        }
    }
    catch (...) {
        // Ensure mutex is released even if write fails
    }
    
    ReleaseMutex(g_mutex);
}
#endif

} // namespace utils