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
////////////////////////////z////////////////////////////////////////////
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

    // if (sensor.beginI2C() == false)  //Begin communication over I2C
    // {
    //   Serial.println("The sensor did not respond. Please check wiring.");
    // }
  }
}

////////////////////////////////////////////////////////////////////////
//
//  #####
// #     # ###### #    #  ####   ####  #####   ####
// #       #      ##   # #      #    # #    # #
//  #####  #####  # #  #  ####  #    # #    #  ####
//       # #      #  # #      # #    # #####       #
// #     # #      #   ## #    # #    # #   #  #    #
//  #####  ###### #    #  ####   ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
// void startSensors() {
//   if (sensor.beginI2C() == false) {  //Begin communication over I2C
//     Serial.println("The sensor did not respond. Please check wiring.");
//   }
// }

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
// float mapFloat(float val, float inMin, float inMax, float outMin, float outMax) {
//   return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
// }

// float getTemperature() {
//   float temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;
//   return temperature;
// }