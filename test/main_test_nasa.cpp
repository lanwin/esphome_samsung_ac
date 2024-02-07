#include "test_stuff.h"
#include "../components/samsung_ac/protocol_nasa.h"

using namespace std;
using namespace esphome::samsung_ac;

void test_nasa_1()
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

void test_nasa_2()
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

void test_process_data()
{
}

Packet test_foo(ProtocolRequest request, const std::string &address)
{
    Packet packet = Packet::createa_partial(Address::parse(address), DataType::Request);

    if (request.mode)
    {
        request.power = true; // ensure system turns on when mode is set

        MessageSet mode(MessageNumber::ENUM_in_operation_mode);
        mode.value = (int)request.mode.value();
        packet.messages.push_back(mode);
    }

    if (request.power)
    {
        MessageSet power(MessageNumber::ENUM_in_operation_power);
        power.value = request.power.value() ? 1 : 0;
        packet.messages.push_back(power);
    }

    return packet;
}

int main(int argc, char *argv[])
{
    ProtocolRequest request;
    request.power = true;
    request.mode = Mode::Heat;

    auto p = test_foo(request, "");

    std::cout << p.to_string() << std::endl;

    return 0;
    test_nasa_1();
    test_nasa_2();
    test_process_data();
};
