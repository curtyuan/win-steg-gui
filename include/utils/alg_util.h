#ifndef UTILS_ALG_UTIL_H
#define UTILS_ALG_UTIL_H

#include <string>
#include <vector>
#include <cstdint>

namespace utils {

/**
 * @brief Compare two byte arrays and log differences
 * @param actual The actual data to compare
 * @param expected The expected data to compare against
 * @param log_prefix Prefix for log messages (e.g., "[TEST]")
 * @return true if data matches, false otherwise
 */
bool compare_bytes(
    const std::vector<uint8_t>& actual,
    const std::vector<uint8_t>& expected,
    const std::string& log_prefix = "[COMPARE]");

/**
 * @brief Format bytes as hex string for display
 * @param data The bytes to format
 * @param max_bytes Maximum number of bytes to format (default: 16)
 * @param prefix Prefix to add before the hex string
 * @return Formatted hex string
 */
std::string format_bytes_hex(
    const std::vector<uint8_t>& data,
    size_t max_bytes = 16,
    const std::string& prefix = "");

} // namespace utils

#endif // UTILS_ALG_UTIL_H 