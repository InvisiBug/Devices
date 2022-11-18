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
#include <ArduinoJson.h>  // Json Library
#include <ESP8266WiFi.h>  // WiFi
#include <OneButton.h>
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

OneButton button1(l1Button, true);
OneButton button2(l2Button, true);
OneButton button3(l3Button, true);
OneButton button4(l4Button, true);

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

char* mqttServerIP = "mqtt.kavanet.io";

bool WiFiConnected = false;

char msg[mqttLen];  // Buffer to store the MQTT messages

long interval = (5 * 1000);  // Update every 5 seconds
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

  button1.attachClick(button1Clicked);
  button1.setDebounceTicks(50);

  button2.attachClick(button2Clicked);
  button2.setDebounceTicks(50);

  button3.attachClick(button3Clicked);
  button3.setDebounceTicks(50);

  button4.attachClick(button4Clicked);
  button4.setDebounceTicks(50);

  pinMode(l1Relay, OUTPUT);
  pinMode(l2Relay, OUTPUT);
  pinMode(l3Relay, OUTPUT);
  pinMode(l4Relay, OUTPUT);

  digitalWrite(l1Relay, false);
  digitalWrite(l2Relay, false);
  digitalWrite(l3Relay, false);
  digitalWrite(l4Relay, false);

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

  button1.tick();
  button2.tick();
  button3.tick();
  button4.tick();

  ///// Timed Events /////
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)  // 5 second update cycle
  {
    previousMillis = currentMillis;
    publishAll();
  }
}
