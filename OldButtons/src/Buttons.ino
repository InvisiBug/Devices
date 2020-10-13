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
#include <ESP8266WiFi.h>   // WiFi
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

#define buttonOne D1
#define buttonTwo D2
#define buttonThree D5
#define buttonFour D6
#define buttonFive D7
#define buttonSix D8

#define sensorPin D3

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

// Temperature & Humidity Sensor
DHTesp dht;

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
const char* nodeName = "Buttons";

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

// Button Debounce States
bool buttonOneState;
bool lastButtonOneState;

bool buttonTwoState;
bool lastButtonTwoState;

bool buttonThreeState;
bool lastButtonThreeState;

bool buttonFourState;
bool lastButtonFourState;

bool buttonFiveState;
bool lastButtonFiveState;

bool buttonSixState;
bool lastButtonSixState;

unsigned long interval = (5 * 1000);  // Update every 5 seconds
unsigned long previousMillis = 0;

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
  Serial << "\n| " << nodeName << " |" << endl;

  pinMode(connectionLED, OUTPUT);

  dht.setup(sensorPin, DHTesp::DHT22);

  pinMode(buttonOne, INPUT);
  pinMode(buttonTwo, INPUT);
  pinMode(buttonThree, INPUT);
  pinMode(buttonFour, INPUT);
  pinMode(buttonFive, INPUT);
  pinMode(buttonSix, INPUT);

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
void loop(void) {
  handleWiFi();
  handleMQTT();
  ArduinoOTA.handle();

  buttonOneState = digitalRead(buttonOne);
  buttonTwoState = digitalRead(buttonTwo);
  buttonThreeState = digitalRead(buttonThree);
  buttonFourState = digitalRead(buttonFour);
  buttonFiveState = digitalRead(buttonFive);
  buttonSixState = digitalRead(buttonSix);

  // Button One
  if (buttonOneState != lastButtonOneState) {
    if (buttonOneState) {
      buttonOnePressed();
    }
  }
  lastButtonOneState = buttonOneState;

  // Button Two
  if (buttonTwoState != lastButtonTwoState) {
    if (buttonTwoState) {
      buttonTwoPressed();
    }
  }
  lastButtonTwoState = buttonTwoState;

  // Button Three
  if (buttonThreeState != lastButtonThreeState) {
    if (buttonThreeState) {
      buttonThreePressed();
    }
  }
  lastButtonThreeState = buttonThreeState;

  // Button Four
  if (buttonFourState != lastButtonFourState) {
    if (buttonFourState) {
      buttonFourPressed();
    }
  }
  lastButtonFourState = buttonFourState;

  // Button Five
  if (buttonFiveState != lastButtonFiveState) {
    if (buttonFiveState) {
      buttonFivePressed();
    }
  }
  lastButtonFiveState = buttonFiveState;

  // Button Six
  if (buttonSixState != lastButtonSixState) {
    if (buttonSixState) {
      buttonSixPressed();
    }
  }
  lastButtonSixState = buttonSixState;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    publishSensors();

    Serial << screenLEDsOff << endl;
  }
}
