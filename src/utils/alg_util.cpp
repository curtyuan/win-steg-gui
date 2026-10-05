#include "utils/alg_util.h"
#include "utils/file_logger.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace utils {

bool compare_bytes(const std::vector<uint8_t>& actual, const std::vector<uint8_t>& expected, const std::string& log_prefix) {
    if (actual.size() != expected.size()) {
        FileLogger::log(log_prefix + " Size mismatch");
        FileLogger::log(log_prefix + " Expected size: " + std::to_string(expected.size()) + " bytes");
        FileLogger::log(log_prefix + " Got size: " + std::to_string(actual.size()) + " bytes");
        FileLogger::log(log_prefix + " " + format_bytes_hex(expected, 16, "Expected bytes: "));
        FileLogger::log(log_prefix + " " + format_bytes_hex(actual, 16, "Got bytes: "));
        return false;
    }
    
    if (actual != expected) {
        FileLogger::log(log_prefix + " Content mismatch");
        size_t mismatch_pos = 0;
        for (; mismatch_pos < actual.size(); ++mismatch_pos) {
            if (actual[mismatch_pos] != expected[mismatch_pos]) break;
        }
        
        size_t context_start = (mismatch_pos > 8) ? mismatch_pos - 8 : 0;
        size_t context_length = std::min<size_t>(16, actual.size() - context_start);
        
        FileLogger::log(log_prefix + " First mismatch at position " + std::to_string(mismatch_pos));
        FileLogger::log(log_prefix + " " + format_bytes_hex(
            std::vector<uint8_t>(expected.begin() + context_start, expected.begin() + context_start + context_length),
            context_length,
            "Expected bytes: "));
        FileLogger::log(log_prefix + " " + format_bytes_hex(
            std::vector<uint8_t>(actual.begin() + context_start, actual.begin() + context_start + context_length),
            context_length,
            "Got bytes: "));
        return false;
    }
    
    return true;
}

std::string format_bytes_hex(const std::vector<uint8_t>& data, size_t max_bytes, const std::string& prefix) {
    std::stringstream ss;
    ss << prefix;
    
    const size_t bytes_to_show = std::min<size_t>(max_bytes, data.size());
    for (size_t i = 0; i < bytes_to_show; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
    }
    
    if (bytes_to_show < data.size()) {
        ss << "...";
    }
    
    return ss.str();
}

} // namespace utils 