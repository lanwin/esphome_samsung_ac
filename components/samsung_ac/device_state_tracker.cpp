#include "device_state_tracker.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome
{
  static const char *TAG = "device_state_tracker"; 

  template <typename T>
  void DeviceStateTracker<T>::update(const std::string &address, const T &current_value) {
      unsigned long now = millis();  

      if (pending_changes_.find(address) != pending_changes_.end()) {
          if (current_value == pending_changes_[address]) {
              pending_changes_.erase(address);
          } else {
              ESP_LOGI(TAG, "Stale value received for device: %s, ignoring.", address.c_str());
              return;
          }
      }

      if (last_values_.find(address) == last_values_.end() || last_values_[address] != current_value) {
          pending_changes_[address] = current_value;
          last_values_[address] = current_value;
          last_update_time_[address] = now;

          ESP_LOGI(TAG, "Value changed for device: %s", address.c_str());
      } else {
          ESP_LOGD(TAG, "No change in value for device: %s", address.c_str());

          if (now - last_update_time_[address] > TIMEOUT_PERIOD) {
              if (pending_changes_.find(address) != pending_changes_.end()) {
                  ESP_LOGW(TAG, "Timeout for device: %s, forcing update.", address.c_str());
                  pending_changes_.erase(address);
              }
          }
      }
  }

  template class DeviceStateTracker<int>; 
} // namespace esphome
