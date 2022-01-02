////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  2019
////////////////////////////////////////////////////////////////////////
// Relays
// L1 - 12
// L2 - 5
// L3 - 4
// L4 - 15

// LEDs are tied in to the relays

// Switches
// S1 - 0  - L1
// S2 - 9  - L2
// S3 - 10 - L3
// S4 - 14 - L4

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
#define connectionLED 13

#define ON LOW  // Need to check these *NB*
#define OFF HIGH

#define l1Relay 12
#define l2Relay 5
#define l3Relay 4
#define l4Relay 15

#define l1Button 0
#define l2Button 9
#define l3Button 10
#define l4Button 14

#define leftSpeaker l4Relay
#define rightSpeaker l3Relay
#define sub l2Relay
#define mixer l1Relay

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
char* wifiSsid = "I Don't Mind";
char* wifiPassword = "Have2Biscuits";

char* nodeName = "Computer Audio";
char* disconnectMsg = "Computer Audio Disconnected";
char* nodePassword = "crm0xhvsmn";

char* controlTopic = "Computer Audio Control";

char* mqttServerIP = "192.168.1.46";

bool WiFiConnected = false;

char msg[mqttLen];  // Buffer to store the MQTT messages

bool buttonOneState;
bool buttonTwoState;
bool buttonThreeState;
bool buttonFourState;

bool lastButtonOneState;
bool lastButtonTwoState;
bool lastButtonThreeState;
bool lastButtonFourState;

long interval = (5 * 1000);  // Update every 5 seconds
unsigned long previousMillis = 0;

long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

// int Left_Speaker;

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

  pinMode(l1Button, INPUT);
  pinMode(l2Button, INPUT);
  pinMode(l3Button, INPUT);
  pinMode(l4Button, INPUT);

  pinMode(l1Relay, OUTPUT);
  pinMode(l2Relay, OUTPUT);
  pinMode(l3Relay, OUTPUT);
  pinMode(l4Relay, OUTPUT);

  startWifi();
  startMQTT();
  startOTA();

  digitalWrite(l1Relay, true);
  digitalWrite(l2Relay, true);
  digitalWrite(l3Relay, true);
  digitalWrite(l4Relay, true);

  digitalWrite(leftSpeaker, true);
  delay(1000);
  digitalWrite(leftSpeaker, false);
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

  buttonOneState = digitalRead(l1Button);
  buttonTwoState = digitalRead(l2Button);
  buttonThreeState = digitalRead(l3Button);
  buttonFourState = digitalRead(l4Button);

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

  ///// Timed Events /////
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)  // 5 second update cycle
  {
    previousMillis = currentMillis;
    publishAll();
  }
}