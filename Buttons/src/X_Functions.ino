////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Kavanet
//  Functions.ino
//  2019
//  *********
////////////////////////////////////////////////////////////////////////
//
//  #     #   #######
//  #  #  # # #       #
//  #  #  # # #       #
//  #  #  # # #####   #
//  #  #  # # #       #
//  #  #  # # #       #
//   ## ##  # #       #
//
////////////////////////////////////////////////////////////////////////
void startWifi() {
  WiFi.mode(WIFI_OFF);                 // Clears the last wifi credentials
  WiFi.mode(WIFI_STA);                 // Wifi Modes (WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA)
  WiFi.begin(wifiSsid, wifiPassword);  // Dont put give the ESP a host name, it screws up the wifi causing disconnects
}

void handleWiFi() {
  if (WiFi.status() != WL_CONNECTED)  // Wait for wifi connection
  {
    WiFiConnected = false;
    digitalWrite(connectionLED, ON);  // Turn on onboard led when connecting

    long now = millis();
    if (now - lastWiFiReconnectAttempt > connectionTimeout) {
      lastWiFiReconnectAttempt = now;
      Serial << ".";
    }
  }

  if ((WiFi.status() == WL_CONNECTED) && (!WiFiConnected)) {
    WiFiConnected = true;
    digitalWrite(connectionLED, OFF);  // Turn off onboard led when connected
    Serial << endl;
    Serial << "| Connected to " << wifiSsid << " |" << endl;
    Serial << "| IP address : " << WiFi.localIP() << " |" << endl;
  }
}

////////////////////////////////////////////////////////////////////////
//
//  ####### #######    #
//  #     #    #      # #
//  #     #    #     #   #
//  #     #    #    #     #
//  #     #    #    #######
//  #     #    #    #     #
//  #######    #    #     #
//
////////////////////////////////////////////////////////////////////////
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

void publishSensors() {
  float temp = dht.getTemperature();
  float humid = dht.getHumidity();

  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  doc["node"] = "Our Room Heating Sensor";
  doc["temperature"] = temp;
  doc["humidity"] = humid;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Our Room Heating Sensor", buffer, n);
}