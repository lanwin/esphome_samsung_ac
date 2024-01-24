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
    void register_address(const std::string address)
    {
        cout << "register_address " << address << endl;
    }
    void set_power(const std::string address, bool value)
    {
        cout << "set_power " << address << " " << to_string(value) << endl;
    }
    void set_room_temperature(const std::string address, float value)
    {
        cout << "set_room_temperature " << address << " " << to_string(value) << endl;
    }
    void set_target_temperature(const std::string address, float value)
    {
        cout << "set_target_temperature " << address << " " << to_string(value) << endl;
    }
    void set_mode(const std::string address, Mode mode)
    {
        cout << "set_mode " << address << " " << to_string((int)mode) << endl;
    }
    void set_room_humidity(const std::string address, float value)
    {
        cout << "set_room_humidity " << address << " " << to_string(value) << endl;
    }
    void set_fanmode(const std::string address, FanMode fanmode)
    {
        cout << "set_fanmode " << address << " " << to_string((int)fanmode) << endl;
    }
};