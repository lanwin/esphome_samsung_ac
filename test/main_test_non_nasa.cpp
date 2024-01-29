#include "test_stuff.h"
#include "../components/samsung_ac/protocol_non_nasa.h"

using namespace std;
using namespace esphome::samsung_ac;

std::vector<uint8_t> create(uint8_t src, uint8_t dst)
{
    std::vector<uint8_t> data;

    for (int i = 0; i < 14; i++)
    {
        data.push_back(0);
    }

    data[0] = 0x32;
    data[1] = src;
    data[2] = dst;

    data[3] = 0x20; // cmd

    uint8_t target_temp = 22;
    uint8_t room_temp = 26;
    uint8_t pipe_in = 25;

    uint8_t pipe_out = 26;

    data[4] = target_temp + 55;
    data[5] = room_temp + 55;
    data[6] = pipe_in + 55;

    data[11] = pipe_out + 55;

    uint8_t a = 8;
    uint8_t b = 16;

    cout << "a    " << std::bitset<8>(a) << endl;
    cout << "b    " << std::bitset<8>(b) << endl;

    uint8_t test = 0;

    test |= a & 0b1111;
    test |= b << 4 & 0b11110000;

    cout << "test " << std::bitset<8>(test) << endl;

    uint8_t a2 = test & 0b00001111;
    uint8_t b2 = (test & 0b11110000) >> 4;

    cout << "a2   " << std::bitset<8>(a2) << " " << std::to_string(a2) << endl;
    cout << "b2   " << std::bitset<8>(b2) << " " << std::to_string(b2) << endl;

    /*

                uint8_t fanspeed = data[7] & 0b00001111;
                bool bladeswing = (data[7] & 0b11110000) == 0xD0;
                bool power = data[8] & 0b10000000;
                uint8_t mode = data[8] & 0b00111111;
                uint8_t pipe_out = data[11] - 55;
    */

    // chk

    data[13] = 0x34;

    return data;
}

NonNasaDataPacket test_decode(std::string data)
{
    NonNasaDataPacket p;
    auto bytes = hex_to_bytes(data);
    assert(p.decode(bytes) == DecodeResult::Ok);
    std::cout << p.to_string() << std::endl;
    return p;
}

void test_decoding()
{
    auto p = test_decode("3200c8204b504e000110004ee234");
    assert(p.command20.power == false);
    assert(p.command20.target_temp == 20);
    assert(p.command20.room_temp == 25);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::Auto);
    assert(p.command20.mode == NonNasaMode::Heat);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204b4f4efd8110004e8034");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 20);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::High);
    assert(p.command20.mode == NonNasaMode::Heat);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204b4f4efc8110004e8134");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 20);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::Medium);
    assert(p.command20.mode == NonNasaMode::Heat);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204b4f4efa8110004e8734");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 20);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::Low);
    assert(p.command20.mode == NonNasaMode::Heat);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204f4f4ef8a21c004eae34");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 24);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::Auto);
    assert(p.command20.mode == NonNasaMode::Auto);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204f4f4efd821c004e8b34");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 24);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::High);
    assert(p.command20.mode == NonNasaMode::Cool);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);

    p = test_decode("3200c8204f4f4efd821c004e8b34");
    assert(p.command20.power == true);
    assert(p.command20.target_temp == 24);
    assert(p.command20.room_temp == 24);
    assert(p.command20.pipe_in == 23);
    assert(p.command20.pipe_out == 23);
    assert(p.command20.fanspeed == NonNasaFanspeed::High);
    assert(p.command20.mode == NonNasaMode::Cool);
    assert(p.command20.wind_direction == NonNasaWindDirection::Stop);
}

NonNasaRequest create_request()
{
    NonNasaRequest p;
    p.dst = "00";
    p.power = false;
    p.target_temp = 20;
    p.fanspeed = NonNasaFanspeed::Auto;
    p.mode = NonNasaMode::Auto;
    return p;
}

void test_request(NonNasaRequest request, std::string expected)
{
    auto actual = bytes_to_hex(request.encode());
    assert_str(actual, expected);
}

