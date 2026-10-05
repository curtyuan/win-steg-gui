#ifndef PAYLOAD_STRATEGY_H
#define PAYLOAD_STRATEGY_H

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace paylo {

/**
 * @brief Abstract class for payload strategy
 */
class PayloadStrategy {
public:
    virtual std::vector<uint8_t> getPayload() const = 0;
    virtual uint32_t getPayloadSize() const = 0;
    virtual ~PayloadStrategy() = default;
};

/**
 * @brief PlainTextPayloadStrategy class for plain text payload strategy
 */
class PlainTextPayloadStrategy : public PayloadStrategy {
public:
    PlainTextPayloadStrategy();
    std::vector<uint8_t> getPayload() const override;
    uint32_t getPayloadSize() const override;
};

/**
 * @brief CalcPayloadStrategy class for calc.exe shellcode payload
 */
class CalcPayloadStrategy : public PayloadStrategy {
public:
    CalcPayloadStrategy() = default;
    std::vector<uint8_t> getPayload() const override;
    uint32_t getPayloadSize() const override;
};

} // namespace paylo

#endif // PAYLOAD_STRATEGY_H 