#ifndef DEVICE_STATE_TRACKER_H
#define DEVICE_STATE_TRACKER_H

#include "esphome/core/log.h"
#include <map>
#include <string>

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
        printf("Stale value received for device: %s, ignoring.\n", address.c_str());
        return;
      }
    }

    if (last_values_.find(address) == last_values_.end() || last_values_[address] != current_value) {
      pending_changes_[address] = current_value;
      last_values_[address] = current_value;
      last_update_time_[address] = now;

      printf("Value changed for device: %s\n", address.c_str());
    } else {
      printf("No change in value for device: %s\n", address.c_str());

      if (now - last_update_time_[address] > TIMEOUT_PERIOD) {
        if (pending_changes_.find(address) != pending_changes_.end()) {
          printf("Timeout for device: %s, forcing update.\n", address.c_str());
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
