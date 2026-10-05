#include "core/extract/lsb_extract.h"
#include "utils/file_logger.h"
#include <fstream>
#include <stdexcept>

namespace extract {

std::vector<uint8_t> LsbExtractor::extract(const std::string& source_path) {
    utils::FileLogger::log("[EXTRACT] Starting extraction from " + source_path);
    
    try {
        gui::BMP image(source_path);
        return extract_from_data(image);
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[EXTRACT] Failed to extract: " + std::string(e.what()));
        throw;
    }
}

std::vector<uint8_t> LsbExtractor::extract_from_data(const gui::BMP& image) {
    uint32_t channelIndex = 0;
    
    // Extract the payload size from first 16 channels (32 bits)
    uint32_t messageSizeInByte = 0;
    for (int i = 0; i < 16; i++) {
        uint8_t twobits = extract_2lsb(image, channelIndex);
        messageSizeInByte |= twobits << (i * 2);
    }
    
    utils::FileLogger::log("[EXTRACT] Extracting " + std::to_string(messageSizeInByte) + " bytes");
    
    // Extract the payload data
    std::vector<uint8_t> extractedData;
    uint32_t messageSizeInBit = messageSizeInByte * 8;
    uint8_t currentByte = 0;
    uint32_t bitsExtracted = 0;
    
    for (uint32_t i = 0; i < messageSizeInBit; i += 2) {
        uint8_t twobits = extract_2lsb(image, channelIndex);
        currentByte |= twobits << bitsExtracted;
        bitsExtracted += 2;
        
        if (bitsExtracted == 8) {
            extractedData.push_back(currentByte);
            currentByte = 0;
            bitsExtracted = 0;
        }
    }
    
    if (bitsExtracted != 0) {
        throw std::runtime_error("Extraction failed: Unexpected remaining bits");
    }
    
    return extractedData;
}

uint8_t LsbExtractor::extract_2lsb(const gui::BMP& image, uint32_t& channelIndex) {
    uint32_t proc_row = (channelIndex / 3) / image.get_width();
    uint32_t proc_col = (channelIndex / 3) % image.get_width();
    uint32_t mod = channelIndex % 3;
    
    if (proc_row >= image.get_height()) {
        throw std::runtime_error("Image too small for payload");
    }
    
    uint32_t pixel = image.get_pixel(proc_col, proc_row);
    uint8_t channelValue;
    
    // Extract the appropriate channel
    switch (mod) {
        case 0: // Blue
            channelValue = pixel & 0xFF;
            break;
        case 1: // Green
            channelValue = (pixel >> 8) & 0xFF;
            break;
        case 2: // Red
            channelValue = (pixel >> 16) & 0xFF;
            break;
        default:
            throw std::invalid_argument("Invalid channel number");
    }
    
    channelIndex++;
    return channelValue & 0x03;
}

bool LsbExtractor::validate_input(const std::vector<uint8_t>& input_data) const {
    // Check minimum BMP header size
    if (input_data.size() < 54) {
        utils::FileLogger::log("[EXTRACT] Input data too small");
        return false;
    }
    
    // Check BMP signature
    if (input_data[0] != 'B' || input_data[1] != 'M') {
        utils::FileLogger::log("[EXTRACT] Invalid BMP signature");
        return false;
    }
    
    // Read image dimensions from header
    uint32_t width, height;
    std::memcpy(&width, &input_data[18], sizeof(width));
    std::memcpy(&height, &input_data[22], sizeof(height));
    
    // Check if image is large enough to contain hidden data
    // Need at least 16 channels (5 pixels + 1 channel) for size field
    return (width * height * 3) >= 16;
}

} // namespace extract