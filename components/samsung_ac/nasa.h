#pragma once

#include <vector>
#include <iostream>
#include "protocol.h"

namespace esphome
{
    namespace samsung_ac
    {
        class Samsung_AC;

        void process_nasa_message(std::vector<uint8_t> data, MessageTarget *target);

        class NasaProtocol : public Protocol
        {
        public:
            NasaProtocol() = default;

            std::vector<uint8_t> get_power_message(const std::string &address, bool value) override;
        };

    } // namespace samsung_ac
} // namespace esphome
