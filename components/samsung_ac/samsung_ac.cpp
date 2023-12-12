#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "debug_mqtt.h"
#include "util.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    static const char *TAG = "samsung_ac";

    void Samsung_AC::setup()
    {
      ESP_LOGW(TAG, "setup");
    }

    void Samsung_AC::update()
    {
      ESP_LOGW(TAG, "update");

      debug_mqtt_connect(debug_mqtt_host, debug_mqtt_port, debug_mqtt_username, debug_mqtt_password);

      // Waiting for first update before beginning processing data
      if (data_processing_init)
      {
        ESP_LOGCONFIG(TAG, "Data Processing starting");
        data_processing_init = false;
      }

      if (data_processing_paused)
      {
        ESP_LOGCONFIG(TAG, "Data Processing is paused !!!!");
      }

      std::string devices = "";
      for (const auto &[address, device] : devices_)
      {
        devices += devices.length() > 0 ? ", " + address : address;
      }
      ESP_LOGCONFIG(TAG, "Configured devices: %s", devices.c_str());

      std::string knownIndoor = "";
      std::string knownOutdoor = "";
      std::string knownOther = "";
      for (auto const &address : addresses_)
      {
        if (address == "00" || address.rfind("10.", 0) == 0)
        {
          knownOutdoor += knownOutdoor.length() > 0 ? ", " + address : address;
        }
        else if (!is_nasa_address(address) || address.rfind("20.", 0) == 0)
        {
          knownIndoor += knownIndoor.length() > 0 ? ", " + address : address;
        }
        else
        {
          knownOther += knownOther.length() > 0 ? ", " + address : address;
        }
      }
      ESP_LOGCONFIG(TAG, "Discovered devices:");
      ESP_LOGCONFIG(TAG, "  Outdoor: %s", (knownOutdoor.length() == 0 ? "-" : knownOutdoor.c_str()));
      ESP_LOGCONFIG(TAG, "  Indoor:  %s", (knownIndoor.length() == 0 ? "-" : knownIndoor.c_str()));
      if (knownOther.length() > 0)
        ESP_LOGCONFIG(TAG, "  Other:   %s", knownOther.c_str());
    }

    void Samsung_AC::send_bus_message(std::vector<uint8_t> &data)
    {
      out_.insert(out_.end(), data.begin(), data.end());
    }

    void Samsung_AC::dump_config()
    {
    }

    void Samsung_AC::loop()
    {
      if (data_processing_init || data_processing_paused)
        return;

      const uint32_t now = millis();
      if (receiving_ && (now - last_transmission_ >= 500))
      {
        ESP_LOGW(TAG, "Last transmission too long ago. Reset RX index.");
        data_.clear();
        receiving_ = false;
      }

      // If there is no data we use the time to send
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
