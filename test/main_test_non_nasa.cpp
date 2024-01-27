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

void test_encoding()
{
    NonNasaRequest p;
    p.dst = "c8";
    p.power = true;
    p.target_temp = 24;
    p.fanspeed = NonNasaFanspeed::Auto;
    p.mode = NonNasaMode::Cool;
    auto data = bytes_to_hex(p.encode());
    std::cout << "expected: 3200c8204f4f4efd821c004e8b34" << std::endl;
    std::cout << "actual:   " << data << std::endl;
    assert(data == "32d0c8b01f04a601f4210000c134");
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

int main(int argc, char *argv[])
{
    test_decoding();
    // test_encoding();
    test_target();
};
