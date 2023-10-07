#pragma once

#include <set>
#include <optional>
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

      void register_address(const std::string address) override
      {
        addresses_.insert(address);
      }

      void set_dataline_debug(bool dataline_debug) { dataline_debug_ = dataline_debug; };

      void /*MessageTarget::*/ register_device(Samsung_AC_Device *device)
      {
        devices_.push_back(device);
      }

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

      void send_bus_message(std::vector<uint8_t> &data);

    protected:
      Samsung_AC_Device *find_device(const std::string address)
      {
        for (int i = 0; i < devices_.size(); i++)
        {
          Samsung_AC_Device *dev = devices_[i];
          if (dev->address == address)
            return dev;
        }
        return nullptr;
      }

      std::vector<Samsung_AC_Device *> devices_;
      std::set<std::string> addresses_;

      std::vector<uint8_t> out_;
      std::vector<uint8_t> data_;
      bool receiving_{false};
      uint32_t last_transmission_{0};

      // settings from yaml
      bool dataline_debug_{false};
    };

  } // namespace samsung_ac
} // namespace esphome
