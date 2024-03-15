#pragma once

#include "esphome/components/number/number.h"
#include "esphome/components/number/number_traits.h"


namespace esphome
{
  namespace samsung_ac
  {
    class  Samsung_AC_NumberDebug : public number::Number{
    public:
      void control(float value) {targetValue = value;}
      void setup(std::string target);
      long targetValue;
      std::string source;

      static std::vector<Samsung_AC_NumberDebug *> elements;
    };

  }
}