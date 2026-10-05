#include "core/aes/aes_encryptor.h"
#include "core/aes/tinyaes/aes.h"
#include <fstream>
#include <stdexcept>
#include <random>

namespace aes {

AesEncryptor::AesEncryptor() : is_initialized_(false) {}

AesEncryptor::~AesEncryptor() {
    // Clear sensitive data
    key_.clear();
    iv_.clear();
}

bool AesEncryptor::init() {
    // Generate random key (32 bytes for AES-256)
    key_ = generate_random_bytes(32);
    
    // Generate random IV (16 bytes)
    iv_ = generate_random_bytes(16);
    
    is_initialized_ = true;
    return true;
}

bool AesEncryptor::init(const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv) {
    if (key.size() != 32 || iv.size() != 16) {
        return false;
    }
    
    key_ = key;
    iv_ = iv;
    is_initialized_ = true;
    return true;
}

bool AesEncryptor::save_key_iv(const std::string& file_path) const {
    if (!is_initialized_) {
        return false;
    }

    std::ofstream file(file_path, std::ios::binary);
    if (!file) {
        return false;
    }

    // Write key followed by IV
    file.write(reinterpret_cast<const char*>(key_.data()), key_.size());
    file.write(reinterpret_cast<const char*>(iv_.data()), iv_.size());

    return true;
}

std::vector<uint8_t> AesEncryptor::encrypt(const std::vector<uint8_t>& data) {
    if (!is_initialized_) {
        throw std::runtime_error("AesEncryptor not initialized with key/IV");
    }

    if (data.empty()) {
        return std::vector<uint8_t>();
    }

    // Create AES context
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key_.data(), iv_.data());

    // Copy data for encryption (AES-256-CBC modifies data in place)
    std::vector<uint8_t> encrypted = data;
    
    // Calculate PKCS7 padding
    // Always add padding, even if the data size is a multiple of block size
    // If it's a multiple, add a full block of padding
    size_t padding = AES_BLOCKLEN - (encrypted.size() % AES_BLOCKLEN);
    if (padding == 0) {
        padding = AES_BLOCKLEN;
    }
    
    // Add padding bytes
    encrypted.resize(encrypted.size() + padding, static_cast<uint8_t>(padding));
    
    // Encrypt blocks
    AES_CBC_encrypt_buffer(&ctx, encrypted.data(), encrypted.size());

    return encrypted;
}

std::vector<uint8_t> AesEncryptor::string_to_bytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::string AesEncryptor::bytes_to_string(const std::vector<uint8_t>& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

std::vector<uint8_t> AesEncryptor::generate_random_bytes(size_t size) {
    std::vector<uint8_t> bytes(size);
    
    // Use random_device for better randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    // Generate random bytes
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(dis(gen));
    }
    
    return bytes;
}

} // namespace aes 