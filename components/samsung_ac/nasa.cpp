#include <queue>
#include <iostream>
#include "esphome/core/log.h"
#include "nasa.h"

namespace esphome
{
    namespace samsung_ac
    {
        static const char *TAG = "samsung_nasa";

        std::string int_to_hex(int number)
        {
            char str[3];
            sprintf(str, "%02x", number);
            return str;
        }

        std::string bytes_to_hex(const std::vector<uint8_t> &data)
        {
            std::string str;
            for (int i = 0; i < data.size(); i++)
            {
                str += int_to_hex(data[i]);
            }
            return str;
        }

        std::vector<uint8_t> hex_to_bytes(const std::string &hex)
        {
            std::vector<uint8_t> bytes;
            for (unsigned int i = 0; i < hex.length(); i += 2)
            {
                bytes.push_back((uint8_t)strtol(hex.substr(i, 2).c_str(), NULL, 16));
            }
            return bytes;
        }

        uint16_t crc16(std::vector<uint8_t> &data, int startIndex, int length)
        {
            uint16_t crc = 0;
            for (int index = startIndex; index < startIndex + length; ++index)
            {
                crc = crc ^ ((uint16_t)((uint8_t)data[index]) << 8);
                for (uint8_t i = 0; i < 8; i++)
                {
                    if (crc & 0x8000)
                        crc = (crc << 1) ^ 0x1021;
                    else
                        crc <<= 1;
                }
            }
            return crc;
        }

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

        struct Address
        {
            AddressClass klass;
            uint8_t channel;
            uint8_t address;
            uint8_t size = 3;

            static Address get_my_address()
            {
                Address address;
                address.klass = AddressClass::JIGTester;
                address.channel = 0xFF;
                address.address = 0;
                return address;
            }

            void decode(std::vector<uint8_t> &data, unsigned int index)
            {
                klass = (AddressClass)data[index];
                channel = data[index + 1];
                address = data[index + 2];
            }

            void encode(std::vector<uint8_t> &data)
            {
                data.push_back((uint8_t)klass);
                data.push_back(channel);
                data.push_back(address);
            }

            static Address parse(const std::string &str)
            {
                Address address;
                char *pEnd;
                address.klass = (AddressClass)strtol(str.c_str(), &pEnd, 16);
                pEnd++; // .
                address.channel = strtol(pEnd, &pEnd, 16);
                pEnd++; // .
                address.address = strtol(pEnd, &pEnd, 16);
                return address;
            }

            std::string to_string()
            {
                char str[9];
                sprintf(str, "%02x.%02x.%02x", (int)klass, channel, address);
                return str;
            }
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

        struct Command
        {
            bool packetInformation = true;
            uint8_t protocolVersion = 2;
            uint8_t retryCount = 0;
            PacketType packetType = PacketType::StandBy;
            DataType dataType = DataType::Undefined;
            uint8_t packetNumber = 0;

            uint8_t size = 3;

            void decode(std::vector<uint8_t> &data, unsigned int index)
            {
                packetInformation = ((int)data[index] & 128) >> 7 == 1;
                protocolVersion = (uint8_t)(((int)data[index] & 96) >> 5);
                retryCount = (uint8_t)(((int)data[index] & 24) >> 3);
                packetType = (PacketType)(((int)data[index + 1] & 240) >> 4);
                dataType = (DataType)((int)data[index + 1] & 15);
                packetNumber = data[index + 2];
            }

            void encode(std::vector<uint8_t> &data)
            {
                data.push_back((uint8_t)((((int)packetInformation ? 1 : 0) << 7) + ((int)protocolVersion << 5) + ((int)retryCount << 3)));
                data.push_back((uint8_t)(((int)packetType << 4) + (int)dataType));
                data.push_back(packetNumber);
            }

            std::string to_string()
            {
                std::string str;
                str += "{";
                str += "PacketInformation: " + std::to_string(packetInformation) + ";";
                str += "ProtocolVersion: " + std::to_string(protocolVersion) + ";";
                str += "RetryCount: " + std::to_string(retryCount) + ";";
                str += "PacketType: " + std::to_string((int)packetType) + ";";
                str += "DataType: " + std::to_string((int)dataType) + ";";
                str += "PacketNumber: " + std::to_string(packetNumber);
                str += "}";
                return str;
            }
        };

        struct Buffer
        {
            uint8_t size;
            uint8_t data[255];
        };

        enum class MessageNumber : uint16_t
        {
            Undefiend = 0,
            ENUM_in_operation_power = 0x4000,
            VAR_in_temp_room_f = 0x4203
        };

        enum MessageSetType : uint8_t
        {
            Enum = 0,
            Variable = 1,
            LongVariable = 2,
            Structure = 3
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

            static MessageSet decode(std::vector<uint8_t> &data, unsigned int index, int capacity)
            {
                MessageSet set = MessageSet((MessageNumber)((uint32_t)data[index] * 256U + (uint32_t)data[index + 1]));
                switch (set.type)
                {
                case Enum:
                    set.value = (int)data[index + 2];
                    set.size = 3;
                    break;
                case Variable:
                    set.value = (int)data[index + 2] << 8 | (int)data[index + 3];
                    set.size = 4;
                    break;
                case LongVariable:
                    set.value = (int)data[index + 2] << 24 | (int)data[index + 3] << 16 | (int)data[index + 4] << 8 | (int)data[index + 5];
                    set.size = 6;
                    break;

                case Structure:
                    if (capacity != 1)
                    {
                        ESP_LOGE(TAG, "structure messages can only have one message but is %d", capacity);
                        return set;
                    }
                    Buffer buffer;
                    set.size = data.size() - index - 3; // 3=end bytes
                    buffer.size = set.size - 2;
                    for (int i = 0; i < buffer.size; i++)
                    {
                        buffer.data[i] = data[i];
                    }
                    set.structure = buffer;
                    break;
                default:
                    ESP_LOGE(TAG, "Unkown type");
                }

                return set;
            };

            void encode(std::vector<uint8_t> &data)
            {
                uint16_t messageNumber = (uint16_t)this->messageNumber;
                data.push_back((uint8_t)((messageNumber >> 8) & 0xff));
                data.push_back((uint8_t)(messageNumber & 0xff));

                switch (type)
                {
                case Enum:
                    data.push_back((uint8_t)value);
                    break;
                case Variable:
                    data.push_back((uint8_t)(value & 0xff));
                    data.push_back((uint8_t)((value >> 8) & 0xff));
                    break;
                case LongVariable:
                    // Todo
                    break;

                case Structure:
                    for (int i = 0; i < structure.size; i++)
                    {
                        data.push_back(structure.data[i]);
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Unkown type");
                }
            }

            std::string to_string()
            {
                switch (type)
                {
                case Enum:
                    return "Enum " + int_to_hex((uint16_t)messageNumber) + " " + std::to_string(value);
                case Variable:
                    return "Variable " + int_to_hex((uint16_t)messageNumber) + " " + std::to_string(value);
                case LongVariable:
                    return "LongVariable " + int_to_hex((uint16_t)messageNumber) + " " + std::to_string(value);
                case Structure:
                    return "Structure #" + int_to_hex((uint16_t)messageNumber) + " " + std::to_string(structure.size);
                default:
                    return "Unknown";
                }
            }
        };

        static int _packetCounter = 0;

        struct Packet
        {
            Address sa;
            Address da;
            Command commad;
            std::vector<MessageSet> messages;

            static Packet create(Address da, DataType dataType, MessageNumber messageNumber, int value)
            {
                Packet packet = createa_partial(da, dataType);
                MessageSet message(messageNumber);
                message.value = value;
                packet.messages.push_back(message);
                return packet;
            }

            static Packet createa_partial(Address da, DataType dataType)
            {
                Packet packet;
                packet.sa = Address::get_my_address();
                packet.da = da;
                packet.commad.packetInformation = true;
                packet.commad.packetType = PacketType::Normal;
                packet.commad.dataType = dataType;
                packet.commad.packetNumber = _packetCounter++;
                return packet;
            }

            bool decode(std::vector<uint8_t> &data)
            {
                if (data[0] != 0x32)
                {
                    ESP_LOGV(TAG, "invalid start byte");
                    return false;
                }

                if (data[data.size() - 1] != 0x34)
                {
                    ESP_LOGV(TAG, "invalid start byte");
                    return false;
                }

                if (data.size() < 16 || data.size() > 1500)
                {
                    ESP_LOGV(TAG, "data out of range");
                    return false;
                }

                int size = (int)data[1] << 8 | (int)data[2];

                if (size + 2 != data.size())
                {
                    ESP_LOGV(TAG, "unexpected size - got %d but should be %d", size, data.size() - 2);
                    return false;
                }

                uint16_t crc_actual = crc16(data, 3, size - 4);
                uint16_t crc_expected = (int)data[data.size() - 3] << 8 | (int)data[data.size() - 2];
                if (crc_expected != crc_actual)
                {
                    ESP_LOGV(TAG, "invalid crc - got %d but should be %d", crc_actual, crc_expected);
                    return false;
                }

                unsigned int cursor = 3;

                sa.decode(data, cursor);
                cursor += sa.size;

                da.decode(data, cursor);
                cursor += da.size;

                commad.decode(data, cursor);
                cursor += commad.size;

                int capacity = (int)data[cursor];
                cursor++;

                messages.clear();
                for (int i = 1; i <= capacity; ++i)
                {
                    MessageSet set = MessageSet::decode(data, cursor, capacity);
                    messages.push_back(set);
                    cursor += set.size;
                }

                return true;
            };

