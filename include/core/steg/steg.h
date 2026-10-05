#ifndef CORE_STEG_H
#define CORE_STEG_H

#include <string>
#include <vector>
#include <memory>

namespace steg {

/**
 * @brief Base interface for all steganography methods
 */
class ISteganographer {
public:
    virtual ~ISteganographer() = default;
    virtual bool embed(const std::string& target_path, 
                      const std::vector<uint8_t>& payload,
                      const std::string& output_path) = 0;
};

/**
 * @brief Factory class for creating steganographers
 */
class SteganographerFactory {
public:
    enum class Method {
        LSB,
        JSTEG
    };

    static std::unique_ptr<ISteganographer> create(Method method);
};

/**
 * @brief Core steganography functionality
 */
bool embed_lsb(const std::string& target_path, 
               const std::vector<uint8_t>& payload,
               const std::string& output_path);

} // namespace steg

#endif // CORE_STEG_H 