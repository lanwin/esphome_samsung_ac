#pragma once
// Fake Hal for Local Testing

namespace esphome
{
    uint32_t millis()
    {
        return 0;
    }
    uint32_t micros()
    {
        return 0;
    }
    void delay(uint32_t ms) {}
} // namespace esphome
