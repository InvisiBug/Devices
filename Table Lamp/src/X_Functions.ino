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
  WiFi.mode(WIFI_OFF);  // Clears the last wifi credentials
  WiFi.mode(WIFI_STA);  // Wifi Modes (WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA)
  WiFi.begin(wifiSsid, wifiPassword);
  WiFi.hostname(nodeName);

  while (WiFi.status() != WL_CONNECTED)  // Wait for wifi connection
  {
    Serial << ".";
    delay(500);
    digitalWrite(wifiLED, ON);  // Turn on green led when connecting
  }
  digitalWrite(wifiLED, OFF);  // Turn off green led when connection is established

  Serial << endl;
  Serial << "| Connected to " << wifiSsid << " |" << endl;
  Serial << "| IP address  : " << WiFi.localIP() << " |" << endl;
}