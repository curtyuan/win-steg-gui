#ifndef CORE_EXTRACT_H
#define CORE_EXTRACT_H

#include <string>
#include <vector>
#include <memory>

namespace extract {

/**
 * @brief Base interface for all extraction methods
 */
class IExtractor {
public:
    virtual ~IExtractor() = default;
    virtual std::vector<uint8_t> extract(const std::string& source_path) = 0;
};

/**
 * @brief Factory class for creating extractors
 */
class ExtractorFactory {
public:
    enum class Method {
        LSB,
        JSTEG
    };

    static std::unique_ptr<IExtractor> create(Method method);
};

/**
 * @brief Core extraction functionality
 */
std::vector<uint8_t> extract_lsb(const std::string& source_path);

} // namespace extract

#endif // CORE_EXTRACT_H 