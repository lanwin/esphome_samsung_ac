#pragma once

#include <vector>
#include <iostream>
#include "protocol.h"

namespace esphome
{
    namespace samsung_ac
    {
        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target);
    } // namespace samsung_ac
} // namespace esphome
