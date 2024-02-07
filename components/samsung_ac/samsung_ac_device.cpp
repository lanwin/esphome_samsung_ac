#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include "conversions.h"
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

      std::set<climate::ClimatePreset> preset;
      preset.insert(climate::ClimatePreset::CLIMATE_PRESET_NONE);
      preset.insert(climate::ClimatePreset::CLIMATE_PRESET_SLEEP);
      traits.set_supported_presets(preset);

      std::set<std::string> customPreset;
      customPreset.insert("Quiet");
      customPreset.insert("Fast");
      customPreset.insert("Long Reach");
      customPreset.insert("WindFree");
      traits.set_supported_custom_presets(customPreset);

      return traits;
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
          device->write_mode(climatemode_to_mode(modeOpt.value()));
        }
      }

      auto fanmodeOpt = call.get_fan_mode();
      if (fanmodeOpt.has_value())
      {
        device->write_fanmode(climatefanmode_to_fanmode(fanmodeOpt.value()));
      }

      auto presetOpt = call.get_preset();
      if (presetOpt.has_value())
      {
        device->write_altmode(preset_to_altmode(presetOpt.value()));
      }

      auto customPresetOpt = call.get_custom_preset();
      if (customPresetOpt.has_value())
      {
        device->write_altmode(custompreset_to_altmode(customPresetOpt.value()));
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

    void Samsung_AC_Device::write_altmode(AltMode value)
    {
      protocol->publish_altmode_message(samsung_ac, address, value);
    }

    void Samsung_AC_Device::write_power(bool value)
    {
      protocol->publish_power_message(samsung_ac, address, value);
    }
  } // namespace samsung_ac
} // namespace esphome
