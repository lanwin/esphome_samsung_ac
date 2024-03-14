#include "samsung_ac_device_custClim.h"
#include "esphome/core/log.h"
#include "samsung_ac.h"
#include "util.h"
#include "conversions.h"
#include <vector>
#include <set>
#include <algorithm>

namespace esphome
{
  namespace samsung_ac
  {

    climate::ClimateTraits Samsung_AC_CustClim::traits() {
      auto traits = climate::ClimateTraits();
      traits.set_supports_current_temperature(true);
      traits.set_visual_temperature_step(1);
      traits.set_visual_min_temperature(setMin);
      traits.set_visual_max_temperature(setMax);

      std::set<climate::ClimateMode> modes;
      for (int i = 0; i < 7; i++) {
        if (m[i] >= 0)modes.insert((climate::ClimateMode)i);
      }
      traits.set_supported_modes(modes);

      return traits;
    }
    
    void Samsung_AC_CustClim::control(const climate::ClimateCall &call){
      ProtocolRequest request;
      auto targetTempOpt = call.get_target_temperature();
      if (targetTempOpt.has_value()) {
        request.target_temp = targetTempOpt.value();
        ESP_LOGV(TAG, "CustClim req: temp=%f", targetTempOpt.value());
      }

      auto modeOpt = call.get_mode();
      if (modeOpt.has_value()){
        request.power = modeOpt.value() >0;
        ESP_LOGV(TAG, "CustClim req: power=%u", request.power.value());
        if (modeAddr) {
          esphome::climate::ClimateMode mod = modeOpt.value();
          if (mod < 0 || mod > 6) {
            ESP_LOGV(TAG, "Mode out of range");
          } else if (m[mod] == -1) {
            ESP_LOGV(TAG, "Mode %u recived from HA but not set in yaml", mod);
          } else {
            request.mode = (Mode)m[mod];
            ESP_LOGV(TAG, "CustClim req: mode=%u", request.mode.value());
          }
        }
      }
      request.caller = this;
      device->publish_request(request);
    }

    void Samsung_AC_CustClim::publishMode(){
      if (!modeAddr) lastReadMode = lastEnabled; // 0-1
      if (!lastEnabled) {
        mode = esphome::climate::ClimateMode::CLIMATE_MODE_OFF;
        publish_state();
      } else {
        int ret = -1;
        for (int i  = 0; i < 7; i++) {
          if(m[i] == lastReadMode) ret = i; //reverse lastReadMode
        }
        if (ret == -1) {
          ESP_LOGE(TAG, "Unable to find read mode %u, not mentioned in the yaml", lastReadMode);
        } else {
          mode = (esphome::climate::ClimateMode)ret;
          ESP_LOGV("TAG", "CC changed mode, read %u>%u for addr 0x%x", lastReadMode, ret);
          publish_state();
        }
      }
    } 

  }
  
}