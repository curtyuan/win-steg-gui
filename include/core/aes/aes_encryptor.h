#ifndef AES_ENCRYPTOR_H
#define AES_ENCRYPTOR_H

#include <string>
#include <vector>
#include <cstdint>
#include <random>

namespace aes {

class AesEncryptor {
public:
    AesEncryptor();
    ~AesEncryptor();

    // Initialize with random key and IV
    bool init();
    
    // Initialize with provided key and IV
    bool init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
    
    // Save key and IV to file for later decryption
    bool save_key_iv(const std::string& file_path) const;
    
    // Core encryption function - works with raw data
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);
    
    // Helper methods
    static std::vector<uint8_t> string_to_bytes(const std::string& str);
    static std::string bytes_to_string(const std::vector<uint8_t>& bytes);

private:
    // Generate random bytes for key/IV
    static std::vector<uint8_t> generate_random_bytes(size_t size);
    
    std::vector<uint8_t> key_;  // 32 bytes for AES-256
    std::vector<uint8_t> iv_;   // 16 bytes for CBC mode
    bool is_initialized_;
};

} // namespace aes

#endif // AES_ENCRYPTOR_H 