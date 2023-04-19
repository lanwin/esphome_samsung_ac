#pragma once

#include <vector>
#include <iostream>
#include "protocol.h"

namespace esphome
{
    namespace samsung_ac
    {
        enum class AddressClass : uint8_t
        {
            Outdoor = 16,                      // 0x00000010
            HTU = 17,                          // 0x00000011
            Indoor = 32,                       // 0x00000020
            ERV = 48,                          // 0x00000030
            Diffuser = 53,                     // 0x00000035
            MCU = 56,                          // 0x00000038
            RMC = 64,                          // 0x00000040
            WiredRemote = 80,                  // 0x00000050
            PIM = 88,                          // 0x00000058
            SIM = 89,                          // 0x00000059
            Peak = 90,                         // 0x0000005A
            PowerDivider = 91,                 // 0x0000005B
            OnOffController = 96,              // 0x00000060
            WiFiKit = 98,                      // 0x00000062
            CentralController = 101,           // 0x00000065
            DMS = 106,                         // 0x0000006A
            JIGTester = 128,                   // 0x00000080
            BroadcastSelfLayer = 176,          // 0x000000B0
            BroadcastControlLayer = 177,       // 0x000000B1
            BroadcastSetLayer = 178,           // 0x000000B2
            BoradcastCS = 179,                 // 0x000000B3
            BroadcastControlAndSetLayer = 179, // 0x000000B3
            BroadcastModuleLayer = 180,        // 0x000000B4
            BoradcastCSM = 183,                // 0x000000B7
            BroadcastLocalLayer = 184,         // 0x000000B8
            BroadcastCSML = 191,               // 0x000000BF
            Undefiend = 255,                   // 0x000000FF
        };

        enum class PacketType : uint8_t
        {
            StandBy = 0,
            Normal = 1,
            Gathering = 2,
            Install = 3,
            Download = 4
        };

        enum class DataType : uint8_t
        {
            Undefined = 0,
            Read = 1,
            Write = 2,
            Request = 3,
            Notification = 4,
            Response = 5,
            Ack = 6,
            Nack = 7
        };

        enum MessageSetType : uint8_t
        {
            Enum = 0,
            Variable = 1,
            LongVariable = 2,
            Structure = 3
        };

        enum class MessageNumber : uint16_t
        {
            Undefiend = 0,
            ENUM_in_operation_power = 0x4000,
            ENUM_in_operation_mode = 0x4001,
            VAR_in_temp_room_f = 0x4203,
            VAR_in_temp_target_f = 0x4201
        };

        struct Address
        {
            AddressClass klass;
            uint8_t channel;
            uint8_t address;
            uint8_t size = 3;

            static Address parse(const std::string &str);
            static Address get_my_address();

            void decode(std::vector<uint8_t> &data, unsigned int index);
            void encode(std::vector<uint8_t> &data);
            std::string to_string();
        };

        struct Command
        {
            bool packetInformation = true;
            uint8_t protocolVersion = 2;
            uint8_t retryCount = 0;
            PacketType packetType = PacketType::StandBy;
            DataType dataType = DataType::Undefined;
            uint8_t packetNumber = 0;

            uint8_t size = 3;

            void decode(std::vector<uint8_t> &data, unsigned int index);
            void encode(std::vector<uint8_t> &data);
            std::string to_string();
        };

        struct Buffer
        {
            uint8_t size;
            uint8_t data[255];
        };

        struct MessageSet
        {
            MessageNumber messageNumber = MessageNumber::Undefiend;
            MessageSetType type = Enum;
            union
            {
                int value;
                Buffer structure;
            };
            uint16_t size = 2;

            MessageSet(MessageNumber messageNumber)
            {
                this->messageNumber = messageNumber;
                // this->deviceType = (NMessageSet.DeviceType) (((int) messageNumber & 57344) >> 13);
                this->type = (MessageSetType)(((uint32_t)messageNumber & 1536) >> 9);
                // this->_msgIndex = (ushort) ((uint) messageNumber & 511U);
            }

            static MessageSet decode(std::vector<uint8_t> &data, unsigned int index, int capacity);

            void encode(std::vector<uint8_t> &data);
            std::string to_string();
        };

        struct Packet
        {
            Address sa;
            Address da;
            Command commad;
            std::vector<MessageSet> messages;

            static Packet create(Address da, DataType dataType, MessageNumber messageNumber, int value);
            static Packet createa_partial(Address da, DataType dataType);

            bool decode(std::vector<uint8_t> &data);
            std::vector<uint8_t> encode();
            std::string to_string();
        };

        void process_nasa_message(std::vector<uint8_t> data, MessageTarget *target);

        std::string bytes_to_hex(const std::vector<uint8_t> &data);
        std::vector<uint8_t> hex_to_bytes(const std::string &hex);

        class NasaProtocol : public Protocol
        {
        public:
            NasaProtocol() = default;

            std::vector<uint8_t> get_power_message(const std::string &address, bool value) override;
            std::vector<uint8_t> get_target_temp_message(const std::string &address, float value) override;
            std::vector<uint8_t> get_mode_message(const std::string &address, Mode value) override;
        };

    } // namespace samsung_ac
} // namespace esphome
