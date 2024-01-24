#include <windows.h>

#include "test_stuff.h"
#include "../components/samsung_ac/protocol_nasa.h"

using namespace std;
using namespace esphome::samsung_ac;

void process_data(std::vector<uint8_t> &data)
{
    Packet packet;
    if (packet.decode(data) != DecodeResult::Ok)
        return;

    if (packet.sa.to_string() != "20.00.02" &&
        packet.da.to_string() != "20.00.02")
        return;

    // if (packet.commad.dataType == DataType::Notification)
    //   return;

    cout << packet.to_string() << endl;
}

int main(int argc, char *argv[])
{
    HANDLE hComm = CreateFileA("COM5", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hComm == INVALID_HANDLE_VALUE)
        throw new std::invalid_argument("com port kann nicht geöffnet werden");

    DCB serialParams = {0};
    serialParams.DCBlength = sizeof(serialParams);
    GetCommState(hComm, &serialParams);
    serialParams.BaudRate = CBR_9600;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = EVENPARITY;
    SetCommState(hComm, &serialParams);

    bool receiving_ = false;
    std::vector<uint8_t> data_;
    do
    {
        char read_buffer[1];
        DWORD bytes_read = 0;
        ReadFile(hComm, &read_buffer, sizeof(read_buffer), &bytes_read, NULL);
        if (bytes_read != 1)
        {
            Sleep(10);
            continue;
        }
        uint8_t c = (uint8_t)read_buffer[0];

        // std::cout << bytes_to_hex(data_) << std::endl;
        if (c == 0x32 && !receiving_) // start-byte found
        {
            receiving_ = true;
            data_.clear();
        }
        if (receiving_)
        {
            data_.push_back(c);
            if (data_.size() < 14 || c != 0x34)
                continue; // endbyte not found
            receiving_ = false;

            process_data(data_);
        }
    } while (true);

    CloseHandle(hComm);
};

// g++ *.cpp -o test.exe && test.exe
