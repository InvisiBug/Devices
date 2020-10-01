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
  // digitalWrite(connectionLED, invertConnectionLED ? OFF : ON);
}

void handleWiFi() {
  if (WiFi.status() != WL_CONNECTED)  // Wait for wifi connection
  {
    WiFiConnected = false;
    // digitalWrite(connectionLED, invertConnectionLED ? OFF : ON);
    digitalWrite(connectionLED, ON);

    long now = millis();
    if (now - lastWiFiReconnectAttempt > connectionTimeout) {
      lastWiFiReconnectAttempt = now;
      Serial << ".";
    }
  }

  if ((WiFi.status() == WL_CONNECTED) && (!WiFiConnected)) {
    WiFiConnected = true;
    // digitalWrite(connectionLED, invertConnectionLED ? ON : OFF);  // Turn off onboard led when connected
    digitalWrite(connectionLED, OFF);
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

////////////////////////////////////////////////////////////////////////
//
// ######
// #     # ###### #        ##   #   #
// #     # #      #       #  #   # #
// ######  #####  #      #    #   #
// #   #   #      #      ######   #
// #    #  #      #      #    #   #
// #     # ###### ###### #    #   #
//
////////////////////////////////////////////////////////////////////////
void relayOn() {
  Serial << "Relay On" << endl;

  relayState = 1;
  digitalWrite(relayPin, 1);

  publishAll();
}

void relayOff() {
  Serial << "Relay Off" << endl;

  relayState = 0;
  digitalWrite(relayPin, 0);

  publishAll();
}