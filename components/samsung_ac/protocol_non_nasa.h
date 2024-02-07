#pragma once

#include <vector>
#include <iostream>
#include <optional>
#include "protocol.h"
#include "util.h"

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

        struct NonNasaCommand20
        {
            uint8_t target_temp = 0;
            uint8_t room_temp = 0;
            uint8_t pipe_in = 0;
            uint8_t pipe_out = 0;

            NonNasaFanspeed fanspeed = NonNasaFanspeed::Auto;
            NonNasaMode mode = NonNasaMode::Heat;
            NonNasaWindDirection wind_direction = NonNasaWindDirection::Stop;

            bool power = false;

            std::string to_string();
        };

        struct NonNasaCommandC6
        {
            bool control_status = false;
            std::string to_string()
            {
                return "control_status:" + std::to_string(control_status);
            };
        };

        struct NonNasaCommandF3
        {
            uint8_t inverter_max_frequency_hz = 0;
            float inverter_total_capacity_requirement_kw = 0;
            float inverter_current_a = 0;
            float inverter_voltage_v = 0;
            float inverter_power_w = 0;

            std::string to_string();
        };

        struct NonNasaCommandRaw
        {
            uint8_t length;
            uint8_t data[16 - 4 - 1];

            std::string to_string()
            {
                std::vector<uint8_t> vec(std::begin(data), std::begin(data) + length);
                return bytes_to_hex(vec);
            };
        };

        enum class NonNasaCommand : uint8_t
        {
            Cmd20 = 0x20,
            CmdC6 = 0xc6,
            CmdF3 = 0xf3,
            CmdF8 = 0xF8,
        };

        struct NonNasaDataPacket
        {
            std::string src;
            std::string dst;

            NonNasaCommand cmd;

            NonNasaDataPacket()
            {
            }

            union
            {
                NonNasaCommand20 command20;
                NonNasaCommandC6 commandC6;
                NonNasaCommandF3 commandF3;
                NonNasaCommandRaw commandF8; // Unknown structure for now
                NonNasaCommandRaw commandRaw;
            };

            DecodeResult decode(std::vector<uint8_t> &data);
            std::string to_string();
        };

        struct NonNasaRequest
        {
            std::string dst;

            uint8_t room_temp = 0;
            uint8_t target_temp = 0;
            NonNasaFanspeed fanspeed = NonNasaFanspeed::Auto;
            NonNasaMode mode = NonNasaMode::Heat;
            bool power = false;

            std::vector<uint8_t> encode();
            std::string to_string();

            static NonNasaRequest create(std::string dst_address);
        };

        DecodeResult try_decode_non_nasa_packet(std::vector<uint8_t> data);
        void process_non_nasa_packet(MessageTarget *target);

        class NonNasaProtocol : public Protocol
        {
        public:
            NonNasaProtocol() = default;

            void publish_request(MessageTarget *target, const std::string &address, ProtocolRequest &request) override;
            void publish_power_message(MessageTarget *target, const std::string &address, bool value) override;
            void publish_target_temp_message(MessageTarget *target, const std::string &address, float value) override;
            void publish_mode_message(MessageTarget *target, const std::string &address, Mode value) override;
            void publish_fanmode_message(MessageTarget *target, const std::string &address, FanMode value) override;
            void publish_altmode_message(MessageTarget *target, const std::string &address, AltMode value) override;
            void publish_swing_mode_message(MessageTarget *target, const std::string &address, SwingMode value) override;
        };
    } // namespace samsung_ac
} // namespace esphome
