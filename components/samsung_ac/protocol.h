#pragma once

#include <vector>
#include <iostream>

namespace esphome
{
    namespace samsung_ac
    {
        class Protocol
        {
        public:
            virtual std::vector<uint8_t> get_power_message(const std::string &address, bool value) = 0;
        };

        class MessageTarget
        {
        public:
            virtual void register_address(const std::string address) = 0;
            virtual void set_power(const std::string address, bool value) = 0;
            virtual void set_room_temperature(const std::string address, float value) = 0;
        };

        void process_message(std::vector<uint8_t> &data, MessageTarget *target);

        Protocol *get_protocol(const std::string &address);

    } // namespace samsung_ac
} // namespace esphome