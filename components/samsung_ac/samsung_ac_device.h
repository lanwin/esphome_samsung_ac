#pragma once

#include <set>
#include <optional>
#include "esphome/components/switch/switch.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "esphome/components/climate/climate.h"
#include "protocol.h"
#include "samsung_ac.h"

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
      Mode str_to_mode(const std::string &value)
      {
        if (value == "Auto")
          return Mode::Auto;
        if (value == "Cool")
          return Mode::Cool;
        if (value == "Dry")
          return Mode::Dry;
        if (value == "Fan")
          return Mode::Fan;
        if (value == "Heat")
          return Mode::Heat;
        return Mode::Unknown;
      }

      std::string mode_to_str(Mode mode)
      {
        switch (mode)
        {
        case Mode::Auto:
          return "Auto";
        case Mode::Cool:
          return "Cool";
        case Mode::Dry:
          return "Dry";
        case Mode::Fan:
          return "Fan";
        case Mode::Heat:
          return "Heat";
        default:
          return "";
        };
      }

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

    class Samsung_AC_Switch : public switch_::Switch
    {
    public:
      std::function<void(bool)> write_state_;

    protected:
      void write_state(bool state) override
      {
        write_state_(state);
      }
    };

    class Samsung_AC_Device
    {
    public:
      Samsung_AC_Device(const std::string &address, Samsung_AC *samsung_ac)
      {
        this->address = address;
        this->samsung_ac = samsung_ac;
        this->protocol = get_protocol(address);
      }

      std::string address;
      sensor::Sensor *room_temperature{nullptr};
      sensor::Sensor *room_humidity{nullptr};
      Samsung_AC_Number *target_temperature{nullptr};
      Samsung_AC_Switch *power{nullptr};
      Samsung_AC_Mode_Select *mode{nullptr};
      Samsung_AC_Climate *climate{nullptr};

      void set_room_temperature_sensor(sensor::Sensor *sensor)
      {
        room_temperature = sensor;
      }

      void set_room_humidity_sensor(sensor::Sensor *sensor)
      {
        room_humidity = sensor;
      }

      void set_power_switch(Samsung_AC_Switch *switch_)
      {
        power = switch_;
        power->write_state_ = [this](bool value)
        {
          ESP_LOGV(TAG, "set power %d", value ? 1 : 0);
          write_power(value);
        };
      }

      void set_mode_select(Samsung_AC_Mode_Select *select)
      {
        mode = select;
        mode->write_state_ = [this](Mode value)
        {
          write_mode(value);
        };
      }

      void set_target_temperature_number(Samsung_AC_Number *number)
      {
        target_temperature = number;
        target_temperature->write_state_ = [this](float value)
        {
          write_target_temperature(value);
        };
      };

      void set_climate(Samsung_AC_Climate *value)
      {
        climate = value;
        climate->device = this;
      }

      void publish_target_temperature(float value)
      {
        if (target_temperature != nullptr)
          target_temperature->publish_state(value);
        if (climate != nullptr)
        {
          climate->target_temperature = value;
          climate->publish_state();
        }
      }

      optional<bool> _cur_power;
      optional<Mode> _cur_mode;

      void publish_power(bool value)
      {
        _cur_power = value;
        if (power != nullptr)
          power->publish_state(value);
        if (climate != nullptr)
          calc_and_publish_mode();
      }

      void publish_mode(Mode value)
      {
        _cur_mode = value;
        if (mode != nullptr)
          mode->publish_state_(value);
        if (climate != nullptr)
          calc_and_publish_mode();
      }

      void publish_fanmode(FanMode value)
      {
        if (climate != nullptr)
        {
          climate->fan_mode = fanmode_to_climatefanmode(value);
          climate->publish_state();
        }
      }

      void publish_room_temperature(float value)
      {
        if (room_temperature != nullptr)
          room_temperature->publish_state(value);
        if (climate != nullptr)
        {
          climate->current_temperature = value;
          climate->publish_state();
        }
      }

      void publish_room_humidity(float value)
      {
        if (room_humidity != nullptr)
          room_humidity->publish_state(value);
      }

      void write_target_temperature(float value);
      void write_mode(Mode value);
      void write_fanmode(FanMode value);
      void write_power(bool value);

    protected:
      Protocol *protocol{nullptr};
      Samsung_AC *samsung_ac{nullptr};

      optional<climate::ClimateMode> mode_to_climatemode(Mode mode)
      {
        switch (mode)
        {
        case Mode::Auto:
          return climate::ClimateMode::CLIMATE_MODE_AUTO;
        case Mode::Cool:
          return climate::ClimateMode::CLIMATE_MODE_COOL;
        case Mode::Dry:
          return climate::ClimateMode::CLIMATE_MODE_DRY;
        case Mode::Fan:
          return climate::ClimateMode::CLIMATE_MODE_FAN_ONLY;
        case Mode::Heat:
          return climate::ClimateMode::CLIMATE_MODE_HEAT;
        default:
          return nullopt;
        }
      }

      climate::ClimateFanMode fanmode_to_climatefanmode(FanMode fanmode)
      {
        switch (fanmode)
        {
        case FanMode::Low:
          return climate::ClimateFanMode::CLIMATE_FAN_LOW;
        case FanMode::Mid:
          return climate::ClimateFanMode::CLIMATE_FAN_MIDDLE;
        case FanMode::Hight:
          return climate::ClimateFanMode::CLIMATE_FAN_HIGH;
        default:
        case FanMode::Auto:
          return climate::ClimateFanMode::CLIMATE_FAN_AUTO;
        }
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
