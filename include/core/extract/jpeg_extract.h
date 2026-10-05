#ifndef JPEG_EXTRACT_H
#define JPEG_EXTRACT_H

#include <vector>
#include <string>
#include <cstdint>
#include "core/extract/extract.h"

namespace extract {

class JpegExtractor : public IExtractor {
public:
    JpegExtractor() = default;
    ~JpegExtractor() override = default;

    std::vector<uint8_t> extract(const std::string& source_path) override;

private:
    static constexpr char MAGIC[] = "jsteg";
    static constexpr size_t HEADER_SIZE = 9;

    bool validate_jpeg(const std::string& path) const;
    std::vector<uint8_t> read_payload(const std::string& path) const;
};

} // namespace extract

#endif // JPEG_EXTRACT_H 