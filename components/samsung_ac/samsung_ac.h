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
          dev->update_room_temperature(value);
      }

      void /*MessageTarget::*/ set_outdoor_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_outdoor_temperature(value);
      }

      void /*MessageTarget::*/ getValueForCustomClimate(const std::string source, uint16_t messageNumber, long value) {
        Samsung_AC_Device *dev = find_device(source);
        if (dev != nullptr) dev->getValueForCustomClimate(messageNumber, value);

      }

      void /*MessageTarget::*/ set_target_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_target_temperature(value);
      }

      void /*MessageTarget::*/ set_power(const std::string address, bool value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_power(value);
      }

      void /*MessageTarget::*/ set_mode(const std::string address, Mode mode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_mode(mode);
      }

      void /*MessageTarget::*/ set_fanmode(const std::string address, FanMode fanmode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_fanmode(fanmode);
      }

      void /*MessageTarget::*/ set_altmode(const std::string address, AltMode altmode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_altmode(altmode);
      }

      void /*MessageTarget::*/ set_swing_vertical(const std::string address, bool vertical) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_swing_vertical(vertical);
      }

      void /*MessageTarget::*/ set_swing_horizontal(const std::string address, bool horizontal) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_swing_horizontal(horizontal);
      }

      optional<std::set<uint16_t>> /*MessageTarget::*/ get_custom_sensors(const std::string address) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          return optional<std::set<uint16_t>>(dev->get_custom_sensors());
        return optional<std::set<uint16_t>>();
      }
      
      void /*MessageTarget::*/ set_custom_sensor(const std::string address, uint16_t message_number, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev != nullptr)
          dev->update_custom_sensor(message_number, value);
      }
      
      Samsung_AC_Device *find_device(const std::string address)
      {
        auto it = devices_.find(address);
        if (it != devices_.end())
        {
          return it->second;
        }
        return nullptr;
      }

    protected:

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
