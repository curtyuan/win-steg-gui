#include "core/extract/jpeg_extract.h"
#include "utils/file_logger.h"
#include <fstream>

namespace extract {

std::vector<uint8_t> JpegExtractor::extract(const std::string& source_path) {
    if (!validate_jpeg(source_path)) {
        utils::FileLogger::log("[JPEG] Invalid JPEG file");
        return {};
    }
    return read_payload(source_path);
}

bool JpegExtractor::validate_jpeg(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    // Check JPEG signature (FF D8)
    uint8_t sig[2];
    if (!file.read(reinterpret_cast<char*>(sig), 2)) return false;
    return sig[0] == 0xFF && sig[1] == 0xD8;
}

std::vector<uint8_t> JpegExtractor::read_payload(const std::string& path) const {
    // TODO: Implement JPEG payload extraction
    utils::FileLogger::log("[JPEG] JPEG extraction not implemented yet");
    return {};
}

} // namespace extract