////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Table Lamp.ino
//  2019
//  *********
//
//  Versions
//  V1 - Project Start - 06/05/2019

/*
 LEDs
 Total -------- 136
 Top ---------- 49
 Left --------- 20
 Right -------- 20
 Bottom Left -- 23
 Bottom Right - 24

 Prismatik Settings
 Top ---------- 49
 Sides -------- 20
 Bottom ------- 57
 Thickness ---- 5%
 Stand Width -- 0%
 Start Offset - 1

*/

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # # #    #  ####  #    # #####  ####
//  #     # # ##   # #    # #    #   #   #
//  ######  # # #  # #    # #    #   #    ####
//  #       # #  # # #    # #    #   #        #
//  #       # #   ## #    # #    #   #   #    #
//  #       # #    #  ####   ####    #    ####
//
////////////////////////////////////////////////////////////////////////
// Board
// Use Wemos D1 R2 & Mini

////////////////////////////////////////////////////////////////////////
//
//  ###
//   #  #    #  ####  #      #    # #####  ######  ####
//   #  ##   # #    # #      #    # #    # #      #
//   #  # #  # #      #      #    # #    # #####   ####
//   #  #  # # #      #      #    # #    # #           #
//   #  #   ## #    # #      #    # #    # #      #    #
//  ### #    #  ####  ######  ####  #####  ######  ####
//
////////////////////////////////////////////////////////////////////////
#include <ArduinoJson.h>  // Json Library
#include <ArduinoOTA.h>   // OTA
#include <ESP8266WiFi.h>  // Wifi
#include <FastLED.h>
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial printouts
#include <string.h>

// Effects
// #include "Fire.h"
#include "ColourCycle.h"
#include "Crisscross.h"
#include "Rainbow.h"
// #include "ChangeBrightness.h"

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     # ###### ###### # #    # # ##### #  ####  #    #  ####
//  #     # #      #      # ##   # #   #   # #    # ##   # #
//  #     # #####  #####  # # #  # #   #   # #    # # #  #  ####
//  #     # #      #      # #  # # #   #   # #    # #  # #      #
//  #     # #      #      # #   ## #   #   # #    # #   ## #    #
//  ######  ###### #      # #    # #   #   #  ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
// Physical I/O
#define totalLEDs 136
#define connectionLED D4
#define LEDdata D5
#define ambientPin D6

// MQTT
#define mqttLen 50

#define ON LOW
#define OFF HIGH

#define colour color  // Means we can spell it properly

#define serialRate 115200  // Standard

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #####  #    #   ##   #####  ######
//  #     #  #  #  #    # #    # #    #  #  #  #    # #
//  ####### #    # #    # #    # #    # #    # #    # #####
//  #     # ###### #####  #    # # ## # ###### #####  #
//  #     # #    # #   #  #    # ##  ## #    # #   #  #
//  #     # #    # #    # #####  #    # #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
// MQTT CLient
WiFiClient espClient;
PubSubClient mqtt(espClient);

// LED ring
CRGB currentLED[totalLEDs];

// Effects
Rainbow rainbow(totalLEDs, currentLED, 20);
Crisscross crissCross(totalLEDs, currentLED, 20);
ColourCycle colourCycle(totalLEDs, currentLED, 20);

////////////////////////////////////////////////////////////////////////
//
//  #     #
//  #     #   ##   #####  #   ##   #####  #      ######  ####
//  #     #  #  #  #    # #  #  #  #    # #      #      #
//  #     # #    # #    # # #    # #####  #      #####   ####
//   #   #  ###### #####  # ###### #    # #      #           #
//    # #   #    # #   #  # #    # #    # #      #      #    #
//     #    #    # #    # # #    # #####  ###### ######  ####
//
////////////////////////////////////////////////////////////////////////
char* wifiSsid = "I Don't Mind";
char* wifiPassword = "Have2Biscuits";

char* nodeName = "Screen LEDs";
char* nodePassword = "crm0xhvsmn";

const char* controlTopic = "Screen LEDs Control";

char* disconnectMsg = "Screen LEDs Disconnected";

char* mqttServerIP = "mqtt.kavanet.io";

char msg[mqttLen];  // Buffer to store the MQTT messages

int mode = 0;

int LEDBrightness = 10;  // As a percentage (saved as a dynamic variable to let us change later)

long interval = (5 * 1000);  // Update every 5 seconds
unsigned long previousMillis = 0;
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;
bool WiFiConnected = false;

int red, green, blue;

////////////////////////////////////////////////////////////////////////
//
//  ######                                                #####
//  #     # #####   ####   ####  #####    ##   #    #    #     # #####   ##   #####  ##### #    # #####
//  #     # #    # #    # #    # #    #  #  #  ##  ##    #         #    #  #  #    #   #   #    # #    #
//  ######  #    # #    # #      #    # #    # # ## #     #####    #   #    # #    #   #   #    # #    #
//  #       #####  #    # #  ### #####  ###### #    #          #   #   ###### #####    #   #    # #####
//  #       #   #  #    # #    # #   #  #    # #    #    #     #   #   #    # #   #    #   #    # #
//  #       #    #  ####   ####  #    # #    # #    #     #####    #   #    # #    #   #    ####  #
//
////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(serialRate);

  Serial << "\n| " << nodeName << " |" << endl;

  pinMode(connectionLED, OUTPUT);
  digitalWrite(connectionLED, ON);  // Should be on (turned off for testing)

  pinMode(ambientPin, OUTPUT);
  digitalWrite(ambientPin, true);

  FastLED.addLeds<WS2812, LEDdata, GRB>(currentLED, totalLEDs);
  FastLED.setBrightness(LEDBrightness * 2.55);

  startWifi();
  startMQTT();
}

///////////////////////////////////////////////////////////////////////
//
//  #     #                    ######
//  ##   ##   ##   # #    #    #     # #####   ####   ####  #####    ##   #    #
//  # # # #  #  #  # ##   #    #     # #    # #    # #    # #    #  #  #  ##  ##
//  #  #  # #    # # # #  #    ######  #    # #    # #      #    # #    # # ## #
//  #     # ###### # #  # #    #       #####  #    # #  ### #####  ###### #    #
//  #     # #    # # #   ##    #       #   #  #    # #    # #   #  #    # #    #
//  #     # #    # # #    #    #       #    #  ####   ####  #    # #    # #    #
//
///////////////////////////////////////////////////////////////////////
void loop(void) {
  // rainbow.run();
  // colourFade.run();

  // if (mode == 2) rainbow.run();

  handleWiFi();
  handleMQTT();

  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;

  //   // publishAll();
  // }
}