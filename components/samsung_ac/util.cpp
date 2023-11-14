#include "util.h"

namespace esphome
{
    namespace samsung_ac
    {
        std::string long_to_hex(long number)
        {
            char str[10];
            sprintf(str, "%02lx", number);
            return str;
        }

        int hex_to_int(const std::string &hex)
        {
            return (int)strtol(hex.c_str(), NULL, 16);
        }

        std::string bytes_to_hex(const std::vector<uint8_t> &data)
        {
            std::string str;
            for (int i = 0; i < data.size(); i++)
            {
                str += long_to_hex(data[i]);
            }
            return str;
        }

        std::vector<uint8_t> hex_to_bytes(const std::string &hex)
        {
            std::vector<uint8_t> bytes;
            for (unsigned int i = 0; i < hex.length(); i += 2)
            {
                bytes.push_back((uint8_t)strtol(hex.substr(i, 2).c_str(), NULL, 16));
            }
            return bytes;
        }

        void print_bits_8(uint8_t value)
        {
            std::cout << std::bitset<8>(value) << std::endl;
        }
    } // namespace samsung_ac
} // namespace esphome