void test_encoding()
{
    NonNasaRequest req;

    req = create_request();
    req.dst = "00";
    req.power = true;
    req.room_temp = 23;
    req.target_temp = 24;
    req.fanspeed = NonNasaFanspeed::Auto;
    req.mode = NonNasaMode::Fan;
    test_request(req, "32d000b01f171803f4210000a634");

    req = create_request();
    req.power = true;
    test_request(req, "32d000b01f041400f4210000ba34");

    req = create_request();
    req.power = false;
    test_request(req, "32d000b01f041400c42100008a34");

    req = create_request();
    req.fanspeed = NonNasaFanspeed::Auto;
    test_request(req, "32d000b01f041400c42100008a34");
    req = create_request();
    req.fanspeed = NonNasaFanspeed::High;
    test_request(req, "32d000b01f04b400c42100002a34");
    req = create_request();
    req.fanspeed = NonNasaFanspeed::Medium;
    test_request(req, "32d000b01f049400c42100000a34");
    req = create_request();
    req.fanspeed = NonNasaFanspeed::Low;
    test_request(req, "32d000b01f045400c4210000ca34");

    req = create_request();
    req.target_temp = 25;
    test_request(req, "32d000b01f041900c42100008734");

    req = create_request();
    req.mode = NonNasaMode::Auto;
    test_request(req, "32d000b01f041400c42100008a34");
    req = create_request();
    req.mode = NonNasaMode::Cool;
    test_request(req, "32d000b01f041401c42100008b34");
    req = create_request();
    req.mode = NonNasaMode::Dry;
    test_request(req, "32d000b01f041402c42100008834");
    req = create_request();
    req.mode = NonNasaMode::Fan;
    test_request(req, "32d000b01f041403c42100008934");
    req = create_request();
    req.mode = NonNasaMode::Heat;
    test_request(req, "32d000b01f041404c42100008e34");
}

void test_target()
{
    DebugTarget target;

    target = test_process_data("32c8dec70101000000000000d134");
    target.assert_only_address("c8");
    target = test_process_data("32c8f0860100000000000008b734");
    target.assert_only_address("c8");
    target = test_process_data("32c8add1ff000000000000004b34");
    target.assert_only_address("c8");
    target = test_process_data("32c8008f00000000000000004734");
    target.assert_only_address("c8");
    target = test_process_data("32c800c0080000004b004d4b4d34");
    target.assert_only_address("c8");
    target = test_process_data("3200c8210300000600000000ec34");
    target.assert_only_address("00");
    target = test_process_data("3200c82f00f0010b010201051a34");
    target.assert_only_address("00");
    target = test_process_data("3200c84020000000408900402134");
    target.assert_only_address("00");

    target = test_process_data("3200c8204d51500001100051e434");
    target.assert_values("00", false, 26.000000, 22.000000, Mode::Heat, FanMode::Auto);

    target = test_process_data("3200c8204f4f4efd821c004e8b34");
    target.assert_values("00", true, 24.000000, 24.000000, Mode::Cool, FanMode::Hight);
}

void test_previous_data_is_used_correctly()
{
    // Sending package 20 on non nasa requiers to send the previous values
    // these values need to be stored for each address. This test makes sure
    // this process works.
    std::cout << "test_previous_data_is_used_correctly" << std::endl;

    DebugTarget target;
    auto bytes = hex_to_bytes("3200c8204d51500001100051e434");
    assert(process_data(bytes, &target) == DataResult::Clear);

    get_protocol("00")->publish_power_message(&target, "00", false);
    NonNasaRequest request1;
    request1.dst = "00";
    request1.room_temp = 26.000000;
    request1.target_temp = 22.000000;
    request1.power = false;
    request1.fanspeed = NonNasaFanspeed::Auto;
    request1.mode = NonNasaMode::Heat;
    assert_str(target.last_publish_data, bytes_to_hex(request1.encode()));

    bytes = hex_to_bytes("3201c8204f4f4efd821c004e8a34");
    assert(process_data(bytes, &target) == DataResult::Clear);

    get_protocol("01")->publish_power_message(&target, "01", true);
    NonNasaRequest request2;
    request2.dst = "01";
    request2.room_temp = 24.000000;
    request2.target_temp = 24.000000;
    request2.power = true;
    request2.fanspeed = NonNasaFanspeed::High;
    request2.mode = NonNasaMode::Cool;
    assert_str(target.last_publish_data, bytes_to_hex(request2.encode()));
}

int main(int argc, char *argv[])
{
    // test_read_file();
    test_decoding();
    test_encoding();
    test_target();

    test_previous_data_is_used_correctly();
};
