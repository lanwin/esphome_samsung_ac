#pragma once

#include <optional>
#include "esphome/components/climate/climate.h"
#include "protocol.h"

namespace esphome
{
  namespace samsung_ac{

    Mode str_to_mode(const std::string &value);
    std::string mode_to_str(Mode mode);

    optional<climate::ClimateMode> mode_to_climatemode(Mode mode);
    Mode climatemode_to_mode(climate::ClimateMode mode);

    climate::ClimateFanMode fanmode_to_climatefanmode(FanMode fanmode);
    FanMode climatefanmode_to_fanmode(climate::ClimateFanMode fanmode);

    optional<climate::ClimatePreset> altmode_to_preset(AltMode mode);
    std::string altmode_to_custompreset(AltMode mode);
    AltMode preset_to_altmode(climate::ClimatePreset preset);
    AltMode custompreset_to_altmode(const std::string &value);

    climate::ClimateSwingMode swingmode_to_climateswingmode(SwingMode swingMode);
    SwingMode climateswingmode_to_swingmode(climate::ClimateSwingMode swingMode);
    
  } // namespace samsung_ac
} // namespace esphome
