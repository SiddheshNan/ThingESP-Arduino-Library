#pragma once

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#endif

#ifndef _DISABLE_TLS_
    #include <WiFiClientSecure.h>
#endif

#include <stdint.h>
#include <string.h>

#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

#include "thingesp/Logger.cpp"
#include "thingesp/Device.cpp"
#include "thingesp/RateLimiter.cpp"
#include "thingesp/Message.cpp"

namespace thing_esp {
    class ThingESPClient : public DeviceData, public Message {
    public:
        ThingESPClient(const char *_username, const char *_projectName, const char *_credentials) : client(espClient) {

            #if !defined(_DISABLE_TLS_) && !defined(ESP32)
                espClient.setInsecure();
                delay(2);
            #endif

            username = _username;
            projectName = _projectName;
            credentials = _credentials;

            this->genMetaData();
        };

        void initDevice() {
            if (wifi_configured) {

                LOG_VALUE("WiFi", "Connecting to: ", ssid)

                WiFi.begin(ssid, ssid_password);

                while (WiFi.status() != WL_CONNECTED) {
                    delay(500);
                }

                LOG("WiFi", "Connected successfully");
                LOG_VALUE("WiFi", "IP address: ", WiFi.localIP());


            }

            randomSeed(micros());

            client.setServer(MQTT_SERVER, MQTT_PORT);
            client.setCallback([this](char *topic, byte *payload, unsigned int length) {
                callback(topic, payload, length);
            });
        }

        void Handle() {
            if (!client.connected()) {
                while (!client.connected()) {
                    LOG("SOCKET", "Attempting connection to ThingESP")

                    if (client.connect(outName.c_str(), outName.c_str(), credentials)) {
                        LOG("SOCKET", "Connected to ThingESP successfully")
                        client.subscribe(c_topic);
                        publishMSG(get_rate_limits_msg());
                    } else {
                        LOG_VALUE("SOCKET", "Error connecting to ThingESP! Error code: ", client.state());
                        if (client.state() == 5) {
                            LOG("SOCKET", "Please check your username, project name or credentials! ");
                        }
                        LOG("SOCKET", "Trying again in 10 seconds..");
                        delay(10000);
                    }
                }
            }
            client.loop();
        }


    protected:

        /*
         * the callback function
         */
        String (*callbackFunction)(String) override;


        /*
         * the WiFi Client
         */
        #if !defined(_DISABLE_TLS_) && !defined(ESP32)
                WiFiClientSecure espClient;
        #else
                WiFiClient espClient;
        #endif


        /*
        * PubSubClient for MQTT
        */
        PubSubClient client;


        void publishMSG(const char *_msg) override {
            client.publish(c_topic, _msg);
        }

        void callback(char *topic, byte *payload, unsigned int length) {
            String msg;

            for (int i = 0; i < length; i++)
                msg.concat((char) payload[i]);

            this->onMessage(msg);
        }


    };

}