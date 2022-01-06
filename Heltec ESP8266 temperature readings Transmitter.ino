#include <Wire.h>
#include <heltec.h>
#include <stdlib.h>

#include <ESP8266WiFi.h>
const char* ssid = "ESP8266";
const char* password = "ESP8266password";
const char* WirelessSSID;

#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#define SCK_PIN 13
#define CS_PIN 12
#define SO_PIN 14
Thermocouple* topthermocouple;
int celsius;




void displayReadingsOnOled() {
  String temperatureDisplay = "Top temperature: " + (String)celsius + "°C";
  String WirelessSSID = "Connected to: " + (String)ssid;

  Heltec.display->clear();
  Heltec.display->drawString(0,0, temperatureDisplay);
  Heltec.display->drawString(0,20, WirelessSSID);
  Heltec.display->display();
  }

void setup() {

  // Serial
  Serial.begin(115200);
  delay(200);

  //  Display
  Heltec.begin(true, true);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->drawString(0,0,"Startup...");
  Heltec.display->display();
  delay(1000);
  
  // WiFi
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"Connecting to WiFi");
  Heltec.display->display();
  int i =0;
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println(++i); Serial.print (' ');
      }
  
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"Connected to WiFi");
  Heltec.display->drawString(0,20,ssid);
  Heltec.display->display();
  
  Serial.print("WiFi connecion established @ ");
  Serial.println(WiFi.localIP());
  WirelessSSID = ssid;
  delay(500);

  // Temperature measurement
  topthermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);
  
  }

void loop() {

  // float celsius = topthermocouple->readCelsius();
  celsius = 21;
  // String intToPrint = "test";
  // const char * intToPrint = celsius;

  char buffer[8];
  itoa(celsius, buffer, 10);
  const char * intToPrint(buffer);
  
  
  // char* intToPrint[5];
  // intToPrint = celsius;
  // itoa(celsius, intToPrint, 10);

  WiFiClient client;
  const char * host = "192.168.4.1";
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) { // WiFi connection failed
    Serial.println("connection failed");
    Heltec.display->clear();
    Heltec.display->drawString(0,0, "Host connection failed");
    Heltec.display->display();
    delay(100);
    return;
  }

  String url = "/data/";
  url += "?sensor_reading=";
  url += intToPrint; // this needs to be a const char
  // url += "test!";
  

  client.print(String("GET ") + url + " HTTP/1.1\r\n" + // Sending to server
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
               
                Serial.print("Sending: ");
                Serial.println(celsius);

  unsigned long timeout = millis();
  while (client.available() == 0) { // Timeout
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      Heltec.display->clear();
      Heltec.display->drawString(0,0, ">>> Client Timeout !");
      Heltec.display->display();
      client.stop();
      return;
    }
  }
  
  displayReadingsOnOled();
  
  delay(100);
}




