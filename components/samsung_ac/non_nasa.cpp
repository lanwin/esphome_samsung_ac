#include <queue>
#include <cmath>
#include <iostream>
#include "esphome/core/log.h"
#include "util.h"
#include "non_nasa.h"

static const char *TAG = "samsung_non_nasa";

namespace esphome
{
    namespace samsung_ac
    {
        NonNasaDataPacket packet;

        uint8_t build_checksum(std::vector<uint8_t> &data)
        {
            uint8_t sum = data[1];
            for (uint8_t i = 2; i < 12; i++)
            {
                sum = sum ^ data[i];
            }
            return sum;
        }

        std::string NonNasaDataPacket::to_string()
        {
            std::string str;
            str += "{";
            str += "src:" + src + ";";
            str += "dst:" + dst + ";";
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

        bool NonNasaDataPacket::decode(std::vector<uint8_t> &data)
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
                target_temp = data[4] - 55;
                room_temp = data[5] - 55;
                pipe_in = data[6] - 55;
                wind_direction = (NonNasaWindDirection)((data[7]) >> 3);
                fanspeed = (NonNasaFanspeed)((data[7] & 0b00000111));
                mode = (NonNasaMode)(data[8] & 0b00111111);
                power = data[8] & 0b10000000;
                pipe_out = data[11] - 55;

                if (wind_direction == (NonNasaWindDirection)0)
                    wind_direction = NonNasaWindDirection::Stop;

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

        NonNasaRequest NonNasaDataPacket::toRequest()
        {
            NonNasaRequest request;
            request.power = power;
            request.target_temp = target_temp;
            request.fanspeed = fanspeed;
            request.mode = mode;
            return request;
        }

        uint8_t encode_request_mode(NonNasaMode value)
        {
            switch (value)
            {
            case NonNasaMode::Auto:
                return 0;
            case NonNasaMode::Cool:
                return 1;
            case NonNasaMode::Dry:
                return 2;
            case NonNasaMode::Fan:
                return 3;
            case NonNasaMode::Heat:
                return 4;
                // NORMALVENT: 7
                // EXCHANGEVENT: 15
                // AIRFRESH: 23
                // SLEEP: 31
                // AUTOVENT: 79

            default:
                return 0; // Auto
            }
        }

        uint8_t encode_request_fanspeed(NonNasaFanspeed value)
        {
            switch (value)
            {
            case NonNasaFanspeed::Auto:
                return 0;
            case NonNasaFanspeed::Low:
                return 64;
            case NonNasaFanspeed::Medium:
                return 128;
            case NonNasaFanspeed::Fresh:
            case NonNasaFanspeed::High:
                return 160;
            default:
                return 0; // Auto
            }
        }

        std::vector<uint8_t> NonNasaRequest::encode()
        {
            std::vector<uint8_t> data{
                0x32,                     // 00 start
                0xD0,                     // 01 src
                (uint8_t)hex_to_int(dst), // 02 dst
                0xB0,                     // 03 cmd
                0x1F,                     // 04
                0x04,                     // 05
                0,                        // 06 temp + fanmode
                0,                        // 07 operation mode
                0,                        // 08 power + individual mode
                0,                        // 09
                0,                        // 10
                0,                        // 11
                0,                        // 12 crc
                0x34                      // 13 end
            };

            // individual seems to deactivate the locale remotes with message "CENTRAL".
            // seems to be like a building management system.
            bool individual = false;

            data[6] = (target_temp & 31U) | encode_request_fanspeed(fanspeed);
            data[7] = (uint8_t)encode_request_mode(mode);
            data[8] = !power ? (uint8_t)0xC0 : (uint8_t)0xF0;
            data[8] |= (individual ? 6U : 4U);
            data[9] = (uint8_t)0x21;
            data[12] = build_checksum(data);

            return data;
        }

        std::vector<uint8_t> NonNasaProtocol::get_power_message(const std::string &address, bool value)
        {
            auto request = packet.toRequest();
            request.power = value;
            return request.encode();
        }

        std::vector<uint8_t> NonNasaProtocol::get_target_temp_message(const std::string &address, float value)
        {
            auto request = packet.toRequest();
            request.target_temp = value;
            return request.encode();
        }

        NonNasaMode mode_to_nonnasa_mode(Mode value)
        {
            switch (value)
            {
            case Mode::Auto:
                return NonNasaMode::Auto;
            case Mode::Cool:
                return NonNasaMode::Cool;
            case Mode::Dry:
                return NonNasaMode::Dry;
            case Mode::Fan:
                return NonNasaMode::Fan;
            case Mode::Heat:
                return NonNasaMode::Heat;
            default:
                return NonNasaMode::Auto;
            }
        }

        std::vector<uint8_t> NonNasaProtocol::get_mode_message(const std::string &address, Mode value)
        {
            auto request = packet.toRequest();
            request.mode = mode_to_nonnasa_mode(value);
            return request.encode();
        }

        NonNasaFanspeed fanmode_to_nonnasa_fanspeed(FanMode value)
        {
            switch (value)
            {
            case FanMode::Hight:
                return NonNasaFanspeed::High;
            case FanMode::Mid:
                return NonNasaFanspeed::Medium;
            case FanMode::Low:
                return NonNasaFanspeed::Low;
            case FanMode::Auto:
            default:
                return NonNasaFanspeed::Auto;
            }
        }

        std::vector<uint8_t> NonNasaProtocol::get_fanmode_message(const std::string &address, FanMode value)
        {
            auto request = packet.toRequest();
            request.fanspeed = fanmode_to_nonnasa_fanspeed(value);
            return request.encode();
        }

        Mode nonnasa_mode_to_mode(NonNasaMode value)
        {
            switch (value)
            {
            case NonNasaMode::Auto:
            case NonNasaMode::Auto_Heat:
                return Mode::Auto;
            case NonNasaMode::Cool:
                return Mode::Cool;
            case NonNasaMode::Dry:
                return Mode::Dry;
            case NonNasaMode::Fan:
                return Mode::Fan;
            case NonNasaMode::Heat:
                return Mode::Heat;
            default:
                return Mode::Auto;
            }
        }

        FanMode nonnasa_fanspeed_to_fanmode(NonNasaFanspeed fanspeed)
        {
            switch (fanspeed)
            {
            case NonNasaFanspeed::Fresh:
            case NonNasaFanspeed::High:
                return FanMode::Hight;
            case NonNasaFanspeed::Medium:
                return FanMode::Mid;
            case NonNasaFanspeed::Low:
                return FanMode::Low;
            default:
            case NonNasaFanspeed::Auto:
                return FanMode::Auto;
            }
        }

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            if (!packet.decode(data))
                return;

            target->register_address(packet.src);
            target->set_target_temperature(packet.src, packet.target_temp);
            target->set_room_temperature(packet.src, packet.room_temp);
            target->set_power(packet.src, packet.power);
            target->set_mode(packet.src, nonnasa_mode_to_mode(packet.mode));
            target->set_fanmode(packet.src, nonnasa_fanspeed_to_fanmode(packet.fanspeed));
        }
    } // namespace samsung_ac
} // namespace esphome
