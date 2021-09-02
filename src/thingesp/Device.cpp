#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include "../PubSubClient/PubSubClient.h"

class DeviceData{
public:

    /*
     * Set WiFi SSID and Password
     */
    void SetWiFi(const char *_ssid, const char *_ssid_password)
    {
        wifi_configured = true;
        ssid = _ssid;
        ssid_password = _ssid_password;
    }


    /*
     *  Generate DeviceName and Topic Name(s)
     */
    void genMetaData(){
       this->outName = projectName + "@" + username;
       this->topic = projectName + "/" + username;
    }

    String projectName;
    const char* username;
    const char* credentials;

    String outName;
    String topic;

    const char* ssid;
    const char* ssid_password;

    bool wifi_configured = false;


    /*
     * Check if TLS Disabled or not
     */
    #if  !defined(_DISABLE_TLS_) && !defined(ESP32)
        unsigned int MQTT_PORT = 1899; // tls port
    #else
        unsigned int MQTT_PORT = 1893; // non-tls port
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