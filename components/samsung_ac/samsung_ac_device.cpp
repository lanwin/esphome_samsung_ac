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

      std::set<std::string> customFan;
      customFan.insert("Turbo");
      traits.set_supported_custom_fan_modes(customFan);

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

      std::set<climate::ClimateSwingMode> swingMode;
      swingMode.insert(climate::ClimateSwingMode::CLIMATE_SWING_OFF);
      swingMode.insert(climate::ClimateSwingMode::CLIMATE_SWING_HORIZONTAL);
      swingMode.insert(climate::ClimateSwingMode::CLIMATE_SWING_VERTICAL);
      swingMode.insert(climate::ClimateSwingMode::CLIMATE_SWING_BOTH);
      traits.set_supported_swing_modes(swingMode);

      return traits;
    }

    void Samsung_AC_Climate::control(const climate::ClimateCall &call)
    {
      traits();

      ProtocolRequest request;

      auto targetTempOpt = call.get_target_temperature();
      if (targetTempOpt.has_value())
        request.target_temp = targetTempOpt.value();

      auto modeOpt = call.get_mode();
      if (modeOpt.has_value())
      {
        if (modeOpt.value() == climate::ClimateMode::CLIMATE_MODE_OFF)
        {
          request.power = false;
        }
        else
        {
          request.mode = climatemode_to_mode(modeOpt.value());
        }
      }

      auto fanmodeOpt = call.get_fan_mode();
      if (fanmodeOpt.has_value())
      {
        request.fan_mode = climatefanmode_to_fanmode(fanmodeOpt.value());
      }

      auto customFanmodeOpt = call.get_custom_fan_mode();
      if (customFanmodeOpt.has_value())
      {
        request.fan_mode = customfanmode_to_fanmode(customFanmodeOpt.value());
      }

      auto presetOpt = call.get_preset();
      if (presetOpt.has_value())
      {
        request.alt_mode = preset_to_altmode(presetOpt.value());
      }

      auto customPresetOpt = call.get_custom_preset();
      if (customPresetOpt.has_value())
      {
        request.alt_mode = custompreset_to_altmode(customPresetOpt.value());
      }

      auto swingModeOpt = call.get_swing_mode();
      if (swingModeOpt.has_value())
      {
        request.swing_mode = climateswingmode_to_swingmode(swingModeOpt.value());
      }

      device->publish_request(request);
    }
  } // namespace samsung_ac
} // namespace esphome
