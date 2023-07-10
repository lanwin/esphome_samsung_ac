#include "util.h"

namespace esphome
{
    namespace samsung_ac
    {
        std::string int_to_hex(int number)
        {
            char str[3];
            sprintf(str, "%02x", number);
            return str;
        }

        std::string bytes_to_hex(const std::vector<uint8_t> &data)
        {
            std::string str;
            for (int i = 0; i < data.size(); i++)
            {
                str += int_to_hex(data[i]);
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
    } // namespace samsung_ac
} // namespace esphome
