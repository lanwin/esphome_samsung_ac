#include "esphome/core/log.h"
#include "protocol.h"
#include "util.h"
#include "nasa.h"
#include "non_nasa.h"

namespace esphome
{
    namespace samsung_ac
    {
        void process_message(std::vector<uint8_t> &data, MessageTarget *target)
        {
            if (data.size() == 14)
            {
                process_non_nasa_message(data, target);
                return;
            }
            if (data.size() >= 16 && data.size() < 1500)
            {
                process_nasa_message(data, target);
                return;
            }

            ESP_LOGW("samsung_ac", "Unknown message type %s", bytes_to_hex(data).c_str());
        }

        bool is_nasa_address(const std::string &address)
        {
            return address.size() != 2;
        }

        Protocol *nasaProtocol = new NasaProtocol();
        Protocol *nonNasaProtocol = new NonNasaProtocol();

        Protocol *get_protocol(const std::string &address)
        {
            if (!is_nasa_address(address))
                return nonNasaProtocol;

            return nasaProtocol;
        }
    } // namespace samsung_ac
} // namespace esphome