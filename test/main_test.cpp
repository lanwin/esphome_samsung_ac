#include <vector>
#include <iostream>
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
};

int main(int argc, char *argv[])
{
    DebugTarget target;
    auto data = hex_to_bytes("32c8dec70101000000000000d134");
    process_message(data, &target);
    data = hex_to_bytes("32c8f0860100000000000008b734");
    process_message(data, &target);
    data = hex_to_bytes("32c8add1ff000000000000004b34");
    process_message(data, &target);
    data = hex_to_bytes("32c8008f00000000000000004734");
    process_message(data, &target);
    data = hex_to_bytes("3200c8210300000600000000ec34");
    process_message(data, &target);
    data = hex_to_bytes("32c8dec70101000000000000d134");
    process_message(data, &target);
    data = hex_to_bytes("32c8f0860100000000000008b734");
    process_message(data, &target);
    data = hex_to_bytes("32c8add1ff000000000000004b34");
    process_message(data, &target);
    data = hex_to_bytes("32c800c0080000004b004d4b4d34");
    process_message(data, &target);
    data = hex_to_bytes("3200c82f00f0010b010201051a34");
    process_message(data, &target);
    data = hex_to_bytes("32c8dec70101000000000000d134");
    process_message(data, &target);
    data = hex_to_bytes("32c8f0860100000000000008b734");
    process_message(data, &target);
    data = hex_to_bytes("3200c84020000000408900402134");
    process_message(data, &target);
    data = hex_to_bytes("3200c8204d51500001100051e434");
    process_message(data, &target);
};

// g++ *.cpp -o test.exe && test.exe
