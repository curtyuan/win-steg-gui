#include "core/aes/aes_decryptor.h"
#include "core/aes/tinyaes/aes.h"
#include "utils/file_util.h"
#include "core/extract/mapping.h"
#include "utils/debug_print.h"
#include <stdexcept>

namespace aes {

AesDecryptor::AesDecryptor() : is_initialized_(false) {}

AesDecryptor::~AesDecryptor() {
    // Clear sensitive data
    key_.clear();
    iv_.clear();
}

bool AesDecryptor::init_from_file(const std::string& key_file_path) {
    try {
        // Read key and IV from file
        if (!utils::read_aes_data(key_file_path, key_, iv_)) {
            return false;
        }
        
        // Validate sizes
        if (key_.size() != 32 || iv_.size() != 16) {
            key_.clear();
            iv_.clear();
            return false;
        }
        
        is_initialized_ = true;
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool AesDecryptor::init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    if (key.size() != 32 || iv.size() != 16) {
        return false;
    }
    
    key_ = key;
    iv_ = iv;
    is_initialized_ = true;
    return true;
}

bool AesDecryptor::init_from_mapped_data(PVOID mapped_data, SIZE_T data_size) {
    try {
        if (!mapped_data || data_size < 8) { // At least space for both sizes
            return false;
        }

        auto data = static_cast<const uint8_t*>(mapped_data);
        
        // Read key size
        uint32_t key_size = *reinterpret_cast<const uint32_t*>(data);
        if (data_size < 4 + key_size + 4) { // size + key + iv_size
            return false;
        }

        // Read key data
        key_.assign(data + 4, data + 4 + key_size);

        // Read IV size
        uint32_t iv_size = *reinterpret_cast<const uint32_t*>(data + 4 + key_size);
        if (data_size < 4 + key_size + 4 + iv_size) { // total needed size
            return false;
        }

        // Read IV data
        iv_.assign(data + 8 + key_size, data + 8 + key_size + iv_size);
        
        is_initialized_ = true;
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

std::vector<uint8_t> AesDecryptor::decrypt(const std::vector<uint8_t>& encrypted_data) {
    debug::DebugPrint("Starting data decryption...");
    
    if (!is_initialized_) {
        debug::DebugPrint("Decryptor not initialized");
        throw std::runtime_error("AesDecryptor not initialized with key/IV");
    }

    if (encrypted_data.size() < AES_BLOCKLEN) {
        debug::DebugPrint("Encrypted data too short");
        throw std::runtime_error("Encrypted data too short to contain IV");
    }

    // Skip the IV at the start of encrypted data (it should match our iv_)
    std::vector<uint8_t> ciphertext(encrypted_data.begin() + AES_BLOCKLEN, encrypted_data.end());

    if (ciphertext.empty() || ciphertext.size() % AES_BLOCKLEN != 0) {
        debug::DebugPrint("Invalid ciphertext size");
        throw std::runtime_error("Invalid ciphertext size");
    }

    // Create AES context
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key_.data(), iv_.data());  // Use IV from key file

    // Copy data for decryption (AES-256-CBC modifies data in place)
    std::vector<uint8_t> decrypted = ciphertext;
    
    // Decrypt blocks
    AES_CBC_decrypt_buffer(&ctx, decrypted.data(), decrypted.size());
    
    // Get padding size from last byte (PKCS7)
    uint8_t padding = decrypted.back();
    
    // Validate padding
    if (padding == 0 || padding > AES_BLOCKLEN || padding > decrypted.size()) {
        debug::DebugPrint("Invalid PKCS7 padding");
        throw std::runtime_error("Invalid PKCS7 padding");
    }
    
    // Verify padding bytes
    for (size_t i = decrypted.size() - padding; i < decrypted.size(); ++i) {
        if (decrypted[i] != padding) {
            debug::DebugPrint("Invalid PKCS7 padding bytes");
            throw std::runtime_error("Invalid PKCS7 padding bytes");
        }
    }
    
    // Remove padding
    decrypted.resize(decrypted.size() - padding);
    
    debug::DebugPrint("Data decryption completed successfully");
    return decrypted;
}

std::vector<uint8_t> AesDecryptor::string_to_bytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::string AesDecryptor::bytes_to_string(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

} // namespace aes 