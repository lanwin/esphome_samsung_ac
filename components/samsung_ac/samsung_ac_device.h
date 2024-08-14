#pragma once

#include <set>
#include <optional>
#include <algorithm>
#include "esphome/core/helpers.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "esphome/components/climate/climate.h"
#include "protocol.h"
#include "samsung_ac.h"
#include "conversions.h"

namespace esphome
{
  namespace samsung_ac
  {
    class Samsung_AC;
    class Samsung_AC_Device;

    class Samsung_AC_Climate : public climate::Climate
    {
    public:
      climate::ClimateTraits traits();
      void control(const climate::ClimateCall &call);
      Samsung_AC_Device *device;

    protected:
      void set_alt_mode_by_name(ProtocolRequest &request, const AltModeName &name);
    };

    class Samsung_AC_Number : public number::Number
    {
    public:
      void control(float value) override
      {
        write_state_(value);
      }
      std::function<void(float)> write_state_;
    };

    class Samsung_AC_Mode_Select : public select::Select
    {
    public:
      void publish_state_(Mode mode)
      {
        this->publish_state(mode_to_str(mode));
      }

      void control(const std::string &value) override
      {
        write_state_(str_to_mode(value));
      }

      std::function<void(Mode)> write_state_;
    };
    
    class Samsung_AC_Water_Heater_Mode_Select : public select::Select
    {
    public:
      void publish_state_(WaterHeaterMode waterheatermode)
      {
        this->publish_state(water_heater_mode_to_str(waterheatermode));
      }

      void control(const std::string &value) override
      {
        write_state_(str_to_water_heater_mode(value));
      }

      std::function<void(WaterHeaterMode)> write_state_;
    };

    class Samsung_AC_Switch : public switch_::Switch
    {
    public:
      std::function<void(bool)> write_state_;

    protected:
      void write_state(bool state) override
      {
        this->publish_state(state);
        write_state_(state);
      }
    };

    struct Samsung_AC_Sensor
    {
      uint16_t message_number;
      sensor::Sensor *sensor;
    };

    class Samsung_AC_Device
    {
    public:
      Samsung_AC_Device(const std::string &address, MessageTarget *target)
      {
        this->address = address;
        this->target = target;
        this->protocol = get_protocol(address);
      }

      std::string address;
      sensor::Sensor *room_temperature{nullptr};
      sensor::Sensor *outdoor_temperature{nullptr};
      sensor::Sensor *indoor_eva_in_temperature{nullptr};
      sensor::Sensor *indoor_eva_out_temperature{nullptr};
      Samsung_AC_Number *target_temperature{nullptr};
      Samsung_AC_Number *water_outlet_target{nullptr};
      Samsung_AC_Number *target_water_temperature{nullptr};
      Samsung_AC_Switch *power{nullptr};
      Samsung_AC_Switch *automatic_cleaning{nullptr};
      Samsung_AC_Switch *water_heater_power{nullptr};
      Samsung_AC_Mode_Select *mode{nullptr};
      Samsung_AC_Water_Heater_Mode_Select *waterheatermode{nullptr};
      Samsung_AC_Climate *climate{nullptr};
      std::vector<Samsung_AC_Sensor> custom_sensors;
      float room_temperature_offset{0};

      void set_room_temperature_sensor(sensor::Sensor *sensor)
      {
        room_temperature = sensor;
      }

      void set_outdoor_temperature_sensor(sensor::Sensor *sensor)
      {
        outdoor_temperature = sensor;
      }

      void set_indoor_eva_in_temperature_sensor(sensor::Sensor *sensor)
      {
        indoor_eva_in_temperature = sensor;
      }

      void set_indoor_eva_out_temperature_sensor(sensor::Sensor *sensor)
      {
        indoor_eva_out_temperature = sensor;
      }

      void add_custom_sensor(int message_number, sensor::Sensor *sensor)
      {
        Samsung_AC_Sensor cust_sensor;
        cust_sensor.message_number = (uint16_t)message_number;
        cust_sensor.sensor = sensor;
        custom_sensors.push_back(std::move(cust_sensor));
      }

      std::set<uint16_t> get_custom_sensors()
      {
        std::set<uint16_t> numbers;
        for (auto &sensor : custom_sensors)
          numbers.insert(sensor.message_number);
        return numbers;
      }

      void set_power_switch(Samsung_AC_Switch *switch_)
      {
        power = switch_;
        power->write_state_ = [this](bool value)
        {
          ProtocolRequest request;
          request.power = value;
          publish_request(request);
        };
      }

      void set_automatic_cleaning_switch(Samsung_AC_Switch *switch_)
      {
        automatic_cleaning = switch_;
        automatic_cleaning->write_state_ = [this](bool value)
        {
          ProtocolRequest request;
          request.automatic_cleaning = value;
          publish_request(request);
        };
      }

      void set_water_heater_power_switch(Samsung_AC_Switch *switch_)
      {
        water_heater_power = switch_;
        water_heater_power->write_state_ = [this](bool value)
        {
          ProtocolRequest request;
          request.water_heater_power = value;
          publish_request(request);
        };
      }

      void set_mode_select(Samsung_AC_Mode_Select *select)
      {
        mode = select;
        mode->write_state_ = [this](Mode value)
        {
          ProtocolRequest request;
          request.mode = value;
          publish_request(request);
        };
      }
      
      void set_water_heater_mode_select(Samsung_AC_Water_Heater_Mode_Select *select)
      {
        waterheatermode = select;
        waterheatermode->write_state_ = [this](WaterHeaterMode value)
        {
          ProtocolRequest request;
          request.waterheatermode = value;
          publish_request(request);
        };
      }

      void set_target_temperature_number(Samsung_AC_Number *number)
      {
        target_temperature = number;
        target_temperature->write_state_ = [this](float value)
        {
          ProtocolRequest request;
          request.target_temp = value;
          publish_request(request);
        };
      };

      void set_water_outlet_target_number(Samsung_AC_Number *number)
      {
        water_outlet_target = number;
        water_outlet_target->write_state_ = [this](float value)
        {
          ProtocolRequest request;
          request.water_outlet_target = value;
          publish_request(request);
        };
      };

      void set_target_water_temperature_number(Samsung_AC_Number *number)
      {
        target_water_temperature = number;
        target_water_temperature->write_state_ = [this](float value)
        {
          ProtocolRequest request;
          request.target_water_temp = value;
          publish_request(request);
        };
      };

      void set_climate(Samsung_AC_Climate *value)
      {
        climate = value;
        climate->device = this;
      }

      void update_target_temperature(float value)
      {
        if (target_temperature != nullptr)
          target_temperature->publish_state(value);
        if (climate != nullptr)
        {
          climate->target_temperature = value;
          climate->publish_state();
        }
      }

      void update_water_outlet_target(float value)
      {
        if (water_outlet_target != nullptr)
          water_outlet_target->publish_state(value);
      }

      void update_target_water_temperature(float value)
      {
        if (target_water_temperature != nullptr)
          target_water_temperature->publish_state(value);
      }

      optional<bool> _cur_power;
      optional<bool> _cur_automatic_cleaning;
      optional<bool> _cur_water_heater_power;
      optional<Mode> _cur_mode;
      optional<WaterHeaterMode> _cur_water_heater_mode;

      void update_power(bool value)
      {
        _cur_power = value;
        if (power != nullptr)
          power->publish_state(value);
        if (climate != nullptr)
          calc_and_publish_mode();
      }

      void update_automatic_cleaning(bool value)
      {
        _cur_automatic_cleaning = value;
        if (automatic_cleaning != nullptr)
          automatic_cleaning->publish_state(value);
        if (climate != nullptr)
          calc_and_publish_mode();
      }

      void update_water_heater_power(bool value)
      {
        _cur_water_heater_power = value;
        if (water_heater_power != nullptr)
          water_heater_power->publish_state(value);
      }

      void update_mode(Mode value)
      {
        _cur_mode = value;
        if (mode != nullptr)
          mode->publish_state_(value);
        if (climate != nullptr)
          calc_and_publish_mode();
      }
      
      void update_water_heater_mode(WaterHeaterMode value)
      {
        _cur_water_heater_mode = value;
        if (waterheatermode != nullptr)
          waterheatermode->publish_state_(value);
      }

      void update_fanmode(FanMode value)
      {
        if (climate != nullptr)
        {
          climate->fan_mode = fanmode_to_climatefanmode(value);

          auto fanmode = fanmode_to_climatefanmode(value);
          if (fanmode.has_value())
          {
            climate->fan_mode = fanmode;
            climate->custom_fan_mode.reset();
          }
          else
          {
            climate->fan_mode.reset();
            climate->custom_fan_mode = fanmode_to_custom_climatefanmode(value);
          }
          climate->publish_state();
        }
      }

      void update_altmode(AltMode value)
      {
        if (climate != nullptr)
        {
          auto supported = get_supported_alt_modes();
          auto mode = std::find_if(supported->begin(), supported->end(), [&value](const AltModeDesc &x)
                                   { return x.value == value; });
          if (mode == supported->end())
          {
            ESP_LOGW(TAG, "Unsupported alt_mode %d", value);
            return;
          }

          auto preset = altmodename_to_preset(mode->name);
          if (preset)
          {
            climate->preset = preset.value();
            climate->custom_preset.reset();
          }
          else
          {
            climate->preset.reset();
            climate->custom_preset = mode->name;
          }
          climate->publish_state();
        }
      }

      void update_swing_vertical(bool value)
      {
        if (climate != nullptr)
        {
          climate->swing_mode = combine(climate->swing_mode, 1, value);
          climate->publish_state();
        }
      }

      void update_swing_horizontal(bool value)
      {
        if (climate != nullptr)
        {
          climate->swing_mode = combine(climate->swing_mode, 2, value);
          climate->publish_state();
        }
      }

      void update_room_temperature(float value)
      {
        if (room_temperature != nullptr)
          room_temperature->publish_state(value + room_temperature_offset);
        if (climate != nullptr)
        {
          climate->current_temperature = value + room_temperature_offset;
          climate->publish_state();
        }
      }

      void update_outdoor_temperature(float value)
      {
        if (outdoor_temperature != nullptr)
          outdoor_temperature->publish_state(value);
      }

      void update_indoor_eva_in_temperature(float value)
      {
        if (indoor_eva_in_temperature != nullptr)
          indoor_eva_in_temperature->publish_state(value);
      }

      void update_indoor_eva_out_temperature(float value)
      {
        if (indoor_eva_out_temperature != nullptr)
          indoor_eva_out_temperature->publish_state(value);
      }

      void update_custom_sensor(uint16_t message_number, float value)
      {
        for (auto &sensor : custom_sensors)
          if (sensor.message_number == message_number)
            sensor.sensor->publish_state(value);
      }

      void publish_request(ProtocolRequest &request)
      {
        protocol->publish_request(target, address, request);
      }

      bool supports_horizontal_swing()
      {
        return supports_horizontal_swing_;
      }

      bool supports_vertical_swing()
      {
        return supports_vertical_swing_;
      }

      void set_supports_horizontal_swing(bool value)
      {
        supports_horizontal_swing_ = value;
      }

      void set_supports_vertical_swing(bool value)
      {
        supports_vertical_swing_ = value;
      }

      void add_alt_mode(const AltModeName &name, AltMode value)
      {
        AltModeDesc desc;
        desc.name = name;
        desc.value = value;
        alt_modes.push_back(std::move(desc));
      }

      const std::vector<AltModeDesc> *get_supported_alt_modes()
      {
        return &alt_modes;
      }

      void set_room_temperature_offset(float value)
      {
        room_temperature_offset = value;
      }

      void protocol_update(MessageTarget *target)
      {
        if (protocol != nullptr)
        {
          protocol->protocol_update(target);
        }
      }

    protected:
      bool supports_horizontal_swing_{true};
      bool supports_vertical_swing_{true};
      std::vector<AltModeDesc> alt_modes;

      Protocol *protocol{nullptr};
      MessageTarget *target{nullptr};

      climate::ClimateSwingMode combine(climate::ClimateSwingMode climateSwingMode, uint8_t mask, bool value)
      {
        uint8_t swingMode = static_cast<uint8_t>(climateswingmode_to_swingmode(climateSwingMode));
        return swingmode_to_climateswingmode(static_cast<SwingMode>(value ? (swingMode | mask) : (swingMode & ~mask)));
      }

      void calc_and_publish_mode()
      {
        if (!_cur_power.has_value())
          return;
        if (!_cur_mode.has_value())
          return;

        climate->mode = climate::ClimateMode::CLIMATE_MODE_OFF;
        if (_cur_power.value() == true)
        {
          auto opt = mode_to_climatemode(_cur_mode.value());
          if (opt.has_value())
            climate->mode = opt.value();
        }

        climate->publish_state();
      }
    };
  } // namespace samsung_ac
} // namespace esphome
