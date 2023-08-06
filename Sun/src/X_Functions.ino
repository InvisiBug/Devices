////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Nest
//  Functions.ino
//  2017
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
//  #####                                      ######
// #     # #   #  ####  ##### ###### #    #    #     #   ##   #####   ##
// #        # #  #        #   #      ##  ##    #     #  #  #    #    #  #
//  #####    #    ####    #   #####  # ## #    #     # #    #   #   #    #
//       #   #        #   #   #      #    #    #     # ######   #   ######
// #     #   #   #    #   #   #      #    #    #     # #    #   #   #    #
//  #####    #    ####    #   ###### #    #    ######  #    #   #   #    #
//
////////////////////////////////////////////////////////////////////////
void changeRelayState() {
  if (relayState)
    relayOff();
  else
    relayOn();
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

void handleOverrunTimer() {
  // if(overRunTimerStart)
  // {
  //   unsigned long currentTime = millis();
  //   // interfaceBoost = true;

  //   if(currentTime - overRunTimerStart > 60 * 60 * 1000) // 60 minutes
  //   // if(currentTime - overRunTimerStart > 5 * 1000)  // 5 seconds
  //   {
  //     overRunTimerStart = 0;

  //     boostActive = false;
  //     interfaceBoost = false;
  //     Serial << "Boost Finish" << endl;
  //     // boost       = false;

  //     publishData();
  //   }
  // }
}