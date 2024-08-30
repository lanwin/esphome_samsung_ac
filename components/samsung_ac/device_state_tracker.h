#ifndef DEVICE_STATE_TRACKER_H
#define DEVICE_STATE_TRACKER_H

#include "esphome/core/log.h"
#include <map>
#include <string>

// Global log helper functions
inline void log_info(const char* tag, const char* format, const std::string &arg) {
    ESP_LOGI(tag, format, arg.c_str());
}

inline void log_warn(const char* tag, const char* format, const std::string &arg) {
    ESP_LOGW(tag, format, arg.c_str());
}

inline void log_debug(const char* tag, const char* format, const std::string &arg) {
    ESP_LOGD(tag, format, arg.c_str());
}

template <typename T>
class DeviceStateTracker {
 public:
  DeviceStateTracker(unsigned long timeout_period)
      : TIMEOUT_PERIOD(timeout_period) {}

  void update(const std::string &address, const T &current_value) {
    unsigned long now = millis();

    if (pending_changes_.find(address) != pending_changes_.end()) {
      if (current_value == pending_changes_[address]) {
        pending_changes_.erase(address);
      } else {
        log_info("device_state_tracker", "Stale value received for device: %s, ignoring.", address);
        return;
      }
    }

    if (last_values_.find(address) == last_values_.end() || last_values_[address] != current_value) {
      pending_changes_[address] = current_value;
      last_values_[address] = current_value;
      last_update_time_[address] = now;

      log_info("device_state_tracker", "Value changed for device: %s", address);
    } else {
      log_debug("device_state_tracker", "No change in value for device: %s", address);

      if (now - last_update_time_[address] > TIMEOUT_PERIOD) {
        if (pending_changes_.find(address) != pending_changes_.end()) {
          log_warn("device_state_tracker", "Timeout for device: %s, forcing update.", address);
          pending_changes_.erase(address);
        }
      }
    }
  }

 private:
  std::map<std::string, T> last_values_;
  std::map<std::string, unsigned long> last_update_time_;
  std::map<std::string, T> pending_changes_;
  const unsigned long TIMEOUT_PERIOD;
};

#endif // DEVICE_STATE_TRACKER_H
