#include <ESP8266WiFi.h>
#include <whesp8266.h>

WH_ESP8266 DeviceClient;

int LED = LED_BUILTIN;

void setup()
{
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);

  DeviceClient.SetDevice("username", "project_name", "credentials");

  DeviceClient.SetWiFi("ssid", "pass");

  DeviceClient.initDevice();

}



String HandleResponse(String query)
{

  if (query == "led on") {
    digitalWrite(LED, 0);
    return "Done: LED Turned ON";
  }

  else if (query == "led off") {
    digitalWrite(LED, 1);
    return "Done: LED Turned OFF";
  }

  else if (query == "led status")
    return digitalRead(LED) ? "LED is OFF" : "LED is ON";


  else return "Your query was invalid..";

}




void loop()
{

  DeviceClient.Handle();

}