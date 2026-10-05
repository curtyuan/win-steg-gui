#ifndef JPEG_STEG_H
#define JPEG_STEG_H

#include <vector>
#include <string>
#include <cstdint>

namespace steg {

// Add temporary stub class until vcpkg is set up
namespace JPEG {
    class Image {
    public:
        void load(const std::string&) {}
        void save(const std::string&) {}
        std::vector<std::vector<int>> get_coefficients() { return {}; }
        void set_coefficients(const std::vector<std::vector<int>>&) {}
    };
}

class JpegSteg {
public:
    JpegSteg() = default;
    ~JpegSteg() = default;

    // Core methods
    bool hide(const std::string& input_jpeg,
             const std::string& output_jpeg, 
             const std::vector<uint8_t>& data);

    std::vector<uint8_t> extract(const std::string& jpeg_path);
    size_t maxPayloadSize(const std::string& jpeg_path);

private:
    static constexpr char MAGIC[] = "jsteg";
    static constexpr size_t HEADER_SIZE = 9;

    bool validateJpeg(const std::string& path) const;
    std::vector<uint8_t> preparePayload(const std::vector<uint8_t>& data) const;
};

} // namespace steg
#endif 