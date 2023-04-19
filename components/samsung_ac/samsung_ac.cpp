#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include <vector>

std::string int_to_hex(int number)
{
  char str[3];
  sprintf(str, "%02x", number);
  return str;
}

std::string bytes_to_hex(const std::vector<uint8_t> &data)
{
  std::string str;
  for (int i = 0; i < data.size(); i++)
  {
    str += int_to_hex(data[i]);
  }
  return str;
}

std::vector<uint8_t> hex_to_bytes(const std::string &hex)
{
  std::vector<uint8_t> bytes;
  for (unsigned int i = 0; i < hex.length(); i += 2)
  {
    bytes.push_back((uint8_t)strtol(hex.substr(i, 2).c_str(), NULL, 16));
  }
  return bytes;
}

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

    void Samsung_AC_Device::set_target_temperature_number(Samsung_AC_Number *number)
    {
      target_temperature = number;
      target_temperature->write_state_ = [this](float value)
      {
        auto data = protocol->get_target_temp_message(address, value);
        samsung_ac->send_bus_message(data);
      };
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
        devices += devices.length() > 0 ? ", " + device->address : device->address;
      }
      ESP_LOGCONFIG(TAG, "registered devices: %s", devices.c_str());

      std::string known = "";
      for (auto const &address : addresses_)
      {
        known += known.length() > 0 ? ", " + address : address;
      }
      ESP_LOGCONFIG(TAG, "known addresses: %s", known.c_str());
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
