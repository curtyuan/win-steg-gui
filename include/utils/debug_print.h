#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H

#include <string>
#include <vector>
#include <cstdint>

namespace debug {

void DebugPrint(const std::string& message);
void print_bytes_info(const std::string& label, const std::vector<uint8_t>& data);

std::string normalizePath(const std::string& path);
std::vector<std::string> listFiles(const std::string& directory);
std::string getExecutablePath();
std::string resolveResourcePath(const std::string& relativePath);
void debugListFiles(const std::string& directory);

} // namespace debug

#endif // DEBUG_PRINT_H