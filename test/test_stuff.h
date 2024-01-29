#include <vector>
#include <iostream>
#include <bitset>
#include <cassert>
#include <optional>

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

    std::string last_publish_data;
    void publish_data(std::vector<uint8_t> &data)
    {
        last_publish_data = bytes_to_hex(data);
        cout << "> publish_data " << last_publish_data << endl;
    }

    std::string last_register_address;
    void register_address(const std::string address)
    {
        cout << "> register_address " << address << endl;
        last_register_address = address;
    }

    std::string last_set_power_address;
    bool last_set_power_value;
    void set_power(const std::string address, bool value)
    {
        cout << "> " << address << " set_power=" << to_string(value) << endl;
        last_set_power_address = address;
        last_set_power_value = value;
    }

    std::string last_set_room_temperature_address;
    float last_set_room_temperature_value;
    void set_room_temperature(const std::string address, float value)
    {
        cout << "> " << address << " set_room_temperature=" << to_string(value) << endl;
        last_set_room_temperature_address = address;
        last_set_room_temperature_value = value;
    }

    std::string last_set_target_temperature_address;
    float last_set_target_temperature_value;
    void set_target_temperature(const std::string address, float value)
    {
        cout << "> " << address << " set_target_temperature=" << to_string(value) << endl;
        last_set_target_temperature_address = address;
        last_set_target_temperature_value = value;
    }

    std::string last_set_room_humidity_address;
    float last_set_room_humidity_value;
    void set_room_humidity(const std::string address, float value)
    {
        cout << "> " << address << " set_room_humidity=" << to_string(value) << endl;
        last_set_room_humidity_address = address;
        last_set_room_humidity_value = value;
    }

    std::string last_set_mode_address;
    Mode last_set_mode_mode;
    void set_mode(const std::string address, Mode mode)
    {
        cout << "> " << address << " set_mode=" << to_string((int)mode) << endl;
        last_set_mode_address = address;
        last_set_mode_mode = mode;
    }

    std::string last_set_fanmode_address;
    FanMode last_set_fanmode_mode;
    void set_fanmode(const std::string address, FanMode fanmode)
    {
        cout << "> " << address << " set_fanmode=" << to_string((int)fanmode) << endl;
        last_set_fanmode_address = address;
        last_set_fanmode_mode = fanmode;
    }

    void assert_only_address(const std::string address)
    {
        assert(last_register_address == address);
        assert(last_set_power_address == "");
        assert(last_set_room_temperature_address == "");
        assert(last_set_target_temperature_address == "");
        assert(last_set_mode_address == "");
        assert(last_set_fanmode_address == "");
    }

    void assert_values(const std::string address, bool power, float room_temp, float target_temp, Mode mode, FanMode fanmode)
    {
        assert(last_register_address == address);

        assert(last_set_power_address == address);
        assert(last_set_power_value == power);

        assert(last_set_room_temperature_address == address);
        assert(last_set_room_temperature_value == room_temp);

        assert(last_set_target_temperature_address == address);
        assert(last_set_target_temperature_value == target_temp);

        assert(last_set_mode_address == address);
        assert(last_set_mode_mode == mode);

        assert(last_set_fanmode_address == address);
        assert(last_set_fanmode_mode == fanmode);
    }

    void assert_values(const std::string address, bool power, float room_temp, float target_temp, Mode mode, FanMode fanmode, float humidity)
    {
        assert_values(address, power, room_temp, target_temp, mode, fanmode);

        assert(last_set_room_humidity_address == address);
        assert(last_set_room_humidity_value == humidity);
    }
};

DebugTarget test_process_data(const std::string &hex)
{
    cout << "test: " << hex << std::endl;
    DebugTarget target;
    auto bytes = hex_to_bytes(hex);
    assert(process_data(bytes, &target) == DataResult::Clear);
    return target;
}

void assert_str(const std::string actual, const std::string expected)
{
    if (actual != expected)
    {
        cout << "actual:   " << actual << std::endl;
        cout << "expected: " << expected << std::endl;
    }
    assert(actual == expected);
}