#pragma once

#include <set>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "protocol.h"

namespace esphome
{
  namespace samsung_ac
  {
    class NasaProtocol;
    class Samsung_AC;

    class Samsung_AC_Number : public esphome::number::Number
    {
    public:
      void control(float value) override
      {
        write_state_(value);
      }
      std::function<void(float)> write_state_;
    };

    class Samsung_AC_Select : public esphome::select::Select
    {
    public:
      void control(const std::string &value) override;
    };

    class Samsung_AC_Switch : public esphome::switch_::Switch
    {
    public:
      std::function<void(bool)> write_state_;

    protected:
      void write_state(bool state) override
      {
        write_state_(state);
      }
    };

    class Samsung_AC_Device
    {
    public:
      Samsung_AC_Device(const std::string &address, Samsung_AC *samsung_ac)
      {
        this->address = address;
        this->samsung_ac = samsung_ac;
        this->protocol = get_protocol(address);
      }

      std::string address;
      esphome::sensor::Sensor *room_temperature{nullptr};
      Samsung_AC_Number *target_temperature{nullptr};
      Samsung_AC_Switch *power{nullptr};

      void set_room_temperature_sensor(esphome::sensor::Sensor *sensor);
      void set_target_temperature_number(Samsung_AC_Number *number);
      void set_power_switch(Samsung_AC_Switch *switch_);

    protected:
      Protocol *protocol{nullptr};
      Samsung_AC *samsung_ac{nullptr};
    };

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

      void register_device(Samsung_AC_Device *device);

      void set_dataline_debug(bool dataline_debug) { dataline_debug_ = dataline_debug; };

      void set_room_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev == nullptr || dev->room_temperature == nullptr)
          return;
        dev->room_temperature->publish_state(value);
      }

      void set_target_temperature(const std::string address, float value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev == nullptr || dev->target_temperature == nullptr)
          return;
        dev->target_temperature->publish_state(value);
      }

      void set_power(const std::string address, bool value) override
      {
        Samsung_AC_Device *dev = find_device(address);
        if (dev == nullptr || dev->power == nullptr)
          return;
        dev->power->publish_state(value);
      }

      void set_mode(const std::string address, Mode mode) override
      {
        Samsung_AC_Device *dev = find_device(address);
        // if (dev == nullptr || dev->mode == nullptr)
        // return;
        // dev->mode->publish_state_(mode);
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
