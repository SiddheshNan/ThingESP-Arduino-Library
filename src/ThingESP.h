#define MQTT_MAX_PACKET_SIZE 1024
#include "./ThingESPClient.cpp"

#if defined(ESP8266)
    class ThingESP8266 : public thing_esp::ThingESPClient {
        public:
            ThingESP8266(const char* _username, const char* _projectName, const char* _credentials) : ThingESPClient(_username, _projectName, _credentials)
            {}
    };
#elif defined(ESP32)
    class ThingESP32 : public thing_esp::ThingESPClient {
        public:
            ThingESP32(const char* _username, const char* _projectName, const char* _credentials) : ThingESPClient(_username, _projectName, _credentials)
            {}
    };
#endif



