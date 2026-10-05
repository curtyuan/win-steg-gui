#include "extract_decrypt_test.h"
#include "core/extract/extract.h"
#include "core/extract/lsb_extract.h"
#include "core/aes/aes_decryptor.h"
#include "utils/file_util.h"
#include "utils/file_logger.h"
#include "utils/alg_util.h"
#include "core/steg/payload/payload_strategy.h"
#include <string>
#include <vector>

namespace test {

bool run_extract_decrypt_tests() {
    // Test image path
    std::string image_path = "assets/images/lena3.bmp";
    std::string key_path = "assets/bins/aes_data.bin";
    
    utils::FileLogger::log("[TEST] Starting extraction and decryption test");
    utils::FileLogger::log("[TEST] Using image: " + image_path);
    utils::FileLogger::log("[TEST] Using key file: " + key_path);

    // Extract encrypted data from image
    std::vector<uint8_t> encrypted_data;
    try {
        extract::LsbExtractor extractor;
        encrypted_data = extractor.extract(image_path);
        utils::FileLogger::log("[TEST] Successfully extracted encrypted data from image");
        utils::FileLogger::log("[TEST] Extracted data size: " + std::to_string(encrypted_data.size()) + " bytes");
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[TEST] Failed to extract data: " + std::string(e.what()));
        return false;
    }

    // Read key and IV
    std::vector<uint8_t> key, iv;
    if (!utils::read_aes_data(key_path, key, iv)) {
        utils::FileLogger::log("[TEST] Failed to read key data");
        return false;
    }

    // Decrypt the data
    std::vector<uint8_t> decrypted_data;
    try {
        aes::AesDecryptor decryptor;
        if (!decryptor.init(key, iv)) {
            utils::FileLogger::log("[TEST] Failed to initialize AES decryptor");
            return false;
        }
        decrypted_data = decryptor.decrypt(encrypted_data);
        utils::FileLogger::log("[TEST] Successfully decrypted data");
        utils::FileLogger::log("[TEST] Decrypted data size: " + std::to_string(decrypted_data.size()) + " bytes");
    }
    catch (const std::exception& e) {
        utils::FileLogger::log("[TEST] Failed to decrypt data: " + std::string(e.what()));
        return false;
    }

    // Get expected payload
    paylo::PlainTextPayloadStrategy payload_strategy;
    std::vector<uint8_t> expected_payload = payload_strategy.getPayload();

    // Compare with expected payload
    if (utils::compare_bytes(decrypted_data, expected_payload, "[TEST]")) {
        utils::FileLogger::log("[TEST] Validation successful: Decrypted payload matches expected reverse shell");
    } else {
        utils::FileLogger::log("[TEST] Validation failed: Payload does not match expected data");
        return false;
    }

    utils::FileLogger::log("[TEST] Test completed successfully");
    return true;
}

} // namespace test