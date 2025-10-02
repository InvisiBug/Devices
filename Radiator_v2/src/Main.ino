
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
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
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
#define relay1Pin 5
#define relay2Pin 12
#define buttonPin 10  // Active low

#define fanPin relay1Pin
#define valvePin relay2Pin
// // I/O Logic
#define ON LOW
#define OFF HIGH

#define oneWireBus 2

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

// Temperature Sensor
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

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

const char* nodeName = "Study Radiator";
const char* nodePassword = "crm0xhvsmn";

const char* disconnectMsg = "Study Radiator Disconnected";
const char* controlTopic = "Study Radiator Control";
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
  Serial << "\n|** " << nodeName << " **|" << endl;

  sensors.begin();

  pinMode(connectionLED, OUTPUT);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(relay1Pin, false);
  digitalWrite(relay2Pin, false);

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
void loop() {
  handleWiFi();
  handleMQTT();

  updateCurrentMillis = millis();

  if (updateCurrentMillis - updatePreviousMillis >= 5 * 1000) {
    updatePreviousMillis = updateCurrentMillis;
    publishAll();

    Serial << "Valve"
           << "\t" << digitalRead(valvePin) << endl;
    Serial << "Fan"
           << "\t" << digitalRead(fanPin) << endl;
    Serial << "Temp"
           << "\t" << sensors.getTempCByIndex(0) << endl;
  }
  test
}

void click() {
  changeRelayState();
}
