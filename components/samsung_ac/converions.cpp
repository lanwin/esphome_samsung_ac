#include "conversions.h"

namespace esphome
{
  namespace samsung_ac
  {
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

  } // namespace samsung_ac
} // namespace esphome
