#ifndef AES_DECRYPTOR_H
#define AES_DECRYPTOR_H

#include <string>
#include <vector>
#include <cstdint>
#include <Windows.h>

namespace aes {

class AesDecryptor {
public:
    AesDecryptor();
    ~AesDecryptor();

    // Initialize with key and IV from file
    bool init_from_file(const std::string& key_file_path);
    
    // Initialize with key and IV from mapped memory
    bool init_from_mapped_data(PVOID mapped_data, SIZE_T data_size);
    
    // Initialize with provided key and IV
    bool init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
    
    // Core decryption function - works with raw data
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encrypted_data);
    
    // Helper methods
    static std::vector<uint8_t> string_to_bytes(const std::string& str);
    static std::string bytes_to_string(const std::vector<uint8_t>& bytes);

private:
    std::vector<uint8_t> key_;  // 32 bytes for AES-256
    std::vector<uint8_t> iv_;   // 16 bytes for CBC mode
    bool is_initialized_;
};

} // namespace aes

#endif // AES_DECRYPTOR_H 