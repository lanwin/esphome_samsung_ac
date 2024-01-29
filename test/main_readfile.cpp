#include "test_stuff.h"
#include "../components/samsung_ac/protocol_non_nasa.h"
#include <fstream>

using namespace std;
using namespace esphome::samsung_ac;

int main(int argc, char *argv[])
{
    std::ifstream file("test.txt");
    std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    DebugTarget target;
    std::vector<uint8_t> data_;
    for (int i = 0; i < str.size(); i += 2)
    {
        uint8_t c = hex_to_int(str.substr(i, 2));
        // cout << long_to_hex(c) << std::endl;
        if (data_.size() == 0 && c != 0x32)
            continue; // skip until start-byte found

        data_.push_back(c);

        if (process_data(data_, &target) == DataResult::Clear)
        {
            data_.clear();
            break; // wait for next loop
        }
    }
};
