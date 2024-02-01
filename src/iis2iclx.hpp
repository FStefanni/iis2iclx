#ifndef IIS2ICLX_HPP
#define IIS2ICLX_HPP

#include <Arduino.h>
#include <cstdint>
#include <Wire.h>

namespace iis2iclx {

constexpr auto Iis2iclxDefaultAddress = ;

class Iis2iclx
{
public:
    Iis2iclx(
        TwoWire & wire = Wire,
        const bool sa0IsVoltage = false
    ) noexcept;
    ~Iis2iclx() noexcept;

    [[nodiscard]] auto setup() noexcept -> bool;
    [[nodiscard]] auto wire() noexcept -> TwoWire &;
    [[nodiscard]] auto writeAddress() const noexcept -> uint8_t;
    [[nodiscard]] auto readAddress() const noexcept -> uint8_t;

private:
    TwoWire & _wire;
    const int16_t _cs;
    const uint8_t _address;

    Iis2iclx(const Iis2iclx &) = delete;
    Iis2iclx(Iis2iclx &&) noexcept = delete;
    auto operator =(const Iis2iclx &) -> Iis2iclx & = delete;
    auto operator =(Iis2iclx &&) noexcept -> Iis2iclx & = delete;
};

} // namespace iis2iclx
#endif
