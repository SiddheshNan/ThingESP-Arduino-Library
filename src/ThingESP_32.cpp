#pragma once

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#endif

#include <stdint.h>
#include <string.h>

#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

#include "thingesp/Logger.cpp"
#include "thingesp/Device.cpp"
#include "thingesp/RateLimiter.cpp"

String HandleResponse(String query) __attribute__((weak));

class ThingESP32 : public DeviceData, public RateLimiter
{
public:
    ThingESP32(const char* _username, const char* _projectName, const char* _credentials) : client(espClient)
    {
        username = _username;
        projectName = _projectName;
        credentials = _credentials;

        genMetaData();
    };


    void sendMsg(String number, String msg)
    {
        if (is_rate_limited()) return;

        DynamicJsonDocument data_out(1024);
        data_out["action"] = "device_call";
        data_out["to_number"] = number;
        data_out["msg"] = msg;
        String outdata;
        serializeJson(data_out, outdata);
        publishMSG(outdata.c_str());
    }

    void initDevice()
    {
        if (wifi_configured) {

            LOG_VALUE("WiFi", "Connecting to: ", ssid)

            WiFi.begin(ssid, ssid_password);

            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
            }

            LOG("WiFi", "Connected successfully");
            LOG_VALUE("WiFi","IP address: ", WiFi.localIP());


        }

        randomSeed(micros());

        client.setServer(MQTT_SERVER, MQTT_PORT);
        client.setCallback([this](char *topic, byte *payload, unsigned int length) {
            callback(topic, payload, length);
        });
    }

    void Handle()
    {
        if (!client.connected())
        {
            while (!client.connected())
            {
                LOG("SOCKET", "Attempting connection to ThingESP")

                if (client.connect(outName.c_str(), outName.c_str(), credentials))
                {
                    LOG("SOCKET", "Connected to ThingESP successfully")
                    client.subscribe(topic.c_str());
                    publishMSG(get_rate_limits_msg());
                }
                else
                {
                    LOG_VALUE("SOCKET", "Error connecting to ThingESP! Error code: ", client.state());
                    if (client.state() == 5)
                        LOG("SOCKET","Please check your username, project name or credentials! ")
                    LOG("SOCKET",  "Trying again in 10 seconds..");
                    delay(10000);
                }
            }
        }
        client.loop();
    }


    void setCallback( String(*clbk)(String) ){
        this->callbackFunction = clbk;
    }

private:

    /*
     * the callback function
     */
    String (*callbackFunction)(String);


    /*
     * the WiFi Client
     */
    WiFiClient espClient;



    /*
    * PubSubClient for MQTT
    */
    PubSubClient client;


    void publishMSG(const char* _msg)
    {
        client.publish(topic.c_str(), _msg);
    }

    void callback(char *topic, byte *payload, unsigned int length)
    {
        String msg;

        for (int i = 0; i < length; i++)
            msg.concat((char)payload[i]);

        onMessage(msg);
    }


    void onMessage(String& data)
    {

        DynamicJsonDocument data_in(1024);
        DynamicJsonDocument data_out(1024);
        deserializeJson(data_in, data);

        String incoming_action = data_in["action"];

        if (incoming_action == "query")
        {
            data_out["msg_id"] = data_in["msg_id"];
            data_out["action"] = "returned_api_response";
            String query = data_in["query"];

            #ifndef _DISABLE_LOWER_CASE_
                        query.toLowerCase();
            #endif

            LOG_VALUE("MSG", "Query: ", query);

            String resp = !!HandleResponse ? HandleResponse(query) : this->callbackFunction(query);

            LOG_VALUE("MSG", "Response: ", resp);

            data_out["returned_api_response"] = resp;

            String out_msg;
            serializeJson(data_out, out_msg);
            publishMSG(out_msg.c_str());

        }
        else if (incoming_action == "RATE_LIMITS_INFO"){
            set_rate_limit((unsigned int)data_in["delay"]);
        }
    };

};