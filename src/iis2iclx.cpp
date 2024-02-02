#include "iis2iclx.hpp"
#include "iis2iclx_reg.h"
#include <array>

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
    constexpr auto maxInit = size_t(5);
    constexpr auto initDelay = 1000;

    const stmdev_ctx_t ctx = {
        .write_reg = platform_write,
        .read_reg = platform_read,
        .mdelay = nullptr,
        .handle = this
    };

    const auto err0 = iis2iclx_bus_mode_set(&ctx, IIS2ICLX_SEL_BY_HW);
    if (err0 != 0) return false;

    for (auto i = size_t(0); ; ++i)
    {
        auto whoAmI = uint8_t(0);
        const auto err = iis2iclx_device_id_get(&ctx, &whoAmI);
        if (err == 0 && whoAmI == IIS2ICLX_ID) break;
        if (i + 1 >= maxInit) return false;
        delay(initDelay);
    }

    const auto err1 = iis2iclx_reset_set(&ctx, PROPERTY_ENABLE);
    if (err1 != 0) return false;

    auto rst = uint8_t(0);
    do
    {
        const auto err = iis2iclx_reset_get(&ctx, &rst);
        if (err != 0) return false;
    } while (rst);

    return true;
}

auto Iis2iclx::read(Iis2iclxData & data) noexcept -> bool
{
    constexpr auto dataSize = size_t(2);
    const stmdev_ctx_t ctx = {
        .write_reg = platform_write,
        .read_reg = platform_read,
        .mdelay = nullptr,
        .handle = this
    };
    auto reg = uint8_t(0);

    // Read acceleration field data
    const auto err0 = iis2iclx_xl_flag_data_ready_get(&ctx, &reg);
    if (err0 != 0 || reg == 0) return false;
    std::array<int16_t, dataSize> rawAcceleration = {};
    const auto err1 = iis2iclx_acceleration_raw_get(&ctx, rawAcceleration.data());
    if (err1 != 0) return false;

    /* Read temperature data */
    const auto err2 = iis2iclx_temp_flag_data_ready_get(&ctx, &reg);
    if (err2 != 0 || reg == 0) return false;
    auto rawTemperature = int16_t(0);
    const auto err3 = iis2iclx_temperature_raw_get(&ctx, &rawTemperature);
    if (err3 != 0) return false;

    data.xAccelerationMg = iis2iclx_from_fs2g_to_mg(rawAcceleration[0]);
    data.yAccelerationMg = iis2iclx_from_fs2g_to_mg(rawAcceleration[1]);
    data.temperatureC = iis2iclx_from_lsb_to_celsius(rawTemperature);
    //data.xAngle = arcsin()

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
