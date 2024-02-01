#include "iis2iclx.hpp"
#include "iis2iclx_reg.h"

namespace iis2iclx {

namespace /*anon*/ {


auto platform_write(void * handle, uint8_t Reg, const uint8_t * Bufp, uint16_t len) -> int32_t
{
    auto & iis = *reinterpret_cast<Iis2iclx *>(handle);
    auto & wire = iis.wire();

    wire.beginTransmission(iis.writeAddress());

    const auto regSize = wire.write(&Reg, sizeof(uint8_t));
    if (regSize != 1) return -1;

    const auto retSize = wire.write(Bufp, len);
    if (len != retSize) return retSize != 0 ? retSize : -2;

    const auto err = wire.endTransmission();

    return err;
}

auto platform_read(void * handle, uint8_t Reg, uint8_t * Bufp, uint16_t len) -> int32_t
{
    auto & iis = *reinterpret_cast<Iis2iclx *>(handle);
    auto & wire = iis.wire();

    wire.beginTransmission(iis.writeAddress());
    const auto regSize = wire.write(&Reg, sizeof(uint8_t));
    const auto err = wire.endTransmission();
    if (regSize != 1) return -1;
    if (err != 0) return err;

    wire.requestFrom(iis.readAddress(), uint8_t(len));

    const auto * const maxAddr = Bufp + len;
    auto retSize = uint16_t(0);
    while ((wire.available() != 0) && Bufp < maxAddr)
    {
        *Bufp = uint8_t(wire.read());
        Bufp += 1;
        retSize += 1;
    }
    if (retSize != len) return retSize != 0 ? retSize : -1;
    return 0;
}

} // namespace

Iis2iclx::Iis2iclx(
    TwoWire & wire,
    const bool sa0IsVoltage
) noexcept:
    _wire(wire),
    _address(sa0IsVoltage ? IIS2ICLX_I2C_ADD_H : IIS2ICLX_I2C_ADD_L)
{}

Iis2iclx::~Iis2iclx() noexcept
{}

auto Iis2iclx::setup() noexcept -> bool
{
    stmdev_ctx_t ctx = {
        .write_reg = platform_write,
        .read_reg = platform_read
        .handle = this
    };
    return true;
}

auto Iis2iclx::wire() noexcept -> TwoWire &
{
    return _wire;
}

auto Iis2iclx::writeAddress() const noexcept -> uint8_t
{
    return _address & 0xFE;
}

auto Iis2iclx::readAddress() const noexcept -> uint8_t
{
    return _address;
}

} // namespace iis2iclx
