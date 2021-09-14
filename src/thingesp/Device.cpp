#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include "../PubSubClient/PubSubClient.h"


namespace thing_esp {
    class DeviceData {
    public:

        /*
         * Set WiFi SSID and Password
         */
        void SetWiFi(const char *_ssid, const char *_ssid_password) {
            wifi_configured = true;
            ssid = _ssid;
            ssid_password = _ssid_password;
        }


        /*
         *  Generate DeviceName and Topic Name(s)
         */
        void genMetaData() {
            this->outName = projectName + "@" + username;
            this->topic = projectName + "/" + username;
            this->c_topic = topic.c_str();
        }

        String projectName;
        const char *username;
        const char *credentials;

        String outName;
        String topic;
        const char* c_topic;

        const char *ssid;
        const char *ssid_password;

        bool wifi_configured = false;


        /*
         * Check if TLS Disabled or not
         */
        #if !defined(_MQTT_PORT_)

            #if  !defined(_DISABLE_TLS_) && !defined(ESP32)
                    unsigned int MQTT_PORT = 1899; // tls port
            #else
                    unsigned int MQTT_PORT = 1893; // non-tls port
            #endif

        #else
            unsigned int MQTT_PORT = _MQTT_PORT_;
        #endif



        /*
         * ThingESP server DNS
         */
        #ifndef _THINGESP_SERVER_
                const char *MQTT_SERVER = "thingesp.siddhesh.me";
        #else
                const char *MQTT_SERVER = _THINGESP_SERVER_;
        #endif


    };
}