#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    static const char *TAG = "samsung_ac";

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
      fan.insert(climate::ClimateFanMode::CLIMATE_FAN_AUTO);

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

    void Samsung_AC_Climate::control(const climate::ClimateCall &call)
    {
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
          device->write_power(true);
        }
      }
    }

    void Samsung_AC_Device::write_target_temperature(float value)
    {
      auto data = protocol->get_target_temp_message(address, value);
      samsung_ac->send_bus_message(data);
    }

    void Samsung_AC_Device::write_mode(Mode value)
    {
      auto data = protocol->get_mode_message(address, value);
      samsung_ac->send_bus_message(data);
    }

    void Samsung_AC_Device::write_power(bool value)
    {
      auto data = protocol->get_power_message(address, value);
      samsung_ac->send_bus_message(data);
    }

    void Samsung_AC::setup() {}

    void Samsung_AC::update()
    {
      ESP_LOGW(TAG, "update");

      std::string devices = "";
      for (auto const &device : devices_)
      {
        devices += devices.length() > 0 ? ", " + device->address : device->address;
      }
      ESP_LOGCONFIG(TAG, "registered devices: %s", devices.c_str());

      std::string knownIndoor = "";
      std::string knownOutdoor = "";
      for (auto const &address : addresses_)
      {
        if (address == "00" || address.rfind("10.", 0) == 0)
        {
          knownOutdoor += knownOutdoor.length() > 0 ? ", " + address : address;
        }
        else
        {
          knownIndoor += knownIndoor.length() > 0 ? ", " + address : address;
        }
      }
      ESP_LOGCONFIG(TAG, "known indoor devices: %s", knownIndoor.c_str());
      ESP_LOGCONFIG(TAG, "known outdoor devices: %s", knownOutdoor.c_str());
    }

    void Samsung_AC::register_device(Samsung_AC_Device *device)
    {
      devices_.push_back(device);
    }

    void Samsung_AC::send_bus_message(std::vector<uint8_t> &data)
    {
      out_.insert(out_.end(), data.begin(), data.end());
    }

    void Samsung_AC::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Samsung_AC:");
      ESP_LOGCONFIG(TAG, "dataline debug enabled?: %s", this->dataline_debug_ ? "true" : "false");
      this->check_uart_settings(2400, 1, uart::UART_CONFIG_PARITY_EVEN, 8);
    }

    void Samsung_AC::loop()
    {
      const uint32_t now = millis();
      if (receiving_ && (now - last_transmission_ >= 500))
      {
        // last transmission too long ago. Reset RX index.
        ESP_LOGW(TAG, "Last transmission too long ago. Reset RX index.");
        data_.clear();
        receiving_ = false;
      }

      if (!available())
      {
        if (out_.size() > 0)
        {
          ESP_LOGW(TAG, "write %s", bytes_to_hex(out_).c_str());
          this->write_array(out_);
          this->flush();
          out_.clear();
        }

        return; // nothing in uart-input-buffer, end here
      }

      last_transmission_ = now;
      while (available())
      {
        uint8_t c;
        read_byte(&c);
        if (c == 0x32 && !receiving_) // start-byte found
        {
          receiving_ = true;
          data_.clear();
        }
        if (receiving_)
        {
          data_.push_back(c);
          if (c != 0x34)
            continue; // endbyte not found

          receiving_ = false;
          process_message(data_, this);
        }
      }
    }

    float Samsung_AC::get_setup_priority() const { return setup_priority::DATA; }

  } // namespace samsung_ac
} // namespace esphome
