/**
 * @file lsb_extract.h
 * @brief LSB steganography extraction implementation
 * @copyright Copyright (C) 2023
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef CORE_EXTRACT_LSB_EXTRACT_H_
#define CORE_EXTRACT_LSB_EXTRACT_H_

#include <string>
#include <vector>
#include <cstdint>
#include "core/extract/extract.h"
#include "gui/blimp/blimp.h"

namespace extract {

/**
 * @brief LSB steganography extractor
 * 
 * This class implements LSB (Least Significant Bit) steganography extraction.
 * It can extract hidden data from BMP images where data was hidden in the
 * least significant bits of pixel values.
 */
class LsbExtractor : public IExtractor {
public:
    /**
     * @brief Extract data from a BMP file
     * @param source_path Path to the BMP file
     * @return Vector containing the extracted data
     */
    std::vector<uint8_t> extract(const std::string& source_path) override;

    /**
     * @brief Extract data from a BMP image
     * @param image BMP image object
     * @return Vector containing extracted data
     */
    std::vector<uint8_t> extract_from_data(const gui::BMP& image);

    /**
     * @brief Validate input data
     * @param input_data Data to validate
     * @return true if data appears to be valid BMP data
     */
    bool validate_input(const std::vector<uint8_t>& input_data) const;

    /**
     * @brief Get the name of this extraction algorithm
     * @return String "LSB"
     */
    std::string get_algorithm_name() const { return "LSB"; }

private:
    /**
     * @brief Extract 2 LSB bits from a channel
     * @param image BMP image object
     * @param channelIndex Index of the channel to extract from
     * @return Extracted 2 LSB bits
     */
    uint8_t extract_2lsb(const gui::BMP& image, uint32_t& channelIndex);
};

} // namespace extract

#endif // CORE_EXTRACT_LSB_EXTRACT_H_ 