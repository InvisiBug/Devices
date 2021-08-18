////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Temperature.ino
//  2019
//  *********
//
//  Versions
//  V1 - Forked from temperature sensors - 13/04/2019
//
// Sensors
//
// Light
// Moisture
// Temperature
// Pressure

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
#include <Adafruit_BMP280.h>  // Temperature sensor
#include <ArduinoJson.h>      // Json Library
#include <ArduinoOTA.h>       // OTA
#include <NTPClient.h>        // Time
#include <PubSubClient.h>     // MQTT
#include <Streaming.h>        // Serial printouts
#include <WiFiClient.h>       // Changed from

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
#define moisturePin 34
#define lightPin 32
#define wifiLED 2

// Time
#define NTP_ADDRESS "uk.pool.ntp.org"  // Uk time server
#define NTP_OFFSET 60 * 60             // In seconds
#define NTP_INTERVAL 60 * 1000         // In miliseconds

// MQTT
#define qos 0

// Led's
#define ON LOW
#define OFF HIGH

// Soil Moisture Sensor
#define wetSoil 1900
#define drySoil 2549  // 3200

// Light sensor
#define night 4095
#define day 150

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

// Time Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// Temperature Sensor
Adafruit_BMP280 bmp;

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

const char* nodeName = "Plant Monitor";
const char* nodePassword = "crm0xhvsmn";

const char* mqttServerIP = "192.168.1.46";

const long sensorInterval = (5 * 1000);

unsigned long previousMillis = 0;

int temperature;  // °C
int pressure;     // Pa
int moisture;     // %
int light;        // %

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

  pinMode(wifiLED, OUTPUT);
  digitalWrite(wifiLED, OFF);  // Should be on (turned off for testing)

  startWifi();
  startMQTT();
  startSensors();
  startOTA();
  // startTime();
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
  unsigned long currentMillis = millis();

  ArduinoOTA.handle();
  // timeClient.update(); // This is needed to keep the time updated
  handleMQTT();

  if (currentMillis - previousMillis >= sensorInterval) {
    previousMillis = currentMillis;

    getSensorData();
    serialPrintSensorData();
    publishSensorData();
  }
}
