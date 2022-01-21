////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Heating Sensor.ino
//  2020
//
//  Pins
//  D1 - SCL
//  D2 - SDA
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
//
////////////////////////////////////////////////////////////////////////
#include <ArduinoJson.h>   // Json
#include <ArduinoOTA.h>    // OTA
#include <NTPClient.h>     // Time
#include <PubSubClient.h>  // MQTT
// #include <SparkFunBME280.h>  // BME 280 Library
#include <BMx280I2C.h>
#include <SPI.h>
#include <Streaming.h>  // Serial Printouts
#include <String.h>
#include <WiFiClient.h>  //
#include <Wire.h>        // SPI Comms

#define I2C_ADDRESS 0x77
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

// Sensor
// BME280 sensor;

BMx280I2C bmx280(I2C_ADDRESS);

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

const char *nodeName = "Front Study Sensor";  // change for different room
const char *nodePassword = "crm0xhvsmn";

const char *disconnectMsg = "Front Study Sensor Disconnected";

const char *mqttServerIP = "mqtt.kavanet.io";

bool WiFiConnected = false;

long interval = (5 * 1000);  // Wait time before taking reading
unsigned long previousMillis = 0;

long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;

float temperature, humidity, pressure;

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
  digitalWrite(D6, HIGH);

  // startWifi();
  // startMQTT();

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
  // handleWiFi();
  // handleMQTT();
  // ArduinoOTA.handle();

  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;

  //   if (WiFiConnected) {
  //     publishSensors();
  //   }
  // }

  delay(1000);

  // start a measurement
  if (!bmx280.measure()) {
    Serial.println("could not start measurement, is a measurement already running?");
    return;
  }

  // wait for the measurement to finish
  do {
    delay(100);
  } while (!bmx280.hasValue());

  Serial.print("Pressure: ");
  Serial.println(bmx280.getPressure());
  Serial.print("Pressure (64 bit): ");
  Serial.println(bmx280.getPressure64());
  Serial.print("Temperature: ");
  Serial.println(bmx280.getTemperature());

  // important: measurement data is read from the sensor in function hasValue() only.
  // make sure to call get*() functions only after hasValue() has returned true.
  if (bmx280.isBME280()) {
    Serial.print("Humidity: ");
    Serial.println(bmx280.getHumidity());
  }
}