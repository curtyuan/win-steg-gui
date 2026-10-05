#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include <string>
#include <Windows.h>

namespace utils {

class FileLogger {
private:
    // Static members
    static HANDLE g_mutex;
    static bool g_initialized;
    
    // Helper functions
    static bool init_mutex();
    static void write_to_file(const std::string& formatted_message);
    static std::string get_timestamp();
    
    // Prevent instantiation
    FileLogger() = delete;
    ~FileLogger() = delete;
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

public:
    // Initialize and cleanup - always available
    static bool init();
    static void cleanup();

    // Logging functions - conditionally compiled
#ifdef _DEBUG
    static void log(const std::string& message);
    static void log(const char* message);
#else
    // In release builds, these become no-ops
    static inline void log(const std::string&) {}
    static inline void log(const char*) {}
#endif
};

} // namespace utils

#endif // FILE_LOGGER_H