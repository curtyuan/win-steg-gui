#include "core/extract/extract.h"
#include "core/extract/lsb_extract.h"
#include "utils/file_logger.h"
#include <fstream>

namespace extract {

std::unique_ptr<IExtractor> ExtractorFactory::create(Method method) {
    switch (method) {
        case Method::LSB:
            return std::make_unique<LsbExtractor>();
        case Method::JSTEG:
            utils::FileLogger::log("[EXTRACT] JSTEG extractor not implemented yet");
            return nullptr;
        default:
            utils::FileLogger::log("[EXTRACT] Unknown extraction method");
            return nullptr;
    }
}

std::vector<uint8_t> extract_lsb(const std::string& source_path) {
    LsbExtractor extractor;
    return extractor.extract(source_path);
}

} // namespace extract