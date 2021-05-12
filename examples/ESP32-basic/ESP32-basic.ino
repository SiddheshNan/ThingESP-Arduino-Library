#include <WiFi.h>
#include <ThingESP.h>

ThingESP32 thing("username", "project_name", "credentials");

int LED = 2;

void setup()
{
  Serial.begin(115200);

  pinMode(LED, OUTPUT);

  thing.SetWiFi("wifi_ssid", "wifi_password");

  thing.initDevice();

}



String HandleResponse(String query)
{

  if (query == "led on") {
    digitalWrite(LED, 1);
    return "Done: LED Turned ON";
  }

  else if (query == "led off") {
    digitalWrite(LED, 0);
    return "Done: LED Turned OFF";
  }

  else if (query == "led status")
    return digitalRead(LED) ? "LED is ON" : "LED is OFF";


  else return "Your query was invalid..";

}




void loop()
{
  thing.Handle();
}