#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "webpage.h"
#include <pgmspace.h>

const char ssid0[32] = "I really hate Rishi Sunak";
const char ssid1[32] = "I really hate Liz Truss";
const char ssid2[32] = "I really hate the T-Level";
const char ssid3[32] = "TRANS RIGHTS!!!!";
const char ssid4[32] = "LCC WiFi really is shit";
const char ssid5[32] = "Digit's probs down atm";
const char ssid6[32] = "WasItWorthSellingOutToCoptrz?";
const char ssid7[32] = "Who am I?";
const char ssid8[32] = "The Gov is corrupt";
const char ssid9[32] = "TheToriesDon'tcareaboutyou";
const char ssid10[32] = "The Chips Here Are Tasty";
const char ssid11[32] = "TRANSRIGHTS!!!!";
const char ssid12[32] = "TheGovOnlySupportsItsAgenda";

const char *ssid_array[16] = {ssid0, ssid1, ssid2, ssid3, ssid4, ssid5, ssid6, ssid7, ssid8, ssid9, ssid10, ssid11, ssid12};
const int SSID_NUMBER = 13; // Change to number of total SSID's

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1);

DNSServer dnsServer;
ESP8266WebServer webServer(80);

unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, array_pos = 0, previous_arraypos = 1;
unsigned long time_now = 0;
int period = 120000;

void LEDFLUTTER()
{
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));

  dnsServer.start(DNS_PORT, "*", APIP);
  webServer.onNotFound([]()
                       { lastActivity=millis(); webServer.send_P(HTTP_CODE, "text/html", page_content); });
  webServer.begin();
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
    dnsServer.processNextRequest();
    webServer.handleClient();
  }
  LEDFLUTTER();
  Serial.println("Changing SSID");
}