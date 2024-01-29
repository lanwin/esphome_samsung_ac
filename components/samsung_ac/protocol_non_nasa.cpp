#include <queue>
#include <map>
#include <cmath>
#include <iostream>
#include "esphome/core/log.h"
#include "util.h"
#include "protocol_non_nasa.h"

std::map<std::string, esphome::samsung_ac::NonNasaCommand20> last_command20s_;

esphome::samsung_ac::NonNasaDataPacket nonpacket_;

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
            str += "target_temp:" + std::to_string(target_temp) + ";";
            str += "room_temp:" + std::to_string(room_temp) + ";";
            str += "pipe_in:" + std::to_string(pipe_in) + ";";
            str += "pipe_out:" + std::to_string(pipe_out) + ";";
            str += "power:" + std::to_string(power ? 1 : 0) + ";";
            str += "wind_direction:" + std::to_string((uint8_t)wind_direction) + ";";
            str += "fanspeed:" + std::to_string((uint8_t)fanspeed) + ";";
            str += "mode:" + long_to_hex((uint8_t)mode) + ";";
            return str;
        }

        std::string NonNasaDataPacket::to_string()
        {
            std::string str;
            str += "{";
            str += "src:" + src + ";";
            str += "dst:" + dst + ";";
            str += "cmd:" + long_to_hex(cmd) + ";";
            if (cmd == 0x20)
                str += "command20:{" + command20.to_string() + "}";
            str += "}";
            return str;
        }

        DecodeResult NonNasaDataPacket::decode(std::vector<uint8_t> &data)
        {
            if (data[0] != 0x32)
                return DecodeResult::InvalidStartByte;

            if (data[data.size() - 1] != 0x34)
                return DecodeResult::InvalidEndByte;

            if (data.size() != 7 && data.size() != 14)
                return DecodeResult::UnexpectedSize;

            auto crc_expected = build_checksum(data);
            auto crc_actual = data[data.size() - 2];
            if (crc_actual != build_checksum(data))
            {
                ESP_LOGW(TAG, "NonNASA: invalid crc - got %d but should be %d: %s", crc_actual, crc_expected, bytes_to_hex(data).c_str());
                return DecodeResult::CrcError;
            }

            src = long_to_hex(data[1]);
            dst = long_to_hex(data[2]);

            cmd = data[3];
            switch (cmd)
            {
            case 0x20: // temperatures
            {
                command20.target_temp = data[4] - 55;
                command20.room_temp = data[5] - 55;
                command20.pipe_in = data[6] - 55;
                command20.wind_direction = (NonNasaWindDirection)((data[7]) >> 3);
                command20.fanspeed = (NonNasaFanspeed)((data[7] & 0b00000111));
                command20.mode = (NonNasaMode)(data[8] & 0b00111111);
                command20.power = data[8] & 0b10000000;
                command20.pipe_out = data[11] - 55;

                if (command20.wind_direction == (NonNasaWindDirection)0)
                    command20.wind_direction = NonNasaWindDirection::Stop;

                return DecodeResult::Ok;
            }
            case 0xc6:
            {
                // makes only sens src == "c8" && dst == "d0"
                commandC6.control_status = data[4];
                return DecodeResult::Ok;
            }
            default:
                return DecodeResult::Ok;
            }
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
                0x1F,                     // 04 ?
                0x04,                     // 05 ?
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

            if (room_temp > 0)
                data[5] = room_temp;
            data[6] = (target_temp & 31U) | encode_request_fanspeed(fanspeed);
            data[7] = (uint8_t)encode_request_mode(mode);
            data[8] = !power ? (uint8_t)0xC0 : (uint8_t)0xF0;
            data[8] |= (individual ? 6U : 4U);
            data[9] = (uint8_t)0x21;
            data[12] = build_checksum(data);

            data[9] = (uint8_t)0x21;

            return data;
        }

        NonNasaRequest NonNasaRequest::create(std::string dst_address)
        {
            NonNasaRequest request;
            request.dst = dst_address;

            auto last_command20_ = last_command20s_[dst_address];
            request.room_temp = last_command20_.room_temp;
            request.power = last_command20_.power;
            request.target_temp = last_command20_.target_temp;
            request.fanspeed = last_command20_.fanspeed;
            request.mode = last_command20_.mode;

            return request;
        }

        std::queue<NonNasaRequest> nonnasa_requests;

        void NonNasaProtocol::publish_power_message(MessageTarget *target, const std::string &address, bool value)
        {
            auto request = NonNasaRequest::create(address);
            request.power = value;
            nonnasa_requests.push(request);
        }

        void NonNasaProtocol::publish_target_temp_message(MessageTarget *target, const std::string &address, float value)
        {
            auto request = NonNasaRequest::create(address);
            request.target_temp = value;
            nonnasa_requests.push(request);
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

        void NonNasaProtocol::publish_mode_message(MessageTarget *target, const std::string &address, Mode value)
        {
            auto request = NonNasaRequest::create(address);
            request.mode = mode_to_nonnasa_mode(value);
            nonnasa_requests.push(request);
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

        void NonNasaProtocol::publish_fanmode_message(MessageTarget *target, const std::string &address, FanMode value)
        {
            auto request = NonNasaRequest::create(address);
            request.fanspeed = fanmode_to_nonnasa_fanspeed(value);
            nonnasa_requests.push(request);
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

        DecodeResult try_decode_non_nasa_packet(std::vector<uint8_t> data)
        {
            return nonpacket_.decode(data);
        }

        void process_non_nasa_packet(MessageTarget *target)
        {
            if (debug_log_packets)
            {
                ESP_LOGW(TAG, "MSG: %s", nonpacket_.to_string().c_str());
            }

            target->register_address(nonpacket_.src);

            if (nonpacket_.cmd == 0x20)
            {
                last_command20s_[nonpacket_.src] = nonpacket_.command20;
                target->set_target_temperature(nonpacket_.src, nonpacket_.command20.target_temp);
                target->set_room_temperature(nonpacket_.src, nonpacket_.command20.room_temp);
                target->set_power(nonpacket_.src, nonpacket_.command20.power);
                target->set_mode(nonpacket_.src, nonnasa_mode_to_mode(nonpacket_.command20.mode));
                target->set_fanmode(nonpacket_.src, nonnasa_fanspeed_to_fanmode(nonpacket_.command20.fanspeed));
            }
            else if (nonpacket_.cmd == 0xc6)
            {
                if (nonpacket_.src == "c8" && nonpacket_.dst == "d0")
                {
                    ESP_LOGW(TAG, "control_status=%d", nonpacket_.commandC6.control_status);

                    while (nonnasa_requests.size() > 0)
                    {
                        auto data = nonnasa_requests.front().encode();
                        target->publish_data(data);
                        nonnasa_requests.pop();
                    }
                }
            }
        }
    } // namespace samsung_ac
} // namespace esphome
