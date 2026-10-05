#include "core/steg/jpeg_steg.h"
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace steg {

constexpr char JpegSteg::MAGIC[];

bool JpegSteg::hide(const std::string& input_jpeg,
                    const std::string& output_jpeg,
                    const std::vector<uint8_t>& data) {
    // Check input parameters
    if (data.empty()) {
        std::cerr << "Empty payload data" << std::endl;
        return false;
    }

    if (input_jpeg == output_jpeg) {
        std::cerr << "Input and output files must be different" << std::endl;
        return false;
    }

    // Check if output directory is writable
    {
        std::ofstream test(output_jpeg);
        if (!test) {
            std::cerr << "Cannot write to output file: " << output_jpeg << std::endl;
            return false;
        }
    }

    if (!validateJpeg(input_jpeg)) {
        std::cerr << "Invalid JPEG file: " << input_jpeg << std::endl;
        return false;
    }

    try {
        auto payload = preparePayload(data);
        // JPEG::Image jpeg;
        // jpeg.load(input_jpeg);

        // Get DCT coefficients
        // auto& coeffs = jpeg.get_coefficients();
        // if (coeffs.empty()) {
        //     std::cerr << "No DCT coefficients found" << std::endl;
        //     return false;
        // }

        // Calculate capacity and check if payload fits
        size_t capacity = 0;
        // for (const auto& block : coeffs) {
        //     for (const auto& coeff : block) {
        //         if (std::abs(coeff) > 1) { // Only use coefficients > |1|
        //             capacity++;
        //         }
        //     }
        // }

        // if (capacity * 8 < payload.size()) {
        //     std::cerr << "Image too small for payload" << std::endl;
        //     return false;
        // }

        // Embed payload bits into LSB of DCT coefficients
        size_t bit_index = 0;
        // for (auto& block : coeffs) {
        //     for (auto& coeff : block) {
        //         if (std::abs(coeff) > 1) {
        //             if (bit_index / 8 >= payload.size()) {
        //                 break;
        //             }
        //             
        //             // Get current bit from payload
        //             uint8_t bit = (payload[bit_index / 8] >> (bit_index % 8)) & 1;
        //             
        //             // Modify coefficient's LSB
        //             if (coeff > 0) {
        //                 coeff = (coeff & ~1) | bit;
        //             } else {
        //                 coeff = (coeff & ~1) | (1 - bit);
        //             }
        //             
        //             bit_index++;
        //         }
        //     }
        // }

        // Update coefficients and save
        // jpeg.set_coefficients(coeffs);
        // jpeg.save(output_jpeg);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error hiding data: " << e.what() << std::endl;
        return false;
    }
}

std::vector<uint8_t> JpegSteg::extract(const std::string& jpeg_path) {
    // Add file size check
    {
        std::ifstream file(jpeg_path, std::ios::binary | std::ios::ate);
        if (!file) {
            std::cerr << "Cannot open file: " << jpeg_path << std::endl;
            return {};
        }
        auto size = file.tellg();
        if (size < HEADER_SIZE) {
            std::cerr << "File too small to contain hidden data" << std::endl;
            return {};
        }
    }

    if (!validateJpeg(jpeg_path)) {
        std::cerr << "Invalid JPEG file: " << jpeg_path << std::endl;
        return {};
    }

    try {
        // JPEG::Image jpeg;
        // jpeg.load(jpeg_path);

        // Get DCT coefficients
        // const auto& coeffs = jpeg.get_coefficients();
        // if (coeffs.empty()) {
        //     std::cerr << "No DCT coefficients found" << std::endl;
        //     return {};
        // }

        // First pass: extract enough bits for header
        std::vector<uint8_t> header(HEADER_SIZE);
        size_t bit_index = 0;
        size_t byte_index = 0;

        // Extract header
        // for (const auto& block : coeffs) {
        //     for (const auto& coeff : block) {
        //         if (std::abs(coeff) > 1) {
        //             if (byte_index >= HEADER_SIZE) break;
        //             
        //             // Extract bit from coefficient
        //             uint8_t bit = (coeff & 1);
        //             if (coeff < 0) bit = 1 - bit;
        //             
        //             header[byte_index] |= bit << (bit_index % 8);
        //             bit_index++;
        //             
        //             if (bit_index % 8 == 0) {
        //                 byte_index++;
        //             }
        //         }
        //     }
        //     if (byte_index >= HEADER_SIZE) break;
        // }

        // Validate magic header
        // if (std::memcmp(header.data(), MAGIC, 5) != 0) {
        //     std::cerr << "Invalid magic header" << std::endl;
        //     return {};
        // }

        // Get payload size
        uint32_t payload_size = 
            (header[8] << 24) | (header[7] << 16) | 
            (header[6] << 8) | header[5];

        // Extract payload
        std::vector<uint8_t> payload(payload_size);
        bit_index = 0;
        byte_index = 0;

        // for (const auto& block : coeffs) {
        //     for (const auto& coeff : block) {
        //         if (std::abs(coeff) > 1) {
        //             if (byte_index >= payload_size) break;
        //             
        //             uint8_t bit = (coeff & 1);
        //             if (coeff < 0) bit = 1 - bit;
        //             
        //             payload[byte_index] |= bit << (bit_index % 8);
        //             bit_index++;
        //             
        //             if (bit_index % 8 == 0) {
        //                 byte_index++;
        //             }
        //         }
        //     }
        //     if (byte_index >= payload_size) break;
        // }

        return payload;
    }
    catch (const std::exception& e) {
        std::cerr << "Error extracting data: " << e.what() << std::endl;
        return {};
    }
}

size_t JpegSteg::maxPayloadSize(const std::string& jpeg_path) {
    if (!validateJpeg(jpeg_path)) {
        std::cerr << "Invalid JPEG file: " << jpeg_path << std::endl;
        return 0;
    }

    try {
        // JPEG::Image jpeg;
        // jpeg.load(jpeg_path);

        // Get DCT coefficients
        // const auto& coeffs = jpeg.get_coefficients();
        // if (coeffs.empty()) {
        //     std::cerr << "No DCT coefficients found" << std::endl;
        //     return 0;
        // }

        // Count coefficients with magnitude > 1
        size_t usable_coeffs = 0;
        // for (const auto& block : coeffs) {
        //     for (const auto& coeff : block) {
        //         if (std::abs(coeff) > 1) {
        //             usable_coeffs++;
        //         }
        //     }
        // }

        // Each coefficient can store 1 bit, so divide by 8 to get bytes
        // Subtract header size to get actual payload capacity
        return usable_coeffs / 8 > HEADER_SIZE ? 
               usable_coeffs / 8 - HEADER_SIZE : 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error calculating capacity: " << e.what() << std::endl;
        return 0;
    }
}

bool JpegSteg::validateJpeg(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open file: " << path << std::endl;
        return false;
    }

    // Check file size
    file.seekg(0, std::ios::end);
    if (file.tellg() < 2) {
        std::cerr << "File too small to be JPEG" << std::endl;
        return false;
    }
    file.seekg(0);

    // Check JPEG magic numbers
    uint8_t magic[2];
    if (!file.read(reinterpret_cast<char*>(magic), 2)) {
        std::cerr << "Failed to read file header" << std::endl;
        return false;
    }
    
    if (magic[0] != 0xFF || magic[1] != 0xD8) {
        std::cerr << "Invalid JPEG header" << std::endl;
        return false;
    }

    return true;
}

std::vector<uint8_t> JpegSteg::preparePayload(const std::vector<uint8_t>& data) const {
    std::vector<uint8_t> payload(HEADER_SIZE + data.size());
    
    // Add magic header
    std::memcpy(payload.data(), MAGIC, 5);
    
    // Add size (4 bytes, little endian)
    uint32_t size = data.size();
    payload[5] = size & 0xFF;
    payload[6] = (size >> 8) & 0xFF;
    payload[7] = (size >> 16) & 0xFF;
    payload[8] = (size >> 24) & 0xFF;
    
    // Add data
    std::memcpy(payload.data() + HEADER_SIZE, data.data(), data.size());
    
    return payload;
}

} // namespace steg 