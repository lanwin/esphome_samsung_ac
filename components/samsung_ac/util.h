#pragma once

#include <vector>
#include <iostream>

namespace esphome
{
    namespace samsung_ac
    {
        std::string int_to_hex(int number);
        std::string bytes_to_hex(const std::vector<uint8_t> &data);
        std::vector<uint8_t> hex_to_bytes(const std::string &hex);
    } // namespace samsung_ac
} // namespace esphome
