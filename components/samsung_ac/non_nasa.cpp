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

        bool decode(std::vector<uint8_t> &data, MessageTarget *target)
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

            std::string src = int_to_hex(data[1]);
            std::string dest = int_to_hex(data[2]);
            uint8_t cmd = data[3];

            target->register_address(src);

            switch (cmd)
            {
            case 0x20: // temperatures
            {
                uint8_t target_temp = data[4] - 55;
                uint8_t room_temp = data[5] - 55;
                uint8_t pipe_in = data[6] - 55;
                uint8_t fanspeed = data[7] & 0b00001111;
                bool bladeswing = (data[7] & 0b11110000) == 0xD0;
                bool power = data[8] & 0b10000000;
                uint8_t mode = data[8] & 0b00111111;
                uint8_t pipe_out = data[11] - 55;

                target->set_target_temperature(src, target_temp);
                target->set_room_temperature(src, room_temp);
                target->set_power(src, power);

                switch (fanspeed)
                {
                case 0: // auto
                    break;
                case 2: // low
                    break;
                case 4: // medium
                    break;
                case 5: // height
                    break;
                case 6: // fresh
                    break;
                }

                switch (mode)
                {
                case 0x01: // heat
                    target->set_mode(src, Mode::Heat);
                    break;
                case 0x02: // cool
                    target->set_mode(src, Mode::Cool);
                    break;
                case 0x04: // dry
                    target->set_mode(src, Mode::Dry);
                    break;
                case 0x08: // fan
                    target->set_mode(src, Mode::Fan);
                    break;
                case 0x21: // auto(heat)
                case 0x22: // auto(cool)
                    target->set_mode(src, Mode::Auto);
                    break;
                }
            }

            case 0xA0:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x64:
            case 0x40:
                ESP_LOGW("command", "%s %s %02X %s", src.c_str(), dest.c_str(), cmd, bytes_to_hex(data).c_str());
                break;

            default:
                break;
            }

            return true;
        }

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            decode(data, target);
        }
    } // namespace samsung_ac
} // namespace esphome
