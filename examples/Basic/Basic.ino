#include <Wire.h>
#include <iis2iclx.hpp>

namespace /*anon*/ {

constexpr auto sda = int(21);
constexpr auto scl = int(22);
constexpr auto readDelay = 2000;

iis2iclx::Iis2iclx sensor;

[[noreturn]] auto fatalError(const char * const msg) noexcept -> void
{
    constexpr auto timeout = 2000;
    while (true)
    {
        Serial.println(msg);
        delay(timeout);
    }
}

} // namespace

auto setup() -> void
{
    Serial.begin(115200);
    Wire.setPins(sda, scl);
    Wire.begin();
    if (!sensor.setup()) fatalError("Unable to setup iis2iclx");
    Serial.println("Setup success!");
}

auto loop() -> void
{
    iis2iclx::Iis2iclxData data;
    const auto ok = sensor.read(data);
    if (!ok) return;
    Serial.println("Acceleration X (mg): " + String(data.xAccelerationMg));
    Serial.println("Acceleration Y (mg): " + String(data.yAccelerationMg));
    Serial.println("Temperature (C): " + String(data.temperatureC));
    Serial.println("Angle X (Degree): " + String(data.xAngleDegree));
    Serial.println("Angle Y (Degree): " + String(data.yAngleDegree));
    delay(readDelay);
}
