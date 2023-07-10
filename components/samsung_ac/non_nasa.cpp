#include <queue>
#include <iostream>
#include "esphome/core/log.h"
#include "util.h"
#include "nasa.h"

namespace esphome
{
    namespace samsung_ac
    {
        static const char *TAG = "samsung_non_nasa";

        bool decode(std::vector<uint8_t> &data)
        {
            if (data[0] != 0x32)
            {
                ESP_LOGV(TAG, "invalid start byte");
                return false;
            }

            if (data[data.size() - 1] != 0x34)
            {
                ESP_LOGV(TAG, "invalid start byte");
                return false;
            }

            uint8_t chksum = data[data.size() - 2];

            uint8_t src = data[1];
            uint8_t dest = data[2];
            uint8_t cmd = data[3];

            switch (cmd)
            {
            case 0xA0:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x64:
            case 0x20:
            case 0x40:
                ESP_LOGW("command", "%02X %02X %02X %s", src, dest, cmd, bytes_to_hex(data).c_str());
                break;

            default:
                break;
            }

            return true;
        }

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            decode(data);
        }
    } // namespace samsung_ac
} // namespace esphome
