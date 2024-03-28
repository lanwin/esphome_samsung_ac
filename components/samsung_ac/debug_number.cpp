#include "debug_number.h"


namespace esphome
{
  namespace samsung_ac
  {
    std::vector<Samsung_AC_NumberDebug *> Samsung_AC_NumberDebug::elements;

    void Samsung_AC_NumberDebug::setup(std::string target) {
        elements.push_back(this);
        source = target;
    }

  }
  
}