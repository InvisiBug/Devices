////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Temp Buttons.ino
//  2019
//  *********
//  29/01/2019  Forked from old system and cleaned up
//  05/02/2019  Added indicator light
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
#include <DHTesp.h>
#include <ESP8266WiFi.h>  // WiFi
#include <OneButton.h>
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial Printouts
#include <WiFiClient.h>    // May not be needed

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
#define connectionLED LED_BUILTIN

// #define lampPin D7
// #define deskLEDsPin D1
// #define speakersPin D2
// #define floodlightPin D3
// #define screenLEDsPin D4
// #define ambientPin D5
// #define sunPin D6

#define lampPin D6
#define deskLEDsPin D1
#define speakersPin D2
#define floodlightPin D5
#define screenLEDsPin D7
#define ambientPin D4
#define sunPin D3

#define ON LOW  // Confirmed for Wemos D1 Mini (On - High for esp 32)
#define OFF HIGH

#define mqttLen 50  // Buffer for non JSON MQTT comms

#define mqttPrint true

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
// MQTT Client
WiFiClient espClient;
PubSubClient mqtt(espClient);

OneButton lamp(lampPin, true);
OneButton deskLEDs(deskLEDsPin, true);
OneButton speakers(speakersPin, true);
OneButton floodlight(floodlightPin, true);
OneButton screenLEDs(screenLEDsPin, true);
OneButton ambient(ambientPin, true);
OneButton sun(sunPin, true);

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
const char* wifiSsid = "I Don't Mind";
const char* wifiPassword = "Have2Biscuits";

const char* nodePassword = "crm0xhvsmn";
const char* nodeName = "Buttons 2";

const char* disconnectMsg = "Buttons Disconnected";

const char* mqttServerIP = "192.168.1.46";

bool WiFiConnected = false;

char msg[mqttLen];  // Buffer to store the MQTT messages

// System Status Variables
bool deskOff = false;
bool lampOff = false;
bool audioOff = false;
bool plugOff = false;
bool computerOff = false;
bool screenLEDsOff = false;
bool sunOff = false;

int deskLEDsRed;
int deskLEDsGreen;
int deskLEDsBlue;

int tableLampRed;
int tableLampGreen;
int tableLampBlue;

int screenLEDsRed;
int screenLEDsGreen;
int screenLEDsBlue;

bool ambientMode = false;

long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

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
  Serial << "\n|** " << nodeName << " **|" << endl;

  startWifi();
  startMQTT();
  startOTA();

  lamp.attachClick(lampClicked);
  lamp.setDebounceTicks(50);

  deskLEDs.attachClick(deskLEDsClicked);
  deskLEDs.setDebounceTicks(50);

  speakers.attachClick(speakersClicked);
  speakers.setDebounceTicks(50);
  speakers.attachLongPressStart(speakersHeld);
  speakers.setPressTicks(250);

  floodlight.attachClick(floodlightClicked);
  floodlight.setDebounceTicks(50);

  screenLEDs.attachClick(screenLEDsClicked);
  screenLEDs.setDebounceTicks(50);

  ambient.attachClick(ambientClicked);
  ambient.setDebounceTicks(50);

  sun.attachClick(sunClicked);
  sun.setDebounceTicks(50);
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
void loop() {
  handleWiFi();
  handleMQTT();
  ArduinoOTA.handle();

  lamp.tick();
  deskLEDs.tick();
  speakers.tick();
  floodlight.tick();
  screenLEDs.tick();
  ambient.tick();
  sun.tick();
}
