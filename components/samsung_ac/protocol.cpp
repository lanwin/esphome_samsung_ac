#include "esphome/core/log.h"
#include "protocol.h"
#include "util.h"
#include "nasa.h"
#include "non_nasa.h"

namespace esphome
{
    namespace samsung_ac
    {
        bool debug_log_messages = false;
        bool debug_log_messages_raw = false;

        void process_message(std::vector<uint8_t> &data, MessageTarget *target)
        {
            if (debug_log_messages_raw)
            {
                ESP_LOGW("samsung_ac", "RAW: %s", bytes_to_hex(data).c_str());
            }

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

        AddressType get_address_type(const std::string &address)
        {
            if (address == "c8" || address.rfind("10.", 0) == 0)
                return AddressType::Outdoor;

            if (address == "00" || address == "01" || address == "02" || address == "03" || address.rfind("20.", 0) == 0)
                return AddressType::Indoor;

            return AddressType::Other;
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