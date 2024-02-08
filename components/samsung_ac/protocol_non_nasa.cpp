#include <queue>
#include <map>
#include <cmath>
#include <iostream>
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "util.h"
#include "protocol_non_nasa.h"

std::map<std::string, esphome::samsung_ac::NonNasaCommand20> last_command20s_;

esphome::samsung_ac::NonNasaDataPacket nonpacket_;

namespace esphome
{
    namespace samsung_ac
    {
        uint8_t build_checksum(std::vector<uint8_t> &data)
        {
            uint8_t sum = data[1];
            for (uint8_t i = 2; i < 12; i++)
            {
                sum = sum ^ data[i];
            }
            return sum;
        }

        std::string NonNasaCommand20::to_string()
        {
            std::string str;
            str += "target_temp:" + std::to_string(target_temp) + "; ";
            str += "room_temp:" + std::to_string(room_temp) + "; ";
            str += "pipe_in:" + std::to_string(pipe_in) + "; ";
            str += "pipe_out:" + std::to_string(pipe_out) + "; ";
            str += "power:" + std::to_string(power ? 1 : 0) + "; ";
            str += "wind_direction:" + std::to_string((uint8_t)wind_direction) + "; ";
            str += "fanspeed:" + std::to_string((uint8_t)fanspeed) + "; ";
            str += "mode:" + long_to_hex((uint8_t)mode);
            return str;
        }

        std::string NonNasaCommandC0::to_string()
        {
            std::string str;
            str += "ou_operation_mode:" + long_to_hex((uint8_t)outdoor_unit_operation_mode) + "; ";
            str += "ou_4way_valve:" + std::to_string(outdoor_unit_4_way_valve ? 1 : 0) + "; ";
            str += "ou_hot_gas_bypass:" + std::to_string(outdoor_unit_hot_gas_bypass ? 1 : 0) + "; ";
            str += "ou_compressor:" + std::to_string(outdoor_unit_compressor ? 1 : 0) + "; ";
            str += "ou_ac_fan:" + std::to_string(outdoor_unit_ac_fan ? 1 : 0) + "; ";
            str += "ou_outdoor_temp[°C]:" + std::to_string(outdoor_unit_outdoor_temp_c) + "; ";
            str += "ou_discharge_temp[°C]:" + std::to_string(outdoor_unit_discharge_temp_c) + "; ";
            str += "ou_condenser_mid_temp[°C]:" + std::to_string(outdoor_unit_condenser_mid_temp_c);
            return str;
        }

        std::string NonNasaCommandC1::to_string()
        {
            std::string str;
            str += "ou_sump_temp[°C]:" + std::to_string(outdoor_unit_sump_temp_c);
            return str;
        }

        std::string NonNasaCommandF0::to_string()
        {
            std::string str;
            str += "ou_freeze_protection:" + std::to_string(outdoor_unit_freeze_protection ? 1 : 0) + "; ";
            str += "ou_heating_overload:" + std::to_string(outdoor_unit_heating_overload ? 1 : 0) + "; ";
            str += "ou_defrost_control:" + std::to_string(outdoor_unit_defrost_control ? 1 : 0) + "; ";
            str += "ou_discharge_protection:" + std::to_string(outdoor_unit_discharge_protection ? 1 : 0) + "; ";
            str += "ou_current_control:" + std::to_string(outdoor_unit_current_control ? 1 : 0) + "; ";
            str += "inverter_order_frequency[Hz]:" + std::to_string(inverter_order_frequency_hz) + "; ";
            str += "inverter_target_frequency[Hz]:" + std::to_string(inverter_target_frequency_hz) + "; ";
            str += "inverter_current_frequency[Hz]:" + std::to_string(inverter_current_frequency_hz) + "; ";
            str += "ou_bldc_fan:" + std::to_string(outdoor_unit_bldc_fan ? 1 : 0) + "; ";
            str += "ou_error_code:" + long_to_hex((uint8_t)outdoor_unit_error_code);
            return str;
        }

