#include "samsung_ac.h"
#include "debug_mqtt.h"
#include "util.h"
#include "log.h"
#include <algorithm>
#include <vector>
#include <thread>

namespace esphome
{
  namespace samsung_ac
  {
    void Samsung_AC::setup()
    {
    }

    void Samsung_AC::update()
    {
      debug_mqtt_connect(debug_mqtt_host, debug_mqtt_port, debug_mqtt_username, debug_mqtt_password);

      // Waiting for first update before beginning processing data
      if (data_processing_init)
      {
        LOGC("Data Processing starting");
        data_processing_init = false;
      }

      std::string devices = "";
      for (const auto &pair : devices_)
      {
        devices += devices.length() > 0 ? ", " + pair.second->address : pair.second->address;
      }
      LOGC("Configured devices: %s", devices.c_str());

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
      LOGC("Discovered devices:");
      LOGC("  Outdoor: %s", (knownOutdoor.length() == 0 ? "-" : knownOutdoor.c_str()));
      LOGC("  Indoor:  %s", (knownIndoor.length() == 0 ? "-" : knownIndoor.c_str()));
      if (knownOther.length() > 0)
        LOGC("  Other:   %s", knownOther.c_str());
    }

    void Samsung_AC::register_device(Samsung_AC_Device *device)
    {
      if (find_device(device->address) != nullptr)
      {
        LOGW("There is already and device for address %s registered.", device->address.c_str());
        return;
      }

      devices_.insert({device->address, device});
    }

    void Samsung_AC::dump_config()
    {
    }

    void Samsung_AC::publish_data(uint8_t id, std::vector<uint8_t> &&data) 
    {
      const uint32_t now = millis();

      if (id == 0)
      {
        LOG_RAW_SEND(now-last_transmission_, data);
        last_transmission_ = now;
        this->write_array(data);
        this->flush();
        return;
      }

      OutgoingData outData;
      outData.id = id;
      outData.data = std::move(data);
      outData.nextRetry = 0;
      outData.retries = 0;
      outData.timeout = now + sendTimeout;
      send_queue_.push_back(std::move(outData));
    }

    void Samsung_AC::ack_data(uint8_t id)
    {
        if (!send_queue_.empty())
        {
          auto senddata = &send_queue_.front();
          if (senddata->id == id) {
            send_queue_.pop_front();
          }
        }
    }

    void Samsung_AC::loop()
    {
      if (data_processing_init)
        return;

      // if more data is expected, do not allow anything to be written
      if (!read_data())
        return;

      // If there is no data we use the time to send
      write_data();
    }

    bool Samsung_AC::read_data()
    {
      // read as long as there is anything to read
      while (available())
      {
        uint8_t c;
        if (read_byte(&c))
        {
          data_.push_back(c);
        }
      }

      if (data_.empty())
        return true;

      const uint32_t now = millis();

      auto result = process_data(data_, this);
      if (result.type == DecodeResultType::Fill)
        return false;

      if (result.type == DecodeResultType::Discard)
      {
        // collect more so that we can log all discarded bytes at once, but don't wait for too long
        if (result.bytes == data_.size() && now-last_transmission_ < 1000)
          return false;
        LOG_RAW_DISCARDED(now-last_transmission_, data_, 0, result.bytes);
      }
      else
      {
        LOG_RAW(now-last_transmission_, data_, 0, result.bytes);
      }

      if (result.bytes == data_.size())
        data_.clear();
      else
      {
        std::move(data_.begin() + result.bytes, data_.end(), data_.begin());
        data_.resize(data_.size() - result.bytes);
      }
      last_transmission_ = now;
      return false;
    }

    void Samsung_AC::write_data()
    {
      if (send_queue_.empty())
        return;  

      auto senddata = &send_queue_.front();

      const uint32_t now = millis();
      if (senddata->timeout <= now && senddata->retries >= minRetries) {
        LOGE("Packet sending timeout %d after %d retries", senddata->id, senddata->retries);
        send_queue_.pop_front();
        return;
      }

      if (now-last_transmission_ > silenceInterval && senddata->nextRetry < now)
      {
        if (senddata->nextRetry > 0){
          LOGW("Retry sending packet %d", senddata->id);
          senddata->retries++;
        }

        LOG_RAW_SEND(now-last_transmission_,  senddata->data);

        last_transmission_ = now;
        senddata->nextRetry = now + retryInterval;

        this->write_array(senddata->data);
        this->flush();
      }
    }

    float Samsung_AC::get_setup_priority() const { return setup_priority::DATA; }
  } // namespace samsung_ac
} // namespace esphome
