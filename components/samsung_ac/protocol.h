#pragma once

#include "esphome/core/optional.h"
#include "util.h"

namespace esphome
{
    namespace samsung_ac
    {
        extern bool debug_log_packets;
        extern bool debug_log_raw_bytes;

        enum class DecodeResult
        {
            Ok = 0,
            InvalidStartByte = 1,
            InvalidEndByte = 2,
            SizeDidNotMatch = 3,
            UnexpectedSize = 4,
            CrcError = 5
        };

        enum class Mode
        {
            Unknown = -1,
            Auto = 0,
            Cool = 1,
            Dry = 2,
            Fan = 3,
            Heat = 4,
        };

        enum class FanMode
        {
            Unknown = -1,
            Auto = 0,
            Low = 1,
            Mid = 2,
            High = 3,
            Turbo = 4,
            Off = 5
        };

        enum class AltMode
        {
            Unknown = -1,
            None = 0,
            Sleep = 1,
            Quiet = 2,
            Fast = 3,
            LongReach = 4,
            Windfree = 5
        };

        enum class SwingMode : uint8_t
        {
            Fix = 0,
            Vertical = 1,
            Horizontal = 2,
            All = 3
        };

        class MessageTarget
        {
        public:
            virtual uint32_t get_miliseconds() = 0;
            virtual void publish_data(std::vector<uint8_t> &data) = 0;
            virtual void register_address(const std::string address) = 0;
            virtual void set_power(const std::string address, bool value) = 0;
            virtual void set_room_temperature(const std::string address, float value) = 0;
            virtual void set_water_temperature(const std::string address, float value) = 0;
            virtual void set_room_humidity(const std::string address, float value) = 0;
            virtual void set_target_temperature(const std::string address, float value) = 0;
            virtual void set_target_water_temperature(const std::string address, float value) = 0;
            virtual void set_mode(const std::string address, Mode mode) = 0;
            virtual void set_fanmode(const std::string address, FanMode fanmode) = 0;
            virtual void set_altmode(const std::string address, AltMode fanmode) = 0;
            virtual void set_swing_vertical(const std::string address, bool vertical) = 0;
            virtual void set_swing_horizontal(const std::string address, bool horizontal) = 0;
        };

        struct ProtocolRequest
        {
        public:
            optional<bool> power;
            optional<Mode> mode;
            optional<float> target_temp;
            optional<float> target_water_temp;
            optional<FanMode> fan_mode;
            optional<SwingMode> swing_mode;
            optional<AltMode> alt_mode;
        };

        class Protocol
        {
        public:
            virtual void publish_request(MessageTarget *target, const std::string &address, ProtocolRequest &request) = 0;
        };

        enum class DataResult
        {
            Fill = 0,
            Clear = 1
        };

        DataResult process_data(std::vector<uint8_t> &data, MessageTarget *target);

        Protocol *get_protocol(const std::string &address);

        bool is_nasa_address(const std::string &address);

        enum class AddressType
        {
            Outdoor = 0,
            Indoor = 1,
            Other = 2
        };

        AddressType get_address_type(const std::string &address);

    } // namespace samsung_ac
} // namespace esphome
