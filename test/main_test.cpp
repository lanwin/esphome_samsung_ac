#include <vector>
#include <iostream>
#include "../components/samsung_ac/nasa.h"

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
};

void test1()
{
    Address to = Address::parse("20.00.00");
    Packet packet;

    packet = Packet::create(to, DataType::Request, MessageNumber::ENUM_in_operation_power, 1);
    packet.commad.packetNumber = 1;
    std::cout << packet.to_string() << std::endl;
    std::cout << bytes_to_hex(packet.encode()) << std::endl;

    packet = Packet::create(to, DataType::Request, MessageNumber::ENUM_in_operation_power, 1);
    packet.commad.packetNumber = 1;
    std::cout << packet.to_string() << std::endl;
    std::cout << bytes_to_hex(packet.encode()) << std::endl;

    std::cout << "32001180ff00200000c0130101400001c24734 expected" << std::endl;
}

void test2()
{
    auto data = hex_to_bytes("32001280ff00200002c013f201420101186e5434");
    Packet packetT;
    packetT.decode(data);
    std::cout << packetT.to_string() << std::endl;

    Packet packet;
    packet = Packet::create(Address::parse("20.00.02"), DataType::Request, MessageNumber::VAR_in_temp_target_f, 28 * 10.0);
    packet.commad.packetNumber = 242;
    std::cout << packet.to_string() << std::endl;
    std::cout << bytes_to_hex(packet.encode()) << std::endl;

    std::cout << "32001280ff00200002c013f201420101186e5434 expected" << std::endl;
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    return 0;
    auto data = hex_to_bytes("32001e200001b000ffc01457044202010e420300f2420500eb420600f5111534");

    // data = HexToBytes("32001e200001b000ffc01406044202010e420300eb420500e6420600e98ba134");

    DebugTarget target;
    process_nasa_message(data, &target);
};

// g++ *.cpp -o test.exe && test.exe
