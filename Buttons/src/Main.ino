////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  2019
//  *********
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

#define button1Pin D1
#define button2Pin D2
#define button3Pin D3
#define button4Pin D4
#define button5Pin D5
#define button6Pin D6
#define button7Pin D7

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

OneButton button1(button1Pin, true);
OneButton button2(button2Pin, true);
OneButton button3(button3Pin, true);
OneButton button4(button4Pin, true);
OneButton button5(button5Pin, true);
OneButton button6(button6Pin, true);
OneButton button7(button7Pin, true);

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

const char* mqttServerIP = "mqtt.kavanet.io";

bool WiFiConnected = false;

char msg[mqttLen];  // Buffer to store the MQTT messages

// System Status Variables
bool deskOff = false;
bool lampOff = false;
bool tableLampOff = false;
bool audioOff = false;
bool floodlightOff = false;
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

  button1.attachClick(button1Clicked);
  button1.setDebounceTicks(50);
  button1.attachLongPressStart(button1Held);
  button1.setPressTicks(250);

  button2.attachClick(button2Clicked);
  button2.setDebounceTicks(50);
  button2.attachLongPressStart(button2Held);
  button2.setPressTicks(250);

  button3.attachClick(button3Clicked);
  button3.setDebounceTicks(50);
  button3.attachLongPressStart(button3Held);
  button3.setPressTicks(250);

  button4.attachClick(button4Clicked);
  button4.setDebounceTicks(50);
  button3.attachLongPressStart(button4Held);
  button4.setPressTicks(250);

  button5.attachClick(button5Clicked);
  button5.setDebounceTicks(50);
  button5.attachLongPressStart(button5Held);
  button5.setPressTicks(250);

  button6.attachClick(button6Clicked);
  button6.setDebounceTicks(50);
  button6.attachLongPressStart(button6Held);
  button6.setPressTicks(250);

  button7.attachClick(button7Clicked);
  button7.setDebounceTicks(50);
  button7.attachLongPressStart(button7Held);
  button7.setPressTicks(250);
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

  button1.tick();
  button2.tick();
  button3.tick();
  button4.tick();
  button5.tick();
  button6.tick();
  button7.tick();
}
