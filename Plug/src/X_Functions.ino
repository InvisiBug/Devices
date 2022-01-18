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
// ######
// #     # ###### #        ##   #   #
// #     # #      #       #  #   # #
// ######  #####  #      #    #   #
// #   #   #      #      ######   #
// #    #  #      #      #    #   #
// #     # ###### ###### #    #   #
//
////////////////////////////////////////////////////////////////////////
void changeRelayState() {
  if (relayState) {
    relayOff();
  } else {
    relayOn();
  }
}

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