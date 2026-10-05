#include "jsteg_test.h"
#include "core/steg/jpeg_steg.h"
#include "core/extract/jpeg_extract.h"
#include <fstream>
#include <iostream>
#include <cstring>

namespace test {

bool JstegTest::test_shellcode_embed_extract() {
    /*
    try {
        // Load shellcode
        auto shellcode = load_shellcode();
        if (shellcode.empty()) {
            std::cerr << "Failed to load shellcode" << std::endl;
            return false;
        }

        // Create steg instance
        steg::JpegSteg steg;

        // Check if image can hold the payload
        size_t max_size = steg.maxPayloadSize(test_jpeg);
        if (max_size < shellcode.size()) {
            std::cerr << "Test image too small for shellcode ("
                     << shellcode.size() << " bytes needed, "
                     << max_size << " bytes available)" << std::endl;
            return false;
        }

        // Embed shellcode
        if (!steg.hide(test_jpeg, output_jpeg, shellcode)) {
            std::cerr << "Failed to embed shellcode" << std::endl;
            return false;
        }

        // Verify output is valid JPEG
        if (!verify_jpeg(output_jpeg)) {
            std::cerr << "Output is not a valid JPEG" << std::endl;
            return false;
        }

        // Extract and verify
        auto extracted = steg.extract(output_jpeg);
        if (extracted.empty()) {
            std::cerr << "Failed to extract data" << std::endl;
            return false;
        }

        // Compare original and extracted data
        if (!compare_data(shellcode, extracted)) {
            std::cerr << "Extracted data does not match original" << std::endl;
            return false;
        }

        std::cout << "Successfully embedded and extracted shellcode ("
                 << shellcode.size() << " bytes)" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return false;
    }
    */
    return false; // Temporarily disabled
}

/*
bool JstegTest::compare_data(const std::vector<uint8_t>& original,
                           const std::vector<uint8_t>& extracted) {
    if (original.size() != extracted.size()) {
        std::cerr << "Size mismatch: original=" << original.size()
                 << " extracted=" << extracted.size() << std::endl;
        return false;
    }

    for (size_t i = 0; i < original.size(); i++) {
        if (original[i] != extracted[i]) {
            std::cerr << "Mismatch at byte " << i << ": "
                     << std::hex << (int)original[i] << " != "
                     << (int)extracted[i] << std::dec << std::endl;
            return false;
        }
    }

    return true;
}

std::vector<uint8_t> JstegTest::load_shellcode() {
    std::ifstream file(shellcode_path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open shellcode file: " << shellcode_path << std::endl;
        return {};
    }

    // Read entire file
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

bool JstegTest::verify_jpeg(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }

    // Check JPEG magic numbers
    uint8_t magic[2];
    if (!file.read(reinterpret_cast<char*>(magic), 2)) {
        return false;
    }

    return magic[0] == 0xFF && magic[1] == 0xD8;
}
*/

} // namespace test 