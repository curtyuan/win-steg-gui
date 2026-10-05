#ifndef MAPPING_H
#define MAPPING_H

#include <Windows.h>
#include <vector>
#include <string>

namespace extract {

/**
 * @brief Map a file into memory with read-only permissions
 * @param file_path Path to the file to map
 * @param mapped_ptr Output pointer to the mapped address
 * @param size Output parameter for size of mapped data
 * @return true if mapping successful, false otherwise
 */
bool map_file_readonly(const std::string& file_path, void*& mapped_ptr, size_t& size);

/**
 * @brief Map data into memory with execute permissions
 * @param data Pointer to the data to map
 * @param size Size of the data
 * @param mapped_ptr Output pointer to the mapped address
 * @return true if mapping successful, false otherwise
 */
bool map_data(const uint8_t* data, size_t size, void*& mapped_ptr);

/**
 * @brief Execute data using Local Mapping
 * @param data The data to map and execute
 */
void execute_mapping(const std::vector<unsigned char>& data);

} // namespace extract

#endif // MAPPING_H 