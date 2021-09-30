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
#include <ArduinoJson.h>  // Json
#include <ArduinoOTA.h>   // OTA
#include <DallasTemperature.h>
#include <NTPClient.h>  // Time
#include <OneWire.h>
#include <PubSubClient.h>  // MQTT
#include <Streaming.h>     // Serial Printouts
#include <String.h>
#include <WiFiClient.h>  //

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

#define ONE_WIRE_BUS 4

#define ON LOW  // Confirmed for Wemos D1 Mini (On - High for esp 32)
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

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress tempDeviceAddress;

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

const char *nodeName = "Radiator Monitor";  // change for different room
const char *nodePassword = "crm0xhvsmn";

const char *disconnectMsg = "Radiator Monitor Disconnected";

const char *mqttServerIP = "mqtt.kavanet.io";

// char inletId = "28A52707D6013C97";

bool WiFiConnected = false;

long interval = (5 * 1000);  // Wait time before taking reading
unsigned long previousMillis = 0;

long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

float temperature, humidity, pressure;

int numberOfDevices;

float inletTemp, outletTemp;

// int correctionFactor = -2.0;

// boopest?
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

  startWifi();
  startMQTT();
  startOTA();

  startSensors();
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

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    sensors.requestTemperatures();  // Send the command to get temperatures

    for (int i = 0; i < numberOfDevices; i++) {
      if (sensors.getAddress(tempDeviceAddress, i)) {
        if (i == 0) {
          inletTemp = sensors.getTempC(tempDeviceAddress);
        } else if (i == 1) {
          outletTemp = sensors.getTempC(tempDeviceAddress);
        }
      }
      Serial << "Inlet Temp: " << inletTemp << "\t Outlet Temp: " << outletTemp << endl;
    }

    if (WiFiConnected) {
      publishSensors();
      // Serial << "Sending Data" << endl;
    }
  }
}