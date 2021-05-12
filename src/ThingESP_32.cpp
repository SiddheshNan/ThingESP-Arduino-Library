#define MQTT_MAX_PACKET_SIZE 1024
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

String HandleResponse(String query);

class ThingESP32
{
public:
  ThingESP32(String username, String deviceName, String password) : g_client(espClient)
  {
    this->Username = username;
    this->DeviceName = deviceName;
    this->Password = password;
  };

  void SetWiFi(const char *ssID, const char *ssID_password)
  {
    this->ssid = ssID;
    this->ssid_password = ssID_password;
  }

  void initDevice()
  {
    this->topic = this->DeviceName + "/" + this->Username;
    this->outname = this->DeviceName + "@" + this->Username;

    this->char_DeviceName = this->DeviceName.c_str();
    this->char_Password = this->Password.c_str();
    this->char_outname = this->outname.c_str();
    this->char_topic = this->topic.c_str();
    this->initiated = true;

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    Serial.println(mqttServer);

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

    g_client.setServer(this->mqttServer, this->mqttPort);
    g_client.setCallback([this](char *topic, byte *payload, unsigned int length) {
      callback(topic, payload, length);
    });
  }

  void Handle()
  {
    if (!g_client.connected())
    {
      while (!g_client.connected())
      {
        delay(10);
        Serial.print("Attempting connection...");
        if (g_client.connect(this->char_outname, this->char_outname, this->char_Password))
        {
          Serial.println("connected");
          g_client.subscribe(this->char_topic);
        }
        else
        {
          Serial.print("failed, rc=");
          Serial.print(g_client.state());
          Serial.println(" try again in 5 seconds");
          delay(5000);
        }
      }
    }
    g_client.loop();
  }

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

private:
  String Username;
  String DeviceName;
  String Password;

  bool initiated = false;

  const char *ssid;
  const char *ssid_password;

  const char *mqttServer = "thingesp.siddhesh.me";

  int mqttPort = 1893;

  String topic;
  String outname;

  const char *char_DeviceName;
  const char *char_Password;
  const char *char_outname;
  const char *char_topic;

  WiFiClient espClient;
  PubSubClient g_client;

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
    onMessage(srr);
  }

  void onMessage(String data)
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
  }

  void publishMSG(const char *info)
  {
    g_client.publish(this->char_topic, info);
  }
};
