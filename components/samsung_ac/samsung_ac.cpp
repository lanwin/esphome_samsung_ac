#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "debug_mqtt.h"
#include "util.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    void Samsung_AC::setup()
    {
      if (debug_log_messages)
      {
        ESP_LOGW(TAG, "setup");
      }
    }

    void Samsung_AC::update()
    {
      if (debug_log_messages)
      {
        ESP_LOGW(TAG, "update");
      }

      debug_mqtt_connect(debug_mqtt_host, debug_mqtt_port, debug_mqtt_username, debug_mqtt_password);

      // Waiting for first update before beginning processing data
      if (data_processing_init)
      {
        ESP_LOGCONFIG(TAG, "Data Processing starting");
        data_processing_init = false;
      }

      std::string devices = "";
      for (const auto &pair : devices_)
      {
        devices += devices.length() > 0 ? ", " + pair.second->address : pair.second->address;
      }
      ESP_LOGCONFIG(TAG, "Configured devices: %s", devices.c_str());

      std::string knownIndoor = "";
      std::string knownOutdoor = "";
      std::string knownOther = "";
      for (auto const &address : addresses_)
      {
        switch (get_address_type(address))
        {
        case AddressType::Outdoor:
          knownOutdoor += knownOutdoor.length() > 0 ? ", " + address : address;
          break;
        case AddressType::Indoor:
          knownIndoor += knownIndoor.length() > 0 ? ", " + address : address;
          break;
        default:
          knownOther += knownOther.length() > 0 ? ", " + address : address;
          break;
        }
      }
      ESP_LOGCONFIG(TAG, "Discovered devices:");
      ESP_LOGCONFIG(TAG, "  Outdoor: %s", (knownOutdoor.length() == 0 ? "-" : knownOutdoor.c_str()));
      ESP_LOGCONFIG(TAG, "  Indoor:  %s", (knownIndoor.length() == 0 ? "-" : knownIndoor.c_str()));
      if (knownOther.length() > 0)
      {
        ESP_LOGCONFIG(TAG, "  Other:   %s", knownOther.c_str());
      }
    }

    void Samsung_AC::register_device(Samsung_AC_Device *device)
    {
      if (find_device(device->address) != nullptr)
      {
        ESP_LOGW(TAG, "There is already and device for address %s registered.", device->address.c_str());
        return;
      }

      devices_.insert({device->address, device});
    }

    void Samsung_AC::dump_config()
    {
    }

    void Samsung_AC::publish_data(std::vector<uint8_t> &data)
    {
      ESP_LOGW(TAG, "write %s", bytes_to_hex(data).c_str());
      this->write_array(data);
      this->flush();
    }

    void Samsung_AC::loop()
    {
      if (data_processing_init)
        return;

      const uint32_t now = millis();
      if (data_.size() > 0 && (now - last_transmission_ >= 500))
      {
        ESP_LOGW(TAG, "Last transmission too long ago. Reset RX index.");
        data_.clear();
      }

      // If there is no data we use the time to send
      if (available())
      {
        last_transmission_ = now;
        while (available())
        {
          uint8_t c;
          if (!read_byte(&c))
            continue;
          if (data_.size() == 0 && c != 0x32)
            continue; // skip until start-byte found

          data_.push_back(c);

          if (process_data(data_, this) == DataResult::Clear)
          {
            data_.clear();
            break; // wait for next loop
          }
        }
      }

      // Allow device protocols to perform recurring tasks (at most every 200ms)
      if (now - last_protocol_update_ >= 200)
      {
        last_protocol_update_ = now;
        for (const auto &pair : devices_)
        {
          Samsung_AC_Device *device = pair.second;
          device->protocol_update(this);
        }
      }
    }

    float Samsung_AC::get_setup_priority() const { return setup_priority::DATA; }
  } // namespace samsung_ac
} // namespace esphome
