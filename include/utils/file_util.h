#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace utils {

/**
 * @brief Read entire file as binary data
 * @param file_path Path to the file
 * @return Vector containing file contents
 * @throws std::runtime_error if file operations fail
 */
std::vector<uint8_t> read_binary(const std::string& file_path);

/**
 * @brief Read binary file into provided vector
 * @param file_path Path to the file
 * @param data Vector to store file contents
 * @return true if successful, false otherwise
 */
bool read_binary_file(const std::string& file_path, std::vector<uint8_t>& data);

/**
 * @brief Read AES key and IV from a file
 * @param file_path Path to the key file
 * @param key Vector to store the key
 * @param iv Vector to store the IV
 * @return true if successful, false otherwise
 */
bool read_aes_data(const std::string& file_path, std::vector<uint8_t>& key, std::vector<uint8_t>& iv);

/**
 * @brief Save raw binary data to a file
 * @param filename Path to save the file
 * @param data Vector containing the raw data
 * @return true if successful, false otherwise
 */
bool save_raw_data(const std::string& filename, const std::vector<uint8_t>& data);

/**
 * @brief Save raw binary data to a file
 * @param filename Path to save the file
 * @param data Pointer to the raw data
 * @param size Size of the data in bytes
 * @return true if successful, false otherwise
 */
bool save_raw_data(const std::string& filename, const uint8_t* data, size_t size);

} // namespace utils 