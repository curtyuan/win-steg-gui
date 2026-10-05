#ifndef JSTEG_TEST_H
#define JSTEG_TEST_H

#include <string>
#include <vector>
#include <cstdint>

namespace test {

class JstegTest {
public:
    /**
     * @brief Test embedding and extracting shellcode
     * @return true if test passes
     */
    bool test_shellcode_embed_extract();

private:
    // Test helper functions
    bool compare_data(const std::vector<uint8_t>& original, 
                     const std::vector<uint8_t>& extracted);
    
    std::vector<uint8_t> load_shellcode();
    bool verify_jpeg(const std::string& path);

    // Test file paths
    const std::string test_jpeg = "test_data/lena.jpg";
    const std::string output_jpeg = "test_data/output.jpg";
    const std::string shellcode_path = "steg/payload/payload_strategy.h";
};

} // namespace test

#endif // JSTEG_TEST_H 