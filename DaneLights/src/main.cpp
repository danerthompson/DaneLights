#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "credentials.h" //Where userSSID, userPassword, and userPort are stored
#include "index.h"

// Replace with your network credentials
const char* ssid = userSSID;
const char* password = userPassword;

//debugging stuff
int count = 0;
unsigned long timer;
int r, g, b;

Preferences preferences;

// Create AsyncWebServer object on port user defined port
AsyncWebServer server(userPort);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  
  ledcSetup(0, 300, 8);
  ledcSetup(1, 300, 8);
  ledcSetup(2, 300, 8);

  ledcAttachPin(14, 0);
  ledcAttachPin(25, 1);
  ledcAttachPin(32, 2);

  timer = millis();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/initialize", HTTP_GET, [](AsyncWebServerRequest *request) {

    //Load current hex code
    preferences.begin("lights", false);
    Serial.println("initialize called");
    String prevRGB = preferences.getString("prevRGB", "000000"); //RGB in hex
    preferences.putString("prevRGB", prevRGB);
    preferences.end();

    request->send(200, "text/plain", prevRGB);
  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    String rgb = "No message";
    char rgbConverted[6];
    const char* param = "hex";

    if (request->hasParam(param)) {
      rgb = request->getParam(param)->value();
    }

    for (int i = 0; i < 6; i++) {
      rgbConverted[i] = rgb[i];
    }

    //Store current hex code
    preferences.begin("lights", false);
    preferences.putString("prevRGB", rgb);
    preferences.end();

    sscanf(rgbConverted, "%02x%02x%02x", &r, &g, &b);

    Serial.println(rgb);
    Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.println(b);

    //Write to LEDs
    ledcWrite(0, r);
    ledcWrite(1, g);
    ledcWrite(2, b);

    request->send(200, "text/plain", "OK");
  });
  server.begin();
}

void loop() {
}