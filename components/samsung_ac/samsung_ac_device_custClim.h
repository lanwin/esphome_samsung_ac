#pragma once
#include <algorithm>
#include "esphome/components/climate/climate.h"

namespace esphome
{
  namespace samsung_ac
  {
    class Samsung_AC_Device;

    class  Samsung_AC_CustClim : public climate::Climate{
    public:
      climate::ClimateTraits traits();
      void control(const climate::ClimateCall &call);
      Samsung_AC_Device *device;
      uint16_t status, set, enable;
      float setMax, setMin;

      uint16_t modeAddr = 0;
      int m[7] = {-1, -1, -1, 1, -1, -1, -1};

      int lastReadMode = -1, lastEnabled = 1;
      void publishMode();
    };

  }
}
