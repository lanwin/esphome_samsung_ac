#pragma once

#include <vector>
#include <iostream>
#include "protocol.h"

namespace esphome
{
    namespace samsung_ac
    {
        enum class NonNasaFanspeed : uint8_t
        {
            Auto = 0,
            Low = 2,
            Medium = 4,
            High = 5,
            Fresh = 6
        };

        enum class NonNasaMode : uint8_t
        {
            Heat = 0x01,
            Cool = 0x02,
            Dry = 0x04,
            Fan = 0x08,
            Auto_Heat = 0x21,
            Auto = 0x22
        };

        enum class NonNasaWindDirection : uint8_t
        {
            Vertical = 26,
            Horizontal = 27,
            FourWay = 28,
            Stop = 31
        };

        struct NonNasaRequest
        {
            std::string dst;

            uint8_t target_temp = 0;
            NonNasaFanspeed fanspeed = NonNasaFanspeed::Auto;
            NonNasaMode mode = NonNasaMode::Heat;
            bool power = false;

            std::vector<uint8_t> encode();
            std::string to_string();
        };

        struct NonNasaDataPacket // cmd 20
        {
            std::string src;
            std::string dst;

            uint8_t target_temp = 0;
            uint8_t room_temp = 0;
            uint8_t pipe_in = 0;
            uint8_t pipe_out = 0;

            NonNasaFanspeed fanspeed = NonNasaFanspeed::Auto;
            NonNasaMode mode = NonNasaMode::Heat;
            NonNasaWindDirection wind_direction = NonNasaWindDirection::Stop;

            bool power = false;

            bool decode(std::vector<uint8_t> &data);
            std::string to_string();
            NonNasaRequest toRequest();
        };

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target);

        class NonNasaProtocol : public Protocol
        {
        public:
            NonNasaProtocol() = default;

            std::vector<uint8_t> get_power_message(const std::string &address, bool value) override;
            std::vector<uint8_t> get_target_temp_message(const std::string &address, float value) override;
            std::vector<uint8_t> get_mode_message(const std::string &address, Mode value) override;
            std::vector<uint8_t> get_fanmode_message(const std::string &address, FanMode value) override;
        };
    } // namespace samsung_ac
} // namespace esphome
