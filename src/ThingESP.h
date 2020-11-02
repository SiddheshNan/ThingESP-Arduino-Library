#define MQTT_MAX_PACKET_SIZE 1024

#if defined(ESP8266)
#include "ThingESP_8266.cpp"
#elif defined(ESP32)
#include "ThingESP_32.cpp"
#endif
