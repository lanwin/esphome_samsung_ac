#pragma once

#include "esphome/components/number/number.h"
#include "esphome/components/number/number_traits.h"


namespace esphome
{
  namespace samsung_ac
  {
    class  Samsung_AC_NumberDebug : public number::Number{
    public:
      static long const UNUSED = -99999;

      void control(float value) {targetValue = value;}
      void setup(std::string target);
      long targetValue = UNUSED;
      std::string source;

      static std::vector<Samsung_AC_NumberDebug *> elements;
    };

  }
}