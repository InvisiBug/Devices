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
#include <ESP8266WiFi.h>  // Wifi
#include <FastLED.h>
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial printouts
#include <string.h>

// Effects
// #include "Fire.h"
#include "ColourCycle.h"
#include "ColourFade.h"
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
#define totalLEDs 24
#define wifiLED D4
#define LEDdata D2

// MQTT
#define mqttLen 50
#define qos 0

#define ON LOW
#define OFF HIGH

#define colour color  // Means we can spell it properly

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
ColourFade colourFade(totalLEDs, currentLED, 10);
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

char* nodeName = "Table Lamp";
char* nodePassword = "crm0xhvsmn";

char* disconnectMsg = "Table Lamp Disconnected";

char* mqttServerIP = "mqtt.kavanet.io";

char msg[mqttLen];  // Buffer to store the MQTT messages

int LEDBrightness = 255;  // As a percentage (saved as a dynamic variable to let us change later)

long interval = (5 * 1000);  // Update every 5 seconds
unsigned long previousMillis = 0;

int mode = 0;  // Thing to hold the current display mode

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
  Serial.begin(115200);
  Serial << "\n| " << nodeName << " |" << endl;

  pinMode(wifiLED, OUTPUT);
  digitalWrite(wifiLED, ON);  // Should be on (turned off for testing)

  FastLED.addLeds<NEOPIXEL, LEDdata>(currentLED, totalLEDs);

  FastLED.setBrightness(LEDBrightness * 2.55);
  FastLED.setCorrection(0xFFB0F0);
  FastLED.setDither(0);

  startWifi();
  startMQTT();
  // startTime();

  // Initialize Effects
  colourCycle.begin();
  colourFade.begin();
  crissCross.begin();
  rainbow.begin();
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
  handleMQTT();

  if (mode == 0) {
    for (int i = 0; i < totalLEDs; i++) {
      currentLED[i].setRGB(red, green, blue);
    }
    FastLED.show();
  }

  else if (mode == 1) {
    rainbow.run();
  }

  else if (mode == 2) {
    colourCycle.run();
  }

  else if (mode == 3) {
    colourFade.run();
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    publishAll();
  }
}