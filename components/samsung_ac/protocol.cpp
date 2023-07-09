#include "esphome/core/log.h"
#include "protocol.h"
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

            // log error
        }

        Protocol *nasaProtocol = new NasaProtocol();

        Protocol *get_protocol(const std::string &address)
        {
            // if(address.size()==2)return nonNasaProtocol;

            return nasaProtocol;
        }

    } // namespace samsung_ac
} // namespace esphome