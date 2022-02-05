
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
#define relay1Pin 12
#define relay2Pin 5
#define buttonPin 0
// // I/O Logic
#define ON LOW
#define OFF HIGH

// Test Board I/O
// #define connectionLED D7
// #define relayPin D5
// #define buttonPin D3
// I/O Logic
// #define ON HIGH
// #define OFF LOW

// MQTT
#define qos 0
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

const char* nodeName = "Heating";
const char* nodePassword = "crm0xhvsmn";

const char* disconnectMsg = "Heating Disconnected";
const char* controlTopic = "Heating Control";
const char* mqttServerIP = "mqtt.kavanet.io";

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
  Serial.begin(19200);
  // Serial << "\n|** " << nodeName << " **|" << endl;

  pinMode(connectionLED, OUTPUT);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(relay1Pin, false);
  digitalWrite(relay2Pin, false);

  startWifi();
  startMQTT();
  // startOTA();

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
  button.tick();

  updateCurrentMillis = millis();
  if (updateCurrentMillis - updatePreviousMillis >= 5 * 1000) {
    updatePreviousMillis = updateCurrentMillis;

    publishAll();
  }
}

void click() {
  changeRelayState();
}
