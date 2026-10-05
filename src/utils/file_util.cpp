#include "utils/file_util.h"
#include "utils/file_logger.h"
#include <fstream>
#include <stdexcept>
#include <sstream>

namespace utils {

std::vector<uint8_t> read_binary(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read file content
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + file_path);
    }
    
    return buffer;
}

bool read_binary_file(const std::string& file_path, std::vector<uint8_t>& data) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            return false;
        }
        
        // Get file size
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Read file content
        data.resize(size);
        if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
            return false;
        }
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool read_aes_data(const std::string& file_path, std::vector<uint8_t>& key, std::vector<uint8_t>& iv) {
    try {
        utils::FileLogger::log("[FILE] Opening AES data file: " + file_path);
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            utils::FileLogger::log("[FILE] Failed to open AES data file");
            return false;
        }

        // Read key size
        uint32_t key_size = 0;
        if (!file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size))) {
            utils::FileLogger::log("[FILE] Failed to read key size");
            return false;
        }
        utils::FileLogger::log("[FILE] Key size: " + std::to_string(key_size) + " bytes");

        // Read key data
        key.resize(key_size);
        if (!file.read(reinterpret_cast<char*>(key.data()), key_size)) {
            utils::FileLogger::log("[FILE] Failed to read key data");
            return false;
        }
        utils::FileLogger::log("[FILE] Successfully read key data");

        // Read IV size
        uint32_t iv_size = 0;
        if (!file.read(reinterpret_cast<char*>(&iv_size), sizeof(iv_size))) {
            utils::FileLogger::log("[FILE] Failed to read IV size");
            return false;
        }
        utils::FileLogger::log("[FILE] IV size: " + std::to_string(iv_size) + " bytes");

        // Read IV data
        iv.resize(iv_size);
        if (!file.read(reinterpret_cast<char*>(iv.data()), iv_size)) {
            utils::FileLogger::log("[FILE] Failed to read IV data");
            return false;
        }
        utils::FileLogger::log("[FILE] Successfully read IV data");

        return true;
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[FILE] Exception reading AES data: " + std::string(e.what()));
        return false;
    }
}

bool save_raw_data(const std::string& filename, const std::vector<uint8_t>& data) {
    return save_raw_data(filename, data.data(), data.size());
}

bool save_raw_data(const std::string& filename, const uint8_t* data, size_t size) {
    if (!data || size == 0) {
        utils::FileLogger::log("[FILE] Invalid data or size for saving");
        return false;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        utils::FileLogger::log("[FILE] Failed to open file for writing: " + filename);
        return false;
    }

    file.write(reinterpret_cast<const char*>(data), size);
    
    if (!file) {
        utils::FileLogger::log("[FILE] Failed to write data to file: " + filename);
        return false;
    }

    file.close();
    utils::FileLogger::log("[FILE] Successfully saved " + std::to_string(size) + " bytes to " + filename);
    return true;
}

} // namespace utils 