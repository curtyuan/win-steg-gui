/**
 * @file lsb_steg.h
 * @brief LSB steganography implementation
 * @copyright Copyright (C) 2023
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef CORE_LSB_STEG_H
#define CORE_LSB_STEG_H

#include <string>
#include <vector>
#include <cstdint>
#include "core/steg/steg.h"

namespace steg {

/**
 * @brief LSB steganography implementation
 */
class LsbSteganographer : public ISteganographer {
public:
    bool embed(const std::string& target_path,
              const std::vector<uint8_t>& payload,
              const std::string& output_path) override;

private:
    bool write_header(uint8_t* data, size_t size, uint32_t payload_size);
    bool embed_payload(uint8_t* data, size_t size, const std::vector<uint8_t>& payload);
};

} // namespace steg

#endif // CORE_LSB_STEG_H
 