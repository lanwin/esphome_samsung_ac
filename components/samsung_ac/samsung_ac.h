#pragma once

#include <set>
#include <map>
#include <optional>
#include <queue>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "samsung_ac_device.h"
#include "protocol.h"

namespace esphome
{
  namespace samsung_ac
  {
    class NasaProtocol;
    class Samsung_AC_Device;

    class Samsung_AC : public PollingComponent,
                       public uart::UARTDevice,
                       public MessageTarget
    {
    public:
      Samsung_AC() = default;

      float get_setup_priority() const override;
      void setup() override;
      void update() override;
      void loop() override;
      void dump_config() override;

      void set_debug_mqtt(std::string host, int port, std::string username, std::string password)
      {
        debug_mqtt_host = host;
        debug_mqtt_port = port;
        debug_mqtt_username = username;
        debug_mqtt_password = password;
      }

      void set_debug_log_messages(bool value)
      {
        debug_log_packets = value;
      }

      void set_debug_log_messages_raw(bool value)
      {
        debug_log_raw_bytes = value;
      }

      void register_device(Samsung_AC_Device *device);

      void /*MessageTarget::*/ register_address(const std::string address) override
      {
        addresses_.insert(address);
      }

      uint32_t /*MessageTarget::*/ get_miliseconds()
      {
        return millis();
      }

      void /*MessageTarget::*/ publish_data(std::vector<uint8_t> &data);

      void /*MessageTarget::*/ set_room_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_room_temperature(value);
      }

      void /*MessageTarget::*/ set_room_humidity(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_room_humidity(value);
      }

      void /*MessageTarget::*/ set_target_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_target_temperature(value);
      }

      void /*MessageTarget::*/ set_power(const std::string address, bool value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_power(value);
      }

      void /*MessageTarget::*/ set_mode(const std::string address, Mode mode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_mode(mode);
      }

      void /*MessageTarget::*/ set_fanmode(const std::string address, FanMode fanmode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->publish_fanmode(fanmode);
      }

    protected:
      Samsung_AC_Device *find_device(const std::string address)
      {
        auto it = devices_.find(address);
        if (it != devices_.end())
        {
          return it->second;
        }
        return nullptr;
      }

      std::map<std::string, Samsung_AC_Device *> devices_;
      std::set<std::string> addresses_;

      std::queue<std::vector<uint8_t>> send_queue_;
      std::vector<uint8_t> data_;
      uint32_t last_transmission_{0};

      bool data_processing_init = true;

      // settings from yaml
      std::string debug_mqtt_host = "";
      uint16_t debug_mqtt_port = 1883;
      std::string debug_mqtt_username = "";
      std::string debug_mqtt_password = "";
    };

  } // namespace samsung_ac
} // namespace esphome
