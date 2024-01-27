#include <vector>
#include <iostream>
#include <bitset>
#include <cassert>

#include "../components/samsung_ac/util.h"
#include "../components/samsung_ac/protocol.h"

using namespace std;
using namespace esphome::samsung_ac;

class DebugTarget : public MessageTarget
{
public:
    uint32_t get_miliseconds()
    {
        return 0;
    }
    void publish_data(std::vector<uint8_t> &data)
    {
    }

    std::string last_register_address;
    void register_address(const std::string address)
    {
        cout << "register_address " << address << endl;
        last_register_address = address;
    }

    std::string last_set_power_address;
    bool last_set_power_value;
    void set_power(const std::string address, bool value)
    {
        cout << "set_power " << address << " " << to_string(value) << endl;
        last_set_power_address = address;
        last_set_power_value = value;
    }

    std::string last_set_room_temperature_address;
    float last_set_room_temperature_value;
    void set_room_temperature(const std::string address, float value)
    {
        cout << "set_room_temperature " << address << " " << to_string(value) << endl;
        last_set_room_temperature_address = address;
        last_set_room_temperature_value = value;
    }

    std::string last_set_target_temperature_address;
    float last_set_target_temperature_value;
    void set_target_temperature(const std::string address, float value)
    {
        cout << "set_target_temperature " << address << " " << to_string(value) << endl;
        last_set_target_temperature_address = address;
        last_set_target_temperature_value = value;
    }

    std::string last_set_room_humidity_address;
    float last_set_room_humidity_value;
    void set_room_humidity(const std::string address, float value)
    {
        cout << "set_room_humidity " << address << " " << to_string(value) << endl;
        last_set_room_humidity_address = address;
        last_set_room_humidity_value = value;
    }

    std::string last_set_mode_address;
    Mode last_set_mode_mode;
    void set_mode(const std::string address, Mode mode)
    {
        cout << "set_mode " << address << " " << to_string((int)mode) << endl;
        last_set_mode_address = address;
        last_set_mode_mode = mode;
    }

    std::string last_set_fanmode_address;
    FanMode last_set_fanmode_mode;
    void set_fanmode(const std::string address, FanMode fanmode)
    {
        cout << "set_fanmode " << address << " " << to_string((int)fanmode) << endl;
        last_set_fanmode_address = address;
        last_set_fanmode_mode = fanmode;
    }

    void assert_values(const std::string address, bool power, float room_temp, float target_temp, float humidity, Mode mode, FanMode fanmode)
    {
        assert(last_register_address == address);

        assert(last_set_power_address == address);
        assert(last_set_power_value == power);

        assert(last_set_room_temperature_address == address);
        assert(last_set_room_temperature_value == room_temp);

        assert(last_set_target_temperature_address == address);
        assert(last_set_target_temperature_value == target_temp);

        assert(last_set_room_humidity_address == address);
        assert(last_set_room_humidity_value == humidity);

        assert(last_set_mode_address == address);
        assert(last_set_mode_mode == mode);

        assert(last_set_fanmode_address == address);
        assert(last_set_fanmode_mode == fanmode);
    }
};

DebugTarget test_process_data(const std::string &hex)
{
    DebugTarget target;
    auto bytes = hex_to_bytes(hex);
    assert(process_data(bytes, &target) == DataResult::Clear);
    return target;
}
