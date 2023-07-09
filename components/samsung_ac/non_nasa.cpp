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

        void process_non_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            ESP_LOGW(TAG, "%s", bytes_to_hex(data).c_str());
        }
    } // namespace samsung_ac
} // namespace esphome
