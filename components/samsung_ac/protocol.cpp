#include "esphome/core/log.h"
#include "protocol.h"
#include "util.h"
#include "protocol_nasa.h"
#include "protocol_non_nasa.h"

namespace esphome
{
    namespace samsung_ac
    {
        bool debug_log_packets = false;
        bool debug_log_raw_bytes = false;

        // This functions is designed to run after a new value was added
        // to the data vector. One by one.
        DataResult process_data(std::vector<uint8_t> &data, MessageTarget *target)
        {
            // NonNASA message?
            if (data.size() <= 14)
            {
                if (data[data.size() - 1] != 0x34 /*valid end byte*/)
                    return DataResult::Fill;

                const auto result = try_decode_non_nasa_packet(data);
                if (result == DecodeResult::SizeDidNotMatch || result == DecodeResult::UnexpectedSize)
                    return DataResult::Fill;

                if (debug_log_raw_bytes)
                {
                    ESP_LOGW(TAG, "RAW: %s", bytes_to_hex(data).c_str());
                }

                if (result == DecodeResult::InvalidStartByte)
                {
                    ESP_LOGW(TAG, "invalid start byte");
                    return DataResult::Clear;
                }
                else if (result == DecodeResult::InvalidEndByte)
                {
                    ESP_LOGW(TAG, "invalid end byte");
                    return DataResult::Clear;
                }
                else if (result == DecodeResult::CrcError)
                {
                    // is logge dwithin decoder
                    return DataResult::Clear;
                }

                process_non_nasa_packet(target);
                return DataResult::Clear;
            }

            // NASA message
            if (data.size() <= 1500)
            {
                const auto result = try_decode_nasa_packet(data);
                if (result == DecodeResult::SizeDidNotMatch || result == DecodeResult::UnexpectedSize)
                    return DataResult::Fill;

                if (debug_log_raw_bytes)
                {
                    ESP_LOGW(TAG, "RAW: %s", bytes_to_hex(data).c_str());
                }

                if (result == DecodeResult::InvalidStartByte)
                {
                    ESP_LOGW(TAG, "invalid start byte");
                    return DataResult::Clear;
                }
                else if (result == DecodeResult::InvalidEndByte)
                {
                    ESP_LOGW(TAG, "invalid end byte");
                    return DataResult::Clear;
                }
                else if (result == DecodeResult::CrcError)
                {
                    // is logge dwithin decoder
                    return DataResult::Clear;
                }

                process_nasa_packet(target);
                return DataResult::Clear;
            }

            if (debug_log_raw_bytes)
            {
                ESP_LOGW(TAG, "RAW: %s", bytes_to_hex(data).c_str());
            }
            // > 1500
            ESP_LOGW(TAG, "Current message exceeds the size limits");
            return DataResult::Clear;
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