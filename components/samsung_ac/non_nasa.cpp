#include <queue>
#include <iostream>
#include "esphome/core/log.h"
#include "util.h"
#include "non_nasa.h"

static const char *TAG = "samsung_non_nasa";

namespace esphome
{
    namespace samsung_ac
    {
        uint8_t build_checksum(std::vector<uint8_t> &data)
        {
            uint8_t sum = data[1];
            for (uint8_t i = 2; i < 12; i++)
            {
                sum = sum ^ data[i];
            }
            return sum;
        }

        std::string NonNasaCommand20::to_string()
        {
            std::string str;
            str += "{";
            str += "target_temp:" + std::to_string(target_temp) + ";";
            str += "room_temp:" + std::to_string(room_temp) + ";";
            str += "pipe_in:" + std::to_string(pipe_in) + ";";
            str += "pipe_out:" + std::to_string(pipe_out) + ";";
            str += "power:" + std::to_string(power ? 1 : 0) + ";";
            str += "wind_direction:" + std::to_string((uint8_t)wind_direction) + ";";
            str += "fanspeed:" + std::to_string((uint8_t)fanspeed) + ";";
            str += "mode:" + int_to_hex((uint8_t)mode) + ";";
            str += "}";
            return str;
        }

        std::string NonNasaPacket::to_string()
        {
            std::string str;
            str += "{";
            str += "src:" + src + ";";
            str += "dst:" + dst + ";";
            str += "cmd:" + std::to_string(20) + ";";
            str += "command:" + command.to_string() + ";";
            str += "}";
            return str;
        }

        bool NonNasaPacket::decode(std::vector<uint8_t> &data)
        {
            ESP_LOGW(TAG, "decode %s", bytes_to_hex(data).c_str());

            if (data[0] != 0x32)
            {
                ESP_LOGV(TAG, "invalid start byte");
                return false;
            }

            if (data.size() != 14)
            {
                ESP_LOGV(TAG, "invalid size");
                return false;
            }

            if (data[13] != 0x34)
            {
                ESP_LOGV(TAG, "invalid end byte");
                return false;
            }

            if (data[12] != build_checksum(data))
            {
                ESP_LOGV(TAG, "invalid checksum");
                return false;
            }

            src = int_to_hex(data[1]);
            dst = int_to_hex(data[2]);
            uint8_t cmd = data[3];

            switch (cmd)
            {
            case 0x20: // temperatures
            {
                command.target_temp = data[4] - 55;
                command.room_temp = data[5] - 55;
                command.pipe_in = data[6] - 55;
                command.wind_direction = (NonNasaWindDirection)((data[7]) >> 3);
                command.fanspeed = (NonNasaFanspeed)((data[7] & 0b00000111));
                command.mode = (NonNasaMode)(data[8] & 0b00111111);
                command.power = data[8] & 0b10000000;
                command.pipe_out = data[11] - 55;

                if (command.wind_direction == (NonNasaWindDirection)0)
                    command.wind_direction = NonNasaWindDirection::Stop;

                break;
            }

            case 0xA0:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x64:
            case 0x40:
                ESP_LOGW(TAG, "unknown command %02X", cmd);
                break;

            default:
                break;
            }

            return true;
        }

        std::vector<uint8_t> NonNasaPacket::encode()
        {
            std::vector<uint8_t> data;

            for (int i = 0; i < 14; i++)
            {
                data.push_back(0);
            }

            data[0] = 0x32;
            data[1] = (uint8_t)hex_to_int(src);
            data[2] = (uint8_t)hex_to_int(dst);

            data[3] = 0x20; // cmd

            data[4] = command.target_temp + 55;
            data[5] = command.room_temp + 55;
            data[6] = command.pipe_in + 55;

            data[7] = (uint8_t)command.wind_direction << 3;
            data[7] += (uint8_t)command.fanspeed;
            data[8] = (uint8_t)command.mode;
            data[8] += command.power ? 0b10000000 : 0;

            data[9] = (uint8_t)hex_to_int("1c");

            data[11] = command.pipe_out + 55;

            data[12] = build_checksum(data);
            data[13] = 0x34;

            return data;
        }

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            NonNasaPacket packet;
            if (!packet.decode(data))
                return;

            target->register_address(packet.src);
            target->set_target_temperature(packet.src, packet.command.target_temp);
            target->set_room_temperature(packet.src, packet.command.room_temp);
            target->set_power(packet.src, packet.command.power);
        }
    } // namespace samsung_ac
} // namespace esphome
