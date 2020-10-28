# 1 "/var/folders/_b/r0nrqkw52p567dwxktn010th0000gp/T/tmp4idepeck"
#include <Arduino.h>
# 1 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
# 21 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <OneButton.h>
#include <PubSubClient.h>
#include <Streaming.h>
#include <WiFiClient.h>
# 41 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
#define connectionLED LED_BUILTIN

#define button1Pin D1
#define button2Pin D2
#define button3Pin D3
#define button4Pin D4
#define button5Pin D5
#define button6Pin D6
#define button7Pin D7

#define ON LOW
#define OFF HIGH

#define mqttLen 50

#define mqttPrint true
# 70 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
WiFiClient espClient;
PubSubClient mqtt(espClient);

OneButton button1(button1Pin, true);
OneButton button2(button2Pin, true);
OneButton button3(button3Pin, true);
OneButton button4(button4Pin, true);
OneButton button5(button5Pin, true);
OneButton button6(button6Pin, true);
OneButton button7(button7Pin, true);
# 92 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
const char* wifiSsid = "I Don't Mind";
const char* wifiPassword = "Have2Biscuits";

const char* nodePassword = "crm0xhvsmn";
const char* nodeName = "Buttons 2";

const char* disconnectMsg = "Buttons Disconnected";

const char* mqttServerIP = "192.168.1.46";

bool WiFiConnected = false;

char msg[mqttLen];


bool deskOff = false;
bool lampOff = false;
bool audioOff = false;
bool plugOff = false;
bool computerOff = false;
bool screenLEDsOff = false;
bool sunOff = false;

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

long connectionTimeout = (2 * 1000);
long lastWiFiReconnectAttempt = 0;
long lastMQTTReconnectAttempt = 0;
# 144 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
void setup();
void loop();
void button1Clicked();
void button1Held();
void button2Clicked();
void button2Held();
void button3Clicked();
void button3Held();
void button4Clicked();
void button4Held();
void button5Clicked();
void button5Held();
void button6Clicked();
void button6Held();
void button7Clicked();
void button7Held();
void startWifi();
void handleWiFi();
void startOTA();
void startMQTT();
void handleMQTT();
void messageReceived(char* topic, byte* payload, unsigned int length);
void printMessage(byte* payload, int length);
void subscribeToTopics();
#line 144 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
void setup() {
  Serial.begin(115200);
  Serial << "\n|** " << nodeName << " **|" << endl;

  startWifi();
  startMQTT();
  startOTA();

  button1.attachClick(button1Clicked);
  button1.setDebounceTicks(50);
  button1.attachLongPressStart(button1Held);
  button1.setPressTicks(250);

  button2.attachClick(button2Clicked);
  button2.setDebounceTicks(50);
  button2.attachLongPressStart(button2Held);
  button2.setPressTicks(250);

  button3.attachClick(button3Clicked);
  button3.setDebounceTicks(50);
  button3.attachLongPressStart(button3Held);
  button3.setPressTicks(250);

  button4.attachClick(button4Clicked);
  button4.setDebounceTicks(50);
  button3.attachLongPressStart(button4Held);
  button4.setPressTicks(250);

  button5.attachClick(button5Clicked);
  button5.setDebounceTicks(50);
  button5.attachLongPressStart(button5Held);
  button5.setPressTicks(250);

  button6.attachClick(button6Clicked);
  button6.setDebounceTicks(50);
  button6.attachLongPressStart(button6Held);
  button6.setPressTicks(250);

  button7.attachClick(button7Clicked);
  button7.setDebounceTicks(50);
  button7.attachLongPressStart(button7Held);
  button7.setPressTicks(250);
}
# 199 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/Main.ino"
void loop() {
  handleWiFi();
  handleMQTT();
  ArduinoOTA.handle();

  button1.tick();
  button2.tick();
  button3.tick();
  button4.tick();
  button5.tick();
  button6.tick();
  button7.tick();
}
# 1 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
# 12 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button1Clicked() {
  Serial << "Desk LEDs" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  if (deskOff) {
    doc["red"] = deskLEDsRed;
    doc["green"] = deskLEDsGreen;
    doc["blue"] = deskLEDsBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  deskOff = !deskOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Desk LED Control", buffer, n);
}

void button1Held() {
}
# 47 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button2Clicked() {
  Serial << "Screen LEDs" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);



  if (screenLEDsOff) {
    doc["red"] = screenLEDsRed;
    doc["green"] = screenLEDsGreen;
    doc["blue"] = screenLEDsBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  screenLEDsOff = !screenLEDsOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Screen LEDs Control", buffer, n);
}

void button2Held() {
  Serial << "Ambient" << endl;
  if (ambientMode) {
    ambientMode = false;
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Screen LEDs Control", msg);
  } else {
    ambientMode = true;
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Screen LEDs Control", msg);
  }
}
# 94 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button3Clicked() {
  Serial << "Lamp" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  if (lampOff) {
    doc["red"] = tableLampRed;
    doc["green"] = tableLampGreen;
    doc["blue"] = tableLampBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  lampOff = !lampOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Table Lamp Control", buffer, n);
}

void button3Held() {
}
# 129 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button4Clicked() {
  Serial << "FloodLight" << endl;
  if (plugOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Plug Control");
    mqtt.publish("Plug Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Plug Control");
    mqtt.publish("Plug Control", msg);
  }
}

void button4Held() {
}
# 153 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button5Clicked() {
  Serial << "Speakers" << endl;
  if (audioOff) {
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Computer Audio Control", msg);
  } else {
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Computer Audio Control", msg);
  }
}

void button5Held() {
}
# 177 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button6Clicked() {
  Serial << "Sun" << endl;
  if (sunOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Sun Control");
    mqtt.publish("Sun Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Sun Control");
    mqtt.publish("Sun Control", msg);
  }
}

void button6Held() {
}
# 201 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Buttons.ino"
void button7Clicked() {

}

void button7Held() {
  mqtt.publish("Heating Button", "1");
}
# 1 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Functions.ino"
# 12 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Functions.ino"
void startWifi() {
  pinMode(connectionLED, OUTPUT);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
}

void handleWiFi() {
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFiConnected = false;
    digitalWrite(connectionLED, ON);

    long now = millis();
    if (now - lastWiFiReconnectAttempt > connectionTimeout) {
      lastWiFiReconnectAttempt = now;
      Serial << ".";
    }
  }

  if ((WiFi.status() == WL_CONNECTED) && (!WiFiConnected)) {
    WiFiConnected = true;
    digitalWrite(connectionLED, OFF);
    Serial << endl;
    Serial << "| Connected to " << wifiSsid << " |" << endl;
    Serial << "| IP address : " << WiFi.localIP() << " |" << endl;
  }
}
# 52 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_Functions.ino"
void startOTA() {
  ArduinoOTA.setHostname(nodeName);
  ArduinoOTA.setPassword(nodePassword);

  Serial << "| OTA Ready |" << endl;

  ArduinoOTA.onStart([]() {
    Serial << "Start OTA Upload" << endl;
  });

  ArduinoOTA.onEnd([]() {
    Serial << "OTA Upload Finished" << endl;
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial << "Progress: " << progress / (total / 100) << "%" << endl;
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial << "Authentication Failed" << endl;
    else if (error == OTA_BEGIN_ERROR)
      Serial << "Begin Failed" << endl;
    else if (error == OTA_CONNECT_ERROR)
      Serial << "Connect Failed" << endl;
    else if (error == OTA_RECEIVE_ERROR)
      Serial << "Receive Failed" << endl;
    else if (error == OTA_END_ERROR)
      Serial << "End Failed" << endl;
  });

  ArduinoOTA.begin();
}
# 1 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_MQTT.ino"
# 19 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_MQTT.ino"
void startMQTT() {
  mqtt.setServer(mqttServerIP, 1883);
  mqtt.setCallback(messageReceived);
}

void handleMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqtt.connected()) {
      long now = millis();
      if (now - lastMQTTReconnectAttempt > connectionTimeout) {
        lastMQTTReconnectAttempt = now;

        if (mqtt.connect(nodeName, nodeName, 1, 0, disconnectMsg)) {
          Serial << "| MQTT connection established |" << endl;
          subscribeToTopics();
          digitalWrite(connectionLED, OFF);
        }

        else {
          Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;

          digitalWrite(connectionLED, ON);
          delay(250);
          digitalWrite(connectionLED, OFF);
        }
      }
    } else {
      mqtt.loop();
    }
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length) {


  if (!strcmp(topic, "Computer Power")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["state"] == false) {
      computerOff = true;
    } else {
      computerOff = false;
    }


  }

  if (!strcmp(topic, "Plug")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["state"] == false) {
      plugOff = true;
    } else {
      plugOff = false;
    }


  }

  if (!strcmp(topic, "Sun")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);


    if (doc["state"] == false) {
      sunOff = true;
    } else {
      sunOff = false;
    }


  }

  if (!strcmp(topic, "Computer Audio")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["Mixer"] == false) {
      audioOff = true;
    } else {
      audioOff = false;
    }


  }

  if (!strcmp(topic, "Desk LEDs")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      deskOff = false;

      deskLEDsRed = doc["red"];
      deskLEDsGreen = doc["green"];
      deskLEDsBlue = doc["blue"];

    } else {
      deskOff = true;
    }
  }

  if (!strcmp(topic, "Table Lamp")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      lampOff = false;

      tableLampRed = doc["red"];
      tableLampGreen = doc["green"];
      tableLampBlue = doc["blue"];

    } else {
      lampOff = true;
    }
  }

  if (!strcmp(topic, "Screen LEDs")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      screenLEDsOff = false;

      screenLEDsRed = doc["red"];
      screenLEDsGreen = doc["green"];
      screenLEDsBlue = doc["blue"];


    } else if (doc["mode"] == 0) {
      screenLEDsOff = true;
      ambientMode = false;
    } else if (doc["mode"] == 1) {
      screenLEDsOff = false;
      ambientMode = true;
    } else {
      screenLEDsOff = true;
    }
  }
}

void printMessage(byte* payload, int length) {
  for (int i = 0; i < length; i++) {
    Serial << (char)payload[i];
  }
  Serial << endl;
}
# 181 "/Users/Matthew.Kavanagh/My Stuff/Projects/Kavanet/Devices/Buttons/src/X_MQTT.ino"
void subscribeToTopics() {
  mqtt.subscribe("Computer Audio");
  mqtt.subscribe("Computer Power");
  mqtt.subscribe("Screen LEDs");
  mqtt.subscribe("Table Lamp");
  mqtt.subscribe("Desk LEDs");
  mqtt.subscribe("Plug");
  mqtt.subscribe("Sun");
}