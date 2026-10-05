/**
 * @file lsb_steg.cpp
 * @brief Implementation of LSB steganography
 * @copyright Copyright (C) 2023
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "core/steg/lsb_steg.h"
#include "utils/file_logger.h"
#include <fstream>

namespace steg {

bool LsbSteganographer::embed(const std::string& target_path,
                            const std::vector<uint8_t>& payload,
                            const std::string& output_path) {
    utils::FileLogger::log("[LSB] Starting embedding...");
    utils::FileLogger::log("[LSB] Target path: " + target_path);
    utils::FileLogger::log("[LSB] Output path: " + output_path);
    
    // Open target file
    std::ifstream file(target_path, std::ios::binary);
    if (!file) {
        utils::FileLogger::log("[LSB] Failed to open target file");
        return false;
    }

    // Get file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size < 32 + payload.size() * 4) {  // Header + payload space needed
        utils::FileLogger::log("[LSB] Target file too small");
        return false;
    }

    // Read file data
    std::vector<uint8_t> data(size);
    if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
        utils::FileLogger::log("[LSB] Failed to read file data");
        return false;
    }

    // Write header with payload size
    if (!write_header(data.data(), size, static_cast<uint32_t>(payload.size()))) {
        utils::FileLogger::log("[LSB] Failed to write header");
        return false;
    }

    // Embed payload
    if (!embed_payload(data.data(), size, payload)) {
        utils::FileLogger::log("[LSB] Failed to embed payload");
        return false;
    }

    // Write output file
    std::ofstream outfile(output_path, std::ios::binary);
    if (!outfile.write(reinterpret_cast<char*>(data.data()), size)) {
        utils::FileLogger::log("[LSB] Failed to write output file");
        return false;
    }

    utils::FileLogger::log("[LSB] Successfully embedded " + std::to_string(payload.size()) + " bytes");
    return true;
}

bool LsbSteganographer::write_header(uint8_t* data, size_t size, uint32_t payload_size) {
    if (size < 32) return false;

    // Write size in first 32 pixels (2 LSB each = 64 bits)
    for (size_t i = 0; i < 32; i++) {
        uint8_t bits = (payload_size >> (i * 2)) & 0x03;
        data[i] = (data[i] & 0xFC) | bits;  // Clear and set 2 LSBs
    }

    return true;
}

bool LsbSteganographer::embed_payload(uint8_t* data, size_t size, const std::vector<uint8_t>& payload) {
    if (size < 32 + payload.size() * 4) return false;

    size_t data_idx = 32;  // Start after header
    for (uint8_t byte : payload) {
        // Each byte needs 4 pixels (2 bits per pixel)
        for (int j = 0; j < 4; j++) {
            if (data_idx >= size) return false;
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            uint8_t current = data[data_idx];
            data[data_idx] = (current & 0xFC) | bits;
            data_idx++;
        }
    }

    return true;
}

} // namespace steg
