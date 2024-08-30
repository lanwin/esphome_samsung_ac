#ifndef DEVICE_STATE_TRACKER_H
#define DEVICE_STATE_TRACKER_H

#include <map>
#include <string>

template <typename T>
class DeviceStateTracker {
 public:
  DeviceStateTracker(unsigned long timeout_period)
      : TIMEOUT_PERIOD(timeout_period) {}

  void update(const std::string &address, const T &current_value);

 private:
  std::map<std::string, T> last_values_;
  std::map<std::string, unsigned long> last_update_time_;
  std::map<std::string, T> pending_changes_;
  const unsigned long TIMEOUT_PERIOD;
};

#endif // DEVICE_STATE_TRACKER_H