        std::string NonNasaCommandF1::to_string()
        {
            std::string str;
            str += "Electronic Expansion Valves: ";
            str += "EEV_A:" + std::to_string(outdoor_unit_EEV_A) + "; ";
            str += "EEV_B:" + std::to_string(outdoor_unit_EEV_B) + "; ";
            str += "EEV_C:" + std::to_string(outdoor_unit_EEV_C) + "; ";
            str += "EEV_D:" + std::to_string(outdoor_unit_EEV_D);
            return str;
        }

        std::string NonNasaCommandF3::to_string()
        {
            std::string str;
            str += "inverter_max_frequency[Hz]:" + std::to_string(inverter_max_frequency_hz) + "; ";
            str += "inverter_total_capacity_requirement[kW]:" + std::to_string(inverter_total_capacity_requirement_kw) + "; ";
            str += "inverter_current[ADC]:" + std::to_string(inverter_current_a) + "; ";
            str += "inverter_voltage[VDC]:" + std::to_string(inverter_voltage_v) + "; ";
            str += "inverter_power[W]:" + std::to_string(inverter_power_w);
            return str;
        }

        std::string NonNasaDataPacket::to_string()
        {
            std::string str;
            str += "{";
            str += "src:" + src + ";";
            str += "dst:" + dst + ";";
            str += "cmd:" + long_to_hex((uint8_t)cmd) + ";";
            switch (cmd)
            {
            case NonNasaCommand::Cmd20:
            {
                str += "command20:{" + command20.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdC0:
            {
                str += "commandC0:{" + commandC0.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdC1:
            {
                str += "commandC1:{" + commandC1.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdC6:
            {
                str += "commandC6:{" + commandC6.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdF0:
            {
                str += "commandF0:{" + commandF0.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdF1:
            {
                str += "commandF1:{" + commandF1.to_string() + "}";
                break;
            }
            case NonNasaCommand::CmdF3:
            {
                str += "commandF3:{" + commandF3.to_string() + "}";
                break;
            }
            default:
            {
                str += "raw:" + commandRaw.to_string();
                break;
            }
            }

            str += "}";
            return str;
        }

        DecodeResult NonNasaDataPacket::decode(std::vector<uint8_t> &data)
        {
            if (data[0] != 0x32)
                return DecodeResult::InvalidStartByte;

            if (data[data.size() - 1] != 0x34)
                return DecodeResult::InvalidEndByte;

            if (data.size() != 7 && data.size() != 14)
                return DecodeResult::UnexpectedSize;

            auto crc_expected = build_checksum(data);
            auto crc_actual = data[data.size() - 2];
            if (crc_actual != build_checksum(data))
            {
                ESP_LOGW(TAG, "NonNASA: invalid crc - got %d but should be %d: %s", crc_actual, crc_expected, bytes_to_hex(data).c_str());
                return DecodeResult::CrcError;
            }

            src = long_to_hex(data[1]);
            dst = long_to_hex(data[2]);

            cmd = (NonNasaCommand)data[3];
            switch (cmd)
            {
            case NonNasaCommand::Cmd20: // temperatures
            {
                command20.target_temp = data[4] - 55;
                command20.room_temp = data[5] - 55;
                command20.pipe_in = data[6] - 55;
                command20.wind_direction = (NonNasaWindDirection)((data[7]) >> 3);
                command20.fanspeed = (NonNasaFanspeed)((data[7] & 0b00000111));
                command20.mode = (NonNasaMode)(data[8] & 0b00111111);
                command20.power = data[8] & 0b10000000;
                command20.pipe_out = data[11] - 55;

                if (command20.wind_direction == (NonNasaWindDirection)0)
                    command20.wind_direction = NonNasaWindDirection::Stop;

                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdC0: // outdoor unit data
            {
                commandC0.outdoor_unit_operation_mode = data[4]; // modes need to be specified
                commandC0.outdoor_unit_4_way_valve = data[6] & 0b10000000;
                commandC0.outdoor_unit_hot_gas_bypass = data[6] & 0b00100000;
                commandC0.outdoor_unit_compressor = data[6] & 0b00000100;
                commandC0.outdoor_unit_ac_fan = data[7] & 0b00000011;
                commandC0.outdoor_unit_outdoor_temp_c = data[8] - 55;
                commandC0.outdoor_unit_discharge_temp_c = data[10] - 55;
                commandC0.outdoor_unit_condenser_mid_temp_c = data[11] - 55;
                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdC1: // outdoor unit data
            {
                commandC1.outdoor_unit_sump_temp_c = data[8] - 55;
                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdC6:
            {
                commandC6.control_status = data[4];
                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdF0: // outdoor unit data
            {
                commandF0.outdoor_unit_freeze_protection = data[4] & 0b10000000;
                commandF0.outdoor_unit_heating_overload = data[4] & 0b01000000;
                commandF0.outdoor_unit_defrost_control = data[4] & 0b00100000;
                commandF0.outdoor_unit_discharge_protection = data[4] & 0b00010000;
                commandF0.outdoor_unit_current_control = data[4] & 0b00001000;
                commandF0.inverter_order_frequency_hz = data[5];
                commandF0.inverter_target_frequency_hz = data[6];
                commandF0.inverter_current_frequency_hz = data[7];
                commandF0.outdoor_unit_bldc_fan = data[8] & 0b00000011; // not sure if correct, i have no ou with BLDC-fan
                commandF0.outdoor_unit_error_code = data[10];
                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdF1: // outdoor unit eev-values
            {
                commandF1.outdoor_unit_EEV_A = (data[4] * 256) + data[5];
                commandF1.outdoor_unit_EEV_B = (data[6] * 256) + data[7];
                commandF1.outdoor_unit_EEV_C = (data[8] * 256) + data[9];
                commandF1.outdoor_unit_EEV_D = (data[10] * 256) + data[11];
                return DecodeResult::Ok;
            }
            case NonNasaCommand::CmdF3: // power consumption
            {
                // Maximum frequency for Inverter (compressor-motor of outdoor-unit) in Hz
                commandF3.inverter_max_frequency_hz = data[4];
                // Sum of required heating/cooling capacity ordered by the indoor-units in kW
                commandF3.inverter_total_capacity_requirement_kw = (float)data[5] / 10;
                // DC-current to the inverter of outdoor-unit in A
                commandF3.inverter_current_a = (float)data[8] / 10;
                // voltage of the DC-link to inverter in V
                commandF3.inverter_voltage_v = (float)data[9] * 2;
                // Power consumption of the outdoo unit inverter in W
                commandF3.inverter_power_w = commandF3.inverter_current_a * commandF3.inverter_voltage_v;
                return DecodeResult::Ok;
            }
            default:
            {
                commandRaw.length = data.size() - 4 - 1;
                auto begin = data.begin() + 4;
                std::copy(begin, begin + commandRaw.length, commandRaw.data);
                return DecodeResult::Ok;
            }
            }
        }

        uint8_t encode_request_mode(NonNasaMode value)
        {
            switch (value)
            {
            case NonNasaMode::Auto:
                return 0;
            case NonNasaMode::Cool:
                return 1;
            case NonNasaMode::Dry:
                return 2;
            case NonNasaMode::Fan:
                return 3;
            case NonNasaMode::Heat:
                return 4;
                // NORMALVENT: 7
                // EXCHANGEVENT: 15
                // AIRFRESH: 23
                // SLEEP: 31
                // AUTOVENT: 79

            default:
                return 0; // Auto
            }
        }

        uint8_t encode_request_fanspeed(NonNasaFanspeed value)
        {
            switch (value)
            {
            case NonNasaFanspeed::Auto:
                return 0;
            case NonNasaFanspeed::Low:
                return 64;
            case NonNasaFanspeed::Medium:
                return 128;
            case NonNasaFanspeed::Fresh:
            case NonNasaFanspeed::High:
                return 160;
            default:
                return 0; // Auto
            }
        }

        std::vector<uint8_t> NonNasaRequest::encode()
        {
            std::vector<uint8_t> data{
                0x32,                     // 00 start
                0xD0,                     // 01 src
                (uint8_t)hex_to_int(dst), // 02 dst
                0xB0,                     // 03 cmd
                0x1F,                     // 04 ?
                0x04,                     // 05 ?
                0,                        // 06 temp + fanmode
                0,                        // 07 operation mode
                0,                        // 08 power + individual mode
                0,                        // 09
                0,                        // 10
                0,                        // 11
                0,                        // 12 crc
                0x34                      // 13 end
            };

            // individual seems to deactivate the locale remotes with message "CENTRAL".
            // seems to be like a building management system.
            bool individual = false;

            if (room_temp > 0)
                data[5] = room_temp;
            data[6] = (target_temp & 31U) | encode_request_fanspeed(fanspeed);
            data[7] = (uint8_t)encode_request_mode(mode);
            data[8] = !power ? (uint8_t)0xC0 : (uint8_t)0xF0;
            data[8] |= (individual ? 6U : 4U);
            data[9] = (uint8_t)0x21;
            data[12] = build_checksum(data);

            data[9] = (uint8_t)0x21;

            return data;
        }

        NonNasaRequest NonNasaRequest::create(std::string dst_address)
        {
            NonNasaRequest request;
            request.dst = dst_address;

            auto last_command20_ = last_command20s_[dst_address];
            request.room_temp = last_command20_.room_temp;
            request.power = last_command20_.power;
            request.target_temp = last_command20_.target_temp;
            request.fanspeed = last_command20_.fanspeed;
            request.mode = last_command20_.mode;

            return request;
        }

        std::queue<NonNasaRequest> nonnasa_requests;

        void NonNasaProtocol::publish_power_message(MessageTarget *target, const std::string &address, bool value)
        {
            auto request = NonNasaRequest::create(address);
            request.power = value;
            nonnasa_requests.push(request);
        }

        void NonNasaProtocol::publish_target_temp_message(MessageTarget *target, const std::string &address, float value)
        {
            auto request = NonNasaRequest::create(address);
            request.target_temp = value;
            nonnasa_requests.push(request);
        }

        NonNasaMode mode_to_nonnasa_mode(Mode value)
        {
            switch (value)
            {
            case Mode::Auto:
                return NonNasaMode::Auto;
            case Mode::Cool:
                return NonNasaMode::Cool;
            case Mode::Dry:
                return NonNasaMode::Dry;
            case Mode::Fan:
                return NonNasaMode::Fan;
            case Mode::Heat:
                return NonNasaMode::Heat;
            default:
                return NonNasaMode::Auto;
            }
        }

        void NonNasaProtocol::publish_mode_message(MessageTarget *target, const std::string &address, Mode value)
        {
            auto request = NonNasaRequest::create(address);
            request.power = true;
            request.mode = mode_to_nonnasa_mode(value);
            nonnasa_requests.push(request);
        }

        NonNasaFanspeed fanmode_to_nonnasa_fanspeed(FanMode value)
        {
            switch (value)
            {
            case FanMode::High:
                return NonNasaFanspeed::High;
            case FanMode::Mid:
                return NonNasaFanspeed::Medium;
            case FanMode::Low:
                return NonNasaFanspeed::Low;
            case FanMode::Auto:
            default:
                return NonNasaFanspeed::Auto;
            }
        }

        void NonNasaProtocol::publish_fanmode_message(MessageTarget *target, const std::string &address, FanMode value)
        {
            auto request = NonNasaRequest::create(address);
            request.fanspeed = fanmode_to_nonnasa_fanspeed(value);
            nonnasa_requests.push(request);
        }

        void NonNasaProtocol::publish_altmode_message(MessageTarget *target, const std::string &address, AltMode value)
        {
            ESP_LOGW(TAG, "change altmode is currently not implemented");
        }

        void NonNasaProtocol::publish_swing_mode_message(MessageTarget *target, const std::string &address, SwingMode value)
        {
            ESP_LOGW(TAG, "change swingmode is currently not implemented");
        }

        void NonNasaProtocol::publish_request(MessageTarget *target, const std::string &address, ProtocolRequest &request)
        {
            auto req = NonNasaRequest::create(address);

            if (request.mode)
            {
                request.power = true; // ensure system turns on when mode is set
                req.mode = mode_to_nonnasa_mode(request.mode.value());
            }

            if (request.power)
                req.power = request.power.value();

            nonnasa_requests.push(req);
        }

        Mode nonnasa_mode_to_mode(NonNasaMode value)
        {
            switch (value)
            {
            case NonNasaMode::Auto:
            case NonNasaMode::Auto_Heat:
                return Mode::Auto;
            case NonNasaMode::Cool:
                return Mode::Cool;
            case NonNasaMode::Dry:
                return Mode::Dry;
            case NonNasaMode::Fan:
                return Mode::Fan;
            case NonNasaMode::Heat:
                return Mode::Heat;
            default:
                return Mode::Auto;
            }
        }

        FanMode nonnasa_fanspeed_to_fanmode(NonNasaFanspeed fanspeed)
        {
            switch (fanspeed)
            {
            case NonNasaFanspeed::Fresh:
            case NonNasaFanspeed::High:
                return FanMode::High;
            case NonNasaFanspeed::Medium:
                return FanMode::Mid;
            case NonNasaFanspeed::Low:
                return FanMode::Low;
            default:
            case NonNasaFanspeed::Auto:
                return FanMode::Auto;
            }
        }

        DecodeResult try_decode_non_nasa_packet(std::vector<uint8_t> data)
        {
            return nonpacket_.decode(data);
        }

        void send_requests(MessageTarget *target, uint8_t delay_ms)
        {
            while (nonnasa_requests.size() > 0)
            {
                delay(delay_ms);
                auto data = nonnasa_requests.front().encode();
                target->publish_data(data);
                nonnasa_requests.pop();
            }
        }

        void process_non_nasa_packet(MessageTarget *target)
        {
            if (debug_log_packets)
            {
                ESP_LOGW(TAG, "MSG: %s", nonpacket_.to_string().c_str());
            }

            target->register_address(nonpacket_.src);

            if (nonpacket_.cmd == NonNasaCommand::Cmd20)
            {
                last_command20s_[nonpacket_.src] = nonpacket_.command20;
                target->set_target_temperature(nonpacket_.src, nonpacket_.command20.target_temp);
                target->set_room_temperature(nonpacket_.src, nonpacket_.command20.room_temp);
                target->set_power(nonpacket_.src, nonpacket_.command20.power);
                target->set_mode(nonpacket_.src, nonnasa_mode_to_mode(nonpacket_.command20.mode));
                target->set_fanmode(nonpacket_.src, nonnasa_fanspeed_to_fanmode(nonpacket_.command20.fanspeed));
                // TODO
                target->set_altmode(nonpacket_.src, AltMode::None);
                // TODO
                target->set_swing_horizontal(nonpacket_.src, false);
                target->set_swing_vertical(nonpacket_.src, false);
            }
            else if (nonpacket_.cmd == NonNasaCommand::CmdF8)
            {
                // After cmd F8 (src:c8 dst:f0) is a lage gap in communication, time to send data
                if (nonpacket_.src == "c8" && nonpacket_.dst == "f0")
                {
                    // the communication needs a delay from cmdf8 to send the data.
                    // series of test-delay-times: 1ms: no reaction, 7ms reactions half the time, 10ms very often a reaction (95%) -> delay on 20ms should be safe
                    // the gap is around ~300ms
                    send_requests(target, 20);
                }
            }
            else if (nonpacket_.cmd == NonNasaCommand::CmdC6)
            {
                // If the control status is set we can send also
                if (nonpacket_.src == "c8" && nonpacket_.dst == "d0" && nonpacket_.commandC6.control_status == true)
                {
                    send_requests(target, 20);
                }
            }
        }
    } // namespace samsung_ac
} // namespace esphome
