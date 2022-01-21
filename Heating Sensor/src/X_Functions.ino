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

  // if ((WiFi.status() == WL_CONNECTED) && (!WiFiConnected)) {
  //   WiFiConnected = true;

  //   digitalWrite(connectionLED, OFF);  // Turn off onboard led when connected

  //   Serial << endl;
  //   Serial << "| Connected to " << wifiSsid << " |" << endl;
  //   Serial << "| IP address : " << WiFi.localIP() << " |" << endl;

  //   if (sensor.beginI2C() == false)  // Begin communication over I2C
  //   {
  //     Serial.println("The sensor did not respond. Please check wiring.");
  //   }
  // }
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
void startSensors() {
  Wire.begin();
  // if (sensor.beginI2C() == false) {  //Begin communication over I2C
  //   Serial.println("The sensor did not respond. Please check wiring.");
  // }

  if (!bmx280.begin()) {
    Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
    while (1)
      ;
  }

  if (bmx280.isBME280())
    Serial.println("sensor is a BME280");
  else
    Serial.println("sensor is a BMP280");

  // reset sensor to default parameters.
  bmx280.resetToDefaults();

  // by default sensing is disabled and must be enabled by setting a non-zero
  // oversampling setting.
  // set an oversampling setting for pressure and temperature measurements.
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);

  // if sensor is a BME280, set an oversampling setting for humidity measurements.
  if (bmx280.isBME280())
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
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
// float mapFloat(float val, float inMin, float inMax, float outMin, float outMax) {
//   return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
// }

// float getTemperature() {
//   float temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;
//   return temperature;
// }