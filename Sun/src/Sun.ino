////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  ino
//  2019
//  *********
//  29/01/2019  Forked from old system and cleaned up
//  05/02/2019  Added indicator light

// Add an interupt for the button
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
#include <ArduinoJson.h>   // Json Library
#include <ArduinoOTA.h>    // OTA
#include <ESP8266WiFi.h>   // WiFi
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial Printouts
#include <WiFiClient.h>    // May not be needed
#include <string.h>        // Used for node disconnect message

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
#define NTP_ADDRESS "uk.pool.ntp.org"  // Uk time server
#define NTP_OFFSET 3600                // In seconds (3600 for uk)
#define NTP_INTERVAL 60 * 1000         // In miliseconds

#define relayPin 12  // 12 This pin also has a blue LED
#define connectionLED 13
#define buttonPin 0

#define ON LOW  // Check this later
#define OFF HIGH

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
const char *wifiSsid = "I Don't Mind";
const char *wifiPassword = "Have2Biscuits";

const char *nodeName = "Sun";
const char *nodePassword = "crm0xhvsmn";

const char *disconnectMsg = "Sun Disconnected";
const char *controlTopic = "Sun Control";
const char *mqttServerIP = "192.168.1.46";

// const char* mqttServerIP = "broker.mqttdashboard.com";  // Use this one for testing without local broker

long interval = (5 * 1000);  // Update every 5 seconds
unsigned long previousMillis = 0;

long overRunTimerStart;

bool heatingState = false;
bool oldHeatingState = false;

bool relayState;
bool buttonState;
bool lastButtonState;

bool WiFiConnected = false;
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
  pinMode(buttonPin, INPUT);
  pinMode(connectionLED, OUTPUT);
  pinMode(relayPin, OUTPUT);

  Serial.begin(115200);
  Serial << "\n| " << nodeName << " |" << endl;

  startWifi();
  startMQTT();
  startOTA();
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
  handleMQTT();
  handleWiFi();
  ArduinoOTA.handle();

  // Serial << digitalRead(buttonPin) << endl;

  buttonState = digitalRead(buttonPin);

  if (buttonState != lastButtonState) {
    if (buttonState == false) {  // Active low
      Serial << "Button Pressed" << endl;
      mqtt.publish("Sun Button", "1");
    }
  }
  lastButtonState = buttonState;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {  // 5 second update cycle
    previousMillis = currentMillis;

    if (WiFiConnected) {
      publishAll();
    }
  }
}