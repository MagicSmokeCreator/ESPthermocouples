#include <Wire.h>
#include <heltec.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
const char* ssid = "ESP8266";
const char* password = "ESP8266password";
ESP8266WebServer server(80);

#include <Thermocouple.h>
#include <MAX6675_Thermocouple.h>
#define SCK_PIN 13
#define CS_PIN 12
#define SO_PIN 14
Thermocouple* thermocouple;
float celsius;
float topTemperature;
const char * readingInt;
int upperlimit = 400;
int lowerlimit = 0;




// VOIDS

void handleSentVar() {
  if (server.hasArg("sensor_reading")) { // this is the variable sent from the client
       
    String readingString = server.arg("sensor_reading"); // converting string to char
    char readingInt[50];
    readingString.toCharArray(readingInt, 50);
    
    // int topTemperature = atoi(readingInt); // converting char to int

    topTemperature = readingString.toFloat(); // converting string to float

    server.send(200, "text/html", "Data received"); // this lets the other ESP know message is received
    // Serial.print("handleSentVar reached");
    // Serial.print(" - readingInt = ");
    // Serial.println(readingInt);
    delay(100);
  }

}

void displayReadingsOnOled() {
  String temperatureDisplay = "Temperature: " + (String)celsius + "°C";
  String Toptemp = "Top temperature: " + (String)topTemperature + "°C";

  Heltec.display->clear();
  Heltec.display->drawString(0,0, temperatureDisplay);
  Heltec.display->drawString(0,20, Toptemp);
  Heltec.display->display();
  }

void setup() {

  delay(1000);

  //  Display
  Heltec.begin(true, true);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->drawString(0,0,"Startup...");
  Heltec.display->display();
  
  // WiFi
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  
  server.on("/data/", HTTP_GET, handleSentVar);
  server.begin();
  
  delay(100);

  // Temperature measurement
  thermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);

  // Serial
  Serial.begin(115200);
  delay(200);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());    
  
  }

void loop() {

  server.handleClient(); 
  
  celsius = thermocouple->readCelsius();

  // Serial.println(upperlimit);
  // Serial.println(lowerlimit);
  Serial.print("Temperature = ");
  Serial.println(celsius);
  Serial.print("Top temperature = ");
  Serial.println(topTemperature);
  
  displayReadingsOnOled();

  delay(1000);
}






