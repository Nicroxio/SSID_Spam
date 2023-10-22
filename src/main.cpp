#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include "webpage.h"
#include "config.h"
#include "secret.h"

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1);

DNSServer dnsServer;
ESP8266WebServer webServer(80);

const char *ssid_array[16] = {ssid0, ssid1, ssid2, ssid3, ssid4, ssid5, ssid6, ssid7, ssid8, ssid9, ssid10, ssid11, ssid12};
unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, array_pos = 0, previous_arraypos = 1;
unsigned long time_now = 0;

void handleRoot()
{
  webServer.send_P(HTTP_CODE, "text/html", page_content);
}

void getIP()
{
  IPAddress ip = WiFi.localIP();
  webServer.send(200, "text/plain", ip.toString().c_str());
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.begin(OTA_SSID, OTA_PASS);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
  dnsServer.start(DNS_PORT, "*", APIP);

  webServer.onNotFound([]()
                       {
    lastActivity = millis();
    webServer.send_P(HTTP_CODE, "text/html", page_content); });
  webServer.on("/", handleRoot);
  webServer.on("/IP", getIP);
  webServer.begin();
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
}

void loop()
{
  time_now = millis();
  Serial.println(ssid_array[array_pos]);
  WiFi.softAP(ssid_array[array_pos]);

  if (array_pos == SSID_NUMBER)
  {
    array_pos = 0;
  }
  else
  {
    array_pos++;
  }

  while (millis() < time_now + period)
  {
    ArduinoOTA.handle();
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  Serial.println("Changing SSID");
}