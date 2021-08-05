////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Heating Controller
//  28/12/2019
//
////////////////////////////////////////////////////////////////////////
//
//  ###
//   #  #    #  ####  #      #    # #####  ######  ####
//   #  ##   # #    # #      #    # #    # #      #
//   #  # #  # #      #      #    # #    # #####   ####
//   #  #  # # #      #      #    # #    # #           #
//   #  #   ## #    # #      #    # #    # #      #    #
//  ### #    #  ####  ######  ####  #####  ######  ####
// boop
////////////////////////////////////////////////////////////////////////
#include <ArduinoJson.h>  // Json Library
#include <ArduinoOTA.h>   // OTA
#include <OneButton.h>
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial printouts
#include <WiFiClient.h>    // Wifi

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
#define connectionLED 13
#define relayPin 12
#define buttonPin 0

// Test Board I/O
// #define connectionLED D7
// #define relayPin D5
// #define buttonPin D3

// MQTT
#define qos 0

// I/O Logic
#define ON LOW
#define OFF HIGH

#define mqttLen 50  // Buffer for non JSON MQTT comms

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

// Button
OneButton button(buttonPin, true);

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

const char* nodeName = "Liams Desk Light";
const char* nodePassword = "Kavanet";

const char* disconnectMsg = "Liams Desk Light Disconnected";
const char* controlTopic = "Liams Desk Light Control";
const char* buttonTopic = "Liams Desk Light Button";
const char* mqttServerIP = "192.168.1.46";

bool WiFiConnected = false;

char msg[mqttLen];  // Buffer to store the MQTT messages

// Connection Timers
long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

// Status Update
unsigned long updateCurrentMillis;
unsigned long updatePreviousMillis;

// unsigned long timerCurrentMillis  = 0;
unsigned long timerPreviousMillis = 0;

bool relayState = false;
bool buttonState = false;
bool lastButtonState = false;

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

  pinMode(connectionLED, OUTPUT);

  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(relayPin, false);

  startWifi();
  startMQTT();
  startOTA();

  button.attachClick(click);
  button.setDebounceTicks(50);
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
  button.tick();

  updateCurrentMillis = millis();
  if (updateCurrentMillis - updatePreviousMillis >= 5 * 1000) {
    updatePreviousMillis = updateCurrentMillis;
    publishAll();
  }
}

void click() {
  mqtt.publish(buttonTopic, "1");
}