            std::vector<uint8_t> encode()
            {
                std::vector<uint8_t> data;
                data.push_back(0x32);
                data.push_back(0); // size
                data.push_back(0); // size
                sa.encode(data);
                da.encode(data);
                commad.encode(data);

                data.push_back((uint8_t)messages.size());
                for (int i = 0; i < messages.size(); i++)
                {
                    messages[i].encode(data);
                }

                int endPosition = data.size() + 1;
                data[1] = (uint8_t)(endPosition >> 8);
                data[2] = (uint8_t)(endPosition & (int)0xFF);

                uint16_t checksum = crc16(data, 3, endPosition - 4);
                data.push_back((uint8_t)((unsigned int)checksum >> 8));
                data.push_back((uint8_t)((unsigned int)checksum & (unsigned int)0xFF));

                data.push_back(0x34);
                return data;
            }

            std::string to_string()
            {
                std::string str;
                str += "#Packet Sa:" + sa.to_string() + " Da:" + da.to_string() + "\n";
                str += "Command: " + commad.to_string() + "\n";

                for (int i = 0; i < messages.size(); i++)
                {
                    str += "Message: " + messages[i].to_string() + "\n";
                }

                return str;
            }
        };

        void test()
        {
            Address to = Address::parse("20.00.00");

            Packet packet;

            std::cout << "32001180ff00200000c0130101400001c24734 expected" << std::endl;

            packet = Packet::create(to, DataType::Request, MessageNumber::ENUM_in_operation_power, 1);
            packet.commad.packetNumber = 1;
            std::cout << packet.to_string() << std::endl;
            std::cout << bytes_to_hex(packet.encode()) << std::endl;

            packet = Packet::create(to, DataType::Request, MessageNumber::ENUM_in_operation_power, 1);
            packet.commad.packetNumber = 1;
            std::cout << packet.to_string() << std::endl;
            std::cout << bytes_to_hex(packet.encode()) << std::endl;

            // std::cout << HexToBytes(Create(to, Request, 0x4000, 1).Encode()) << std::endl;

            // std::cout << HexToBytes(foo().Encode()) << std::endl;
        }

        std::vector<uint8_t> NasaProtocol::get_power_message(const std::string &address, bool value)
        {
            Address to = Address::parse(address);
            return Packet::create(to, DataType::Request, MessageNumber::ENUM_in_operation_power, value ? 1 : 0).encode();
        }

        Packet packet_;
        void process_nasa_message(std::vector<uint8_t> data, MessageTarget *target)
        {
            if (packet_.decode(data) == false)
                return;

            if (packet_.commad.packetType != PacketType::Normal)
                return;

            target->register_address(packet_.sa.to_string());

            for (int i = 0; i < packet_.messages.size(); i++)
            {
                MessageSet &message = packet_.messages[i];
                switch (message.messageNumber)
                {
                case MessageNumber::VAR_in_temp_room_f: // VAR_in_temp_room_f unit = 'Celsius'
                {
                    double temp = (double)message.value / (double)10;
                    ESP_LOGW(TAG, "temp s:%s d:%s %f°", packet_.sa.to_string().c_str(), packet_.da.to_string().c_str(), temp);
                    target->set_room_temperature(packet_.sa.to_string(), temp);
                    break;
                }

                case MessageNumber::ENUM_in_operation_power: // ENUM_in_operation_power bool
                {
                    ESP_LOGW(TAG, "power s:%s d:%s %d", packet_.sa.to_string().c_str(), packet_.da.to_string().c_str(), message.value);
                    target->set_power(packet_.sa.to_string(), message.value != 0);
                    break;
                }

                default:
                    continue;
                }
            }

            // ESP_LOGW(TAG, "%s", bytes_to_hex(data).c_str());
            // ESP_LOGW(TAG, "p s:%s d:%s t:%d m:%d", packet_.sa.to_string().c_str(), packet_.da.to_string().c_str(), (uint8_t)packet_.commad.dataType, packet_.messages.size());
            // ESP_LOGW(TAG, "%s", packet_.ToString().c_str());
            //   std::cout << packet.ToString() << std::endl;
        }

    } // namespace samsung_ac
} // namespace esphome
