#ifndef IIS2ICLX_HPP
#define IIS2ICLX_HPP

#include <Arduino.h>
#include <cstdint>
#include <Wire.h>

namespace iis2iclx {

struct Iis2iclxData
{
    float xAccelerationMg = 0.0F;
    float yAccelerationMg = 0.0F;
    float temperatureC = 0.0F;
    float xAngleDegree = 0.0F;
    float yAngleDegree = 0.0F;
};

class Iis2iclx
{
public:
    Iis2iclx(
        TwoWire & wire = Wire,
        const bool sa0IsVoltage = false
    ) noexcept;
    ~Iis2iclx() noexcept;

    [[nodiscard]] auto setup() noexcept -> bool;
    [[nodiscard]] auto read(Iis2iclxData & data) noexcept -> bool;

    [[nodiscard]] auto wire() noexcept -> TwoWire &;
    [[nodiscard]] auto address() const noexcept -> uint8_t;

private:
    TwoWire & _wire;
    const uint8_t _address;

    Iis2iclx(const Iis2iclx &) = delete;
    Iis2iclx(Iis2iclx &&) noexcept = delete;
    auto operator =(const Iis2iclx &) -> Iis2iclx & = delete;
    auto operator =(Iis2iclx &&) noexcept -> Iis2iclx & = delete;
};

} // namespace iis2iclx
#endif
