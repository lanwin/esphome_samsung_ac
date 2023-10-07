#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include <vector>

namespace esphome
{
  namespace samsung_ac
  {
    static const char *TAG = "samsung_ac";

    void Samsung_AC::setup()
    {
    }

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

    void Samsung_AC::send_bus_message(std::vector<uint8_t> &data)
    {
      out_.insert(out_.end(), data.begin(), data.end());
    }

    void Samsung_AC::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Samsung_AC:");
      ESP_LOGCONFIG(TAG, "dataline debug enabled?: %s", this->dataline_debug_ ? "true" : "false");
      this->check_uart_settings(9600, 1, uart::UART_CONFIG_PARITY_EVEN, 8);
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
