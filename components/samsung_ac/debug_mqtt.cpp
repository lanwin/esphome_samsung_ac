#include "esphome/core/log.h"
#include "debug_mqtt.h"

#ifdef USE_ESP8266
#include <AsyncMqttClient.h>

AsyncMqttClient *mqtt_client{nullptr};
#endif

namespace esphome
{
    namespace samsung_ac
    {
        bool debug_mqtt_connected()
        {
#ifdef USE_ESP8266
            if (mqtt_client == nullptr)
                return false;

            return mqtt_client->connected();
#else
            return false;
#endif
        }

        void debug_mqtt_connect(const std::string &host, const uint16_t port, const std::string &username, const std::string &password)
        {
#ifdef USE_ESP8266
            if (mqtt_client == nullptr)
            {
                mqtt_client = new AsyncMqttClient();
                mqtt_client->setServer(host.c_str(), port);
                if (username.length() > 0)
                    mqtt_client->setCredentials(username.c_str(), password.c_str());
            }

            if (!mqtt_client->connected())
                mqtt_client->connect();
#endif
        }

        bool debug_mqtt_publish(const std::string &topic, const std::string &payload)
        {
#ifdef USE_ESP8266
            if (mqtt_client == nullptr)
                return false;

            return mqtt_client->publish(topic.c_str(), 0, false, payload.c_str()) != 0;
#else
            return true;
#endif
        }
    } // namespace samsung_ac
} // namespace esphome
