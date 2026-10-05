#include "core/steg/steg.h"
#include "core/steg/lsb_steg.h"
#include "utils/file_logger.h"

namespace steg {

std::unique_ptr<ISteganographer> SteganographerFactory::create(Method method) {
    switch (method) {
        case Method::LSB:
            return std::make_unique<LsbSteganographer>();
        case Method::JSTEG:
            utils::FileLogger::log("[STEG] JSTEG steganographer not implemented yet");
            return nullptr;
        default:
            utils::FileLogger::log("[STEG] Unknown steganography method");
            return nullptr;
    }
}

bool embed_lsb(const std::string& target_path, 
               const std::vector<uint8_t>& payload,
               const std::string& output_path) {
    auto steg = SteganographerFactory::create(SteganographerFactory::Method::LSB);
    if (!steg) {
        utils::FileLogger::log("[STEG] Failed to create LSB steganographer");
        return false;
    }
    return steg->embed(target_path, payload, output_path);
}

} // namespace steg 