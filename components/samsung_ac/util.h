#pragma once

#include <vector>
#include <iostream>
#include <vector>
#include <bitset>
#include <optional>
#include <functional>
#include <experimental/optional>

namespace esphome
{
    namespace samsung_ac
    {
        static const char *TAG = "samsung_ac";

        std::string long_to_hex(long number);
        int hex_to_int(const std::string &hex);
        std::string bytes_to_hex(const std::vector<uint8_t> &data);
        std::vector<uint8_t> hex_to_bytes(const std::string &hex);
        void print_bits_8(uint8_t value);

        template <typename T>
        using opt = std::experimental::optional<T>;
        using nullopt_t = std::experimental::nullopt_t;
        constexpr auto nullopt = std::experimental::nullopt;

    } // namespace samsung_ac
} // namespace esphome
