#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

#ifndef _DISABLE_TLS_
#include <WiFiClientSecure.h>
#endif

String HandleResponse(String query);

class ThingESP8266
{
public:
  ThingESP8266(String username, String deviceName, String password)
  {
      #ifndef _DISABLE_TLS_
        WiFiClientSecure espClient;
        #if !defined(ESP32)
          espClient.setInsecure();
        #endif
        this->espClient = espClient;
        PubSubClient client(this->espClient);
      #else
        WiFiClient espClient;
        PubSubClient client(espClient);
      #endif
   
    delay(2);
    this->client = client;
    this->Username = username;
    this->DeviceName = deviceName;
    this->Password = password;
  };

  void SetWiFi(const char *ssID, const char *ssID_password)
  {
    this->ssid = ssID;
    this->ssid_password = ssID_password;
  }

  void logic(String data)
  {
    DynamicJsonDocument data_in(1024);
    DynamicJsonDocument data_out(1024);
    deserializeJson(data_in, data);

    if (data_in["action"] == "query")
    {
      data_out["msg_id"] = data_in["msg_id"];
      data_out["action"] = "returned_api_response";
      String query = data_in["query"];
      query.toLowerCase();
      data_out["returned_api_response"] = HandleResponse(query);
      String outdata;
      serializeJson(data_out, outdata);
      publishMSG(outdata.c_str());
    }
  };

  void sendMsg(String number, String msg)
  {
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
    //  this->client = client;
    this->topic = this->DeviceName + "/" + this->Username;
    this->outname = this->DeviceName + "@" + this->Username;
    this->char_DeviceName = this->DeviceName.c_str();
    this->char_Password = this->Password.c_str();
    this->char_outname = this->outname.c_str();
    this->char_topic = this->topic.c_str();
    this->initiated = true;
    this->setupIT();
  }

  void Handle()
  {
    if (!client.connected())
    {
      while (!client.connected())
      {
        Serial.print("Attempting connection...");
        if (client.connect(this->char_outname, this->char_outname, this->char_Password))
        {
          Serial.println("connected");
          client.subscribe(this->char_topic);
        }
        else
        {
          Serial.print("failed, rc=");
          Serial.print(this->client.state());
          Serial.println(" try again in 5 seconds");
          delay(5000);
        }
      }
    }
    this->client.loop();
  }

  void publishMSG(const char *info)
  {
    client.publish(this->char_topic, info);
  }

  void SetHost(const char *host)
  {
    this->mqttServer = host;
  }

private:
  String Username;
  String DeviceName;
  String Password;

  bool initiated = false;

  const char *ssid;
  const char *ssid_password;

  const char *mqttServer = "thingesp.siddhesh.me";
  #ifndef _DISABLE_TLS_
  int mqttPort = 1899;
  #else 
  int mqttPort = 1893;
  #endif


  /*
  #ifndef _THINGESP_SERVER_
  const char *mqttServer = "thingesp.siddhesh.me";
  #else
  const char *mqttServer = _THINGESP_SERVER_;
  #endif


  #ifndef _THINGESP_PORT_
  #define _THINGESP_PORT_  1893
  #endif

  #ifndef _THINGESP_PORT_TLS_
  #define _THINGESP_PORT_TLS_  1899
  #endif

  #ifndef _DISABLE_TLS_
  int mqttPort = _THINGESP_PORT_TLS_;
  #endif
  #ifdef _DISABLE_TLS_
  int mqttPort = _THINGESP_PORT_;
  #endif
*/

  String topic;
  String outname;
  const char *char_DeviceName;
  const char *char_Password;
  const char *char_outname;
  const char *char_topic;

  PubSubClient client;
  WiFiClientSecure espClient;

  void callback(char *topic, byte *payload, unsigned int length)
  {
    String srr;
    Serial.println();
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();
    for (int i = 0; i < length; i++)
    {
      srr.concat((char)payload[i]);
    }
    Serial.print(srr);
    this->logic(srr);
  }

  void setupIT()
  {

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Serial.println(mqttPort);

    WiFi.begin(ssid, ssid_password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    this->client.setServer(this->mqttServer, this->mqttPort);
    this->client.setCallback([this](char *topic, byte *payload, unsigned int length) {
      callback(topic, payload, length);
    });
  }
};
