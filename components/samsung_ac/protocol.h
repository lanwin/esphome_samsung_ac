#pragma once

#include <set>
#include "esphome/core/optional.h"
#include "util.h"

namespace esphome
{
    namespace samsung_ac
    {
        extern bool debug_log_raw_bytes;
        extern bool non_nasa_keepalive;
        extern bool debug_log_undefined_messages;
        extern bool debug_log_messages;

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

        enum class WaterHeaterMode
        {
            Unknown = -1,
            Eco = 0,
            Standard = 1,
            Power = 2,
            Force = 3,
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

        typedef std::string AltModeName;
        typedef uint8_t AltMode;

        struct AltModeDesc
        {
            AltModeName name;
            AltMode value;
        };

        enum class SwingMode : uint8_t
        {
            Fix = 0,
            Vertical = 1,
            Horizontal = 2,
            All = 3
        };

        enum class OutdoorOperationMode
        {
            OP_STOP = 0,
            OP_SAFETY = 1,
            OP_NORMAL = 2,
            OP_BALANCE = 3,
            OP_RECOVERY = 4,
            OP_DEICE = 5,
            OP_COMPDOWN = 6,
            OP_PROHIBIT = 7,
            OP_LINEJIG = 8,
            OP_PCBJIG = 9,
            OP_TEST = 10,
            OP_CHARGE = 11,
            OP_PUMPDOWN = 12,
            OP_PUMPOUT = 13,
            OP_VACCUM = 14,
            OP_CALORYJIG = 15,
            OP_PUMPDOWNSTOP = 16,
            OP_SUBSTOP = 17,
            OP_CHECKPIPE = 18,
            OP_CHECKREF = 19,
            OP_FPTJIG = 20,
            OP_NONSTOP_HEAT_COOL_CHANGE = 21,
            OP_AUTO_INSPECT = 22,
            OP_ELECTRIC_DISCHARGE = 23,
            OP_SPLIT_DEICE = 24,
            OP_INVETER_CHECK = 25,
            OP_NONSTOP_DEICE = 26,
            OP_REM_TEST = 27,
            OP_RATING = 28,
            OP_PC_TEST = 29,
            OP_PUMPDOWN_THERMOOFF = 30,
            OP_3PHASE_TEST = 31,
            OP_SMARTINSTALL_TEST = 32,
            OP_DEICE_PERFORMANCE_TEST = 33,
            OP_INVERTER_FAN_PBA_CHECK = 34,
            OP_AUTO_PIPE_PAIRING = 35,
            OP_AUTO_CHARGE = 36,
            OP_UNKNOWN = -1
        };

        class MessageTarget
        {
        public:
            virtual uint32_t get_miliseconds() = 0;
            virtual void publish_data(std::vector<uint8_t> &data) = 0;
            virtual void register_address(const std::string address) = 0;
            virtual void set_power(const std::string address, bool value) = 0;
            virtual void set_automatic_cleaning(const std::string address, bool value) = 0;
            virtual void set_water_heater_power(const std::string address, bool value) = 0;
            virtual void set_room_temperature(const std::string address, float value) = 0;
            virtual void set_target_temperature(const std::string address, float value) = 0;
            virtual void set_water_outlet_target(const std::string address, float value) = 0;
            virtual void set_outdoor_temperature(const std::string address, float value) = 0;
            virtual void set_indoor_eva_in_temperature(const std::string address, float value) = 0;
            virtual void set_indoor_eva_out_temperature(const std::string address, float value) = 0;
            virtual void set_target_water_temperature(const std::string address, float value) = 0;
            virtual void set_mode(const std::string address, Mode mode) = 0;
            virtual void set_water_heater_mode(const std::string address, WaterHeaterMode waterheatermode) = 0;
            virtual void set_fanmode(const std::string address, FanMode fanmode) = 0;
            virtual void set_altmode(const std::string address, AltMode altmode) = 0;
            virtual void set_swing_vertical(const std::string address, bool vertical) = 0;
            virtual void set_swing_horizontal(const std::string address, bool horizontal) = 0;
            virtual optional<std::set<uint16_t>> get_custom_sensors(const std::string address) = 0;
            virtual void set_custom_sensor(const std::string address, uint16_t message_number, float value) = 0;
            virtual void set_error_code(const std::string address, int error_code) = 0;
            virtual void set_outdoor_operation_mode(const std::string &address, OutdoorOperationMode outdooroperationmode) = 0;

        };

        struct ProtocolRequest
        {
        public:
            optional<bool> power;
            optional<bool> automatic_cleaning;
            optional<bool> water_heater_power;
            optional<Mode> mode;
            optional<WaterHeaterMode> waterheatermode;
            optional<float> target_temp;
            optional<float> water_outlet_target;
            optional<float> target_water_temp;
            optional<FanMode> fan_mode;
            optional<SwingMode> swing_mode;
            optional<AltMode> alt_mode;
        };

        class Protocol
        {
        public:
            virtual void publish_request(MessageTarget *target, const std::string &address, ProtocolRequest &request) = 0;
            virtual void protocol_update(MessageTarget *target) = 0;
        };

        enum class ProtocolProcessing
        {
            Auto = 0,
            NASA = 1,
            NonNASA = 2
        };

        extern ProtocolProcessing protocol_processing;

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

inline std::string outdoor_operation_mode_to_string(OutdoorOperationMode mode)
{
    switch (mode)
    {
    case OutdoorOperationMode::OP_STOP:
        return "OP_STOP: Outdoor unit is stopped.";
    case OutdoorOperationMode::OP_SAFETY:
        return "OP_SAFETY: Operating in safety mode.";
    case OutdoorOperationMode::OP_NORMAL:
        return "OP_NORMAL: Normal operation mode.";
    case OutdoorOperationMode::OP_BALANCE:
        return "OP_BALANCE: Balancing mode.";
    case OutdoorOperationMode::OP_RECOVERY:
        return "OP_RECOVERY: Recovery mode.";
    case OutdoorOperationMode::OP_DEICE:
        return "OP_DEICE: Defrost mode.";
    case OutdoorOperationMode::OP_COMPDOWN:
        return "OP_COMPDOWN: Compressor down mode.";
    case OutdoorOperationMode::OP_PROHIBIT:
        return "OP_PROHIBIT: Prohibited state.";
    case OutdoorOperationMode::OP_LINEJIG:
        return "OP_LINEJIG: Line jig mode.";
    case OutdoorOperationMode::OP_PCBJIG:
        return "OP_PCBJIG: PCB jig mode.";
    case OutdoorOperationMode::OP_TEST:
        return "OP_TEST: Test mode.";
    case OutdoorOperationMode::OP_CHARGE:
        return "OP_CHARGE: Charge mode.";
    case OutdoorOperationMode::OP_PUMPDOWN:
        return "OP_PUMPDOWN: Pump down mode.";
    case OutdoorOperationMode::OP_PUMPOUT:
        return "OP_PUMPOUT: Pump out mode.";
    case OutdoorOperationMode::OP_VACCUM:
        return "OP_VACCUM: Vacuum mode.";
    case OutdoorOperationMode::OP_CALORYJIG:
        return "OP_CALORYJIG: Calory jig mode.";
    case OutdoorOperationMode::OP_PUMPDOWNSTOP:
        return "OP_PUMPDOWNSTOP: Pump down stop mode.";
    case OutdoorOperationMode::OP_SUBSTOP:
        return "OP_SUBSTOP: Sub stop mode.";
    case OutdoorOperationMode::OP_CHECKPIPE:
        return "OP_CHECKPIPE: Pipe check mode.";
    case OutdoorOperationMode::OP_CHECKREF:
        return "OP_CHECKREF: Refrigerant check mode.";
    case OutdoorOperationMode::OP_FPTJIG:
        return "OP_FPTJIG: FPT jig mode.";
    case OutdoorOperationMode::OP_NONSTOP_HEAT_COOL_CHANGE:
        return "OP_NONSTOP_HEAT_COOL_CHANGE: Non-stop heat/cool change mode.";
    case OutdoorOperationMode::OP_AUTO_INSPECT:
        return "OP_AUTO_INSPECT: Auto inspection mode.";
    case OutdoorOperationMode::OP_ELECTRIC_DISCHARGE:
        return "OP_ELECTRIC_DISCHARGE: Electric discharge mode.";
    case OutdoorOperationMode::OP_SPLIT_DEICE:
        return "OP_SPLIT_DEICE: Split defrost mode.";
    case OutdoorOperationMode::OP_INVETER_CHECK:
        return "OP_INVETER_CHECK: Inverter check mode.";
    case OutdoorOperationMode::OP_NONSTOP_DEICE:
        return "OP_NONSTOP_DEICE: Non-stop defrost mode.";
    case OutdoorOperationMode::OP_REM_TEST:
        return "OP_REM_TEST: Remote test mode.";
    case OutdoorOperationMode::OP_RATING:
        return "OP_RATING: Rating mode.";
    case OutdoorOperationMode::OP_PC_TEST:
        return "OP_PC_TEST: PC test mode.";
    case OutdoorOperationMode::OP_PUMPDOWN_THERMOOFF:
        return "OP_PUMPDOWN_THERMOOFF: Thermo-off pump down mode.";
    case OutdoorOperationMode::OP_3PHASE_TEST:
        return "OP_3PHASE_TEST: Three-phase test mode.";
    case OutdoorOperationMode::OP_SMARTINSTALL_TEST:
        return "OP_SMARTINSTALL_TEST: Smart install test mode.";
    case OutdoorOperationMode::OP_DEICE_PERFORMANCE_TEST:
        return "OP_DEICE_PERFORMANCE_TEST: Defrost performance test mode.";
    case OutdoorOperationMode::OP_INVERTER_FAN_PBA_CHECK:
        return "OP_INVERTER_FAN_PBA_CHECK: Inverter fan PBA check mode.";
    case OutdoorOperationMode::OP_AUTO_PIPE_PAIRING:
        return "OP_AUTO_PIPE_PAIRING: Auto pipe pairing mode.";
    case OutdoorOperationMode::OP_AUTO_CHARGE:
        return "OP_AUTO_CHARGE: Automatic charge mode.";
    default:
        return "OP_UNKNOWN: Unknown operation mode.";
    }
}
