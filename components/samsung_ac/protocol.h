#pragma once

#include <vector>
#include <iostream>

namespace esphome
{
    namespace samsung_ac
    {
        enum class Mode
        {
            Unknown = -1,
            Auto = 0,
            Cool = 1,
            Dry = 2,
            Fan = 3,
            Heat = 4,
        };

        class Protocol
        {
        public:
            virtual std::vector<uint8_t> get_power_message(const std::string &address, bool value) = 0;
            virtual std::vector<uint8_t> get_target_temp_message(const std::string &address, float value) = 0;
            virtual std::vector<uint8_t> get_mode_message(const std::string &address, Mode value) = 0;
        };

        class MessageTarget
        {
        public:
            virtual void register_address(const std::string address) = 0;
            virtual void set_power(const std::string address, bool value) = 0;
            virtual void set_room_temperature(const std::string address, float value) = 0;
            virtual void set_target_temperature(const std::string address, float value) = 0;
            virtual void set_mode(const std::string address, Mode mode) = 0;
        };

        void process_message(std::vector<uint8_t> &data, MessageTarget *target);

        Protocol *get_protocol(const std::string &address);

    } // namespace samsung_ac
} // namespace esphome