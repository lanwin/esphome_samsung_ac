#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    climate::ClimateTraits Samsung_AC_Climate::traits()
    {
      auto traits = climate::ClimateTraits();

      traits.set_supports_current_temperature(true);

      traits.set_visual_temperature_step(1);
      traits.set_visual_min_temperature(16);
      traits.set_visual_max_temperature(30);

      std::set<climate::ClimateMode> modes;
      modes.insert(climate::CLIMATE_MODE_OFF);
      modes.insert(climate::CLIMATE_MODE_AUTO);
      modes.insert(climate::CLIMATE_MODE_COOL);
      modes.insert(climate::CLIMATE_MODE_DRY);
      modes.insert(climate::CLIMATE_MODE_FAN_ONLY);
      modes.insert(climate::CLIMATE_MODE_HEAT);
      traits.set_supported_modes(modes);

      std::set<climate::ClimateFanMode> fan;
      fan.insert(climate::ClimateFanMode::CLIMATE_FAN_HIGH);
      fan.insert(climate::ClimateFanMode::CLIMATE_FAN_MIDDLE);
      fan.insert(climate::ClimateFanMode::CLIMATE_FAN_LOW);

      if (this->mode != climate::CLIMATE_MODE_FAN_ONLY)
      {
        fan.insert(climate::ClimateFanMode::CLIMATE_FAN_AUTO);
      }

      if (is_nasa_address(device->address))
      {
        // fan.insert(climate::ClimateFanMode::CLIMATE_FAN_DIFFUSE);
      }

      traits.set_supported_fan_modes(fan);

      return traits;
    }

    Mode climatemode_to_mode(climate::ClimateMode mode)
    {
      switch (mode)
      {
      case climate::ClimateMode::CLIMATE_MODE_COOL:
        return Mode::Cool;
      case climate::ClimateMode::CLIMATE_MODE_HEAT:
        return Mode::Heat;
      case climate::ClimateMode::CLIMATE_MODE_FAN_ONLY:
        return Mode::Fan;
      case climate::ClimateMode::CLIMATE_MODE_DRY:
        return Mode::Dry;
      case climate::ClimateMode::CLIMATE_MODE_AUTO:
        return Mode::Auto;
      default:
        return Mode::Unknown;
      }
    }

    FanMode climatefanmode_to_fanmode(climate::ClimateFanMode fanmode)
    {
      switch (fanmode)
      {
      case climate::ClimateFanMode::CLIMATE_FAN_LOW:
        return FanMode::Low;
      case climate::ClimateFanMode::CLIMATE_FAN_MIDDLE:
        return FanMode::Mid;
      case climate::ClimateFanMode::CLIMATE_FAN_HIGH:
        return FanMode::Hight;
      case climate::ClimateFanMode::CLIMATE_FAN_AUTO:
      default:
        return FanMode::Auto;
      }
    }

    void Samsung_AC_Climate::control(const climate::ClimateCall &call)
    {
      traits();

      auto targetTempOpt = call.get_target_temperature();
      if (targetTempOpt.has_value())
        device->write_target_temperature(targetTempOpt.value());

      auto modeOpt = call.get_mode();
      if (modeOpt.has_value())
      {
        if (modeOpt.value() == climate::ClimateMode::CLIMATE_MODE_OFF)
        {
          device->write_power(false);
        }
        else
        {
          device->write_power(true);
          device->write_mode(climatemode_to_mode(modeOpt.value()));
        }
      }

      auto fanmodeOpt = call.get_fan_mode();
      if (fanmodeOpt.has_value())
      {
        device->write_fanmode(climatefanmode_to_fanmode(fanmodeOpt.value()));
      }
    }

    void Samsung_AC_Device::write_target_temperature(float value)
    {
      protocol->publish_target_temp_message(samsung_ac, address, value);
    }

    void Samsung_AC_Device::write_mode(Mode value)
    {
      protocol->publish_mode_message(samsung_ac, address, value);
    }

    void Samsung_AC_Device::write_fanmode(FanMode value)
    {
      protocol->publish_fanmode_message(samsung_ac, address, value);
    }

    void Samsung_AC_Device::write_power(bool value)
    {
      protocol->publish_power_message(samsung_ac, address, value);
    }
  } // namespace samsung_ac
} // namespace esphome
