#include "esphome/core/log.h"
#include "samsung_ac.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    static const char *TAG = "samsung_ac";

    void Samsung_AC_Select::control(const std::string &value)
    {
      ESP_LOGW(TAG, "control %s", value.c_str());
    }

    void Samsung_AC_Device::set_room_temperature_sensor(esphome::sensor::Sensor *sensor)
    {
      room_temperature = sensor;
    }

    void Samsung_AC_Device::set_power_switch(Samsung_AC_Switch *switch_)
    {
      power = switch_;
      power->write_state_ = [this](bool value)
      {
        auto data = protocol->get_power_message(address, value);
        samsung_ac->send_bus_message(data);
      };
    }

    void Samsung_AC::setup() {}

    void Samsung_AC::update()
    {
      ESP_LOGW(TAG, "update");

      std::string devices = "";
      for (auto const &device : devices_)
      {
        if (devices.length() > 0)
          devices += ", ";
        devices += device->address;
      }
      ESP_LOGCONFIG(TAG, "reg devices: %s", devices.c_str());

      std::string known = "";
      for (auto const &address : addresses_)
      {
        if (known.length() > 0)
          known += ", ";
        known += address;
      }
      ESP_LOGCONFIG(TAG, "known: %s", known.c_str());
    }

    void Samsung_AC::register_device(Samsung_AC_Device *device)
    {
      devices_.push_back(device);
    }

    void Samsung_AC::send_bus_message(std::vector<uint8_t> &data)
    {
      this->write_array(data);
    }

    void Samsung_AC::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Samsung_AC:");
      ESP_LOGCONFIG(TAG, "dataline debug enabled?: %s", this->dataline_debug_ ? "true" : "false");
      this->check_uart_settings(2400, 1, esphome::uart::UART_CONFIG_PARITY_EVEN, 8);
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
        // nothing in uart-input-buffer, end here
        return;

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
