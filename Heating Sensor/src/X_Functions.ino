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

    // if(sensor.beginI2C() == false) //Begin communication over I2C
    // {
    //   Serial.println("The sensor did not respond. Please check wiring.");
    // }
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
  pinMode(sensorPowerPin, OUTPUT);
  // pinMode(batterySensePin, OUTPUT);

  digitalWrite(sensorPowerPin, HIGH);
  // digitalWrite(batterySensePin, HIGH);

  if (sensor.beginI2C() == false)  //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
  }

  sensor.setTemperatureCorrection(correctionFactor);
}

void disableSensor() {
  digitalWrite(sensorPowerPin, LOW);
}

void enableSensor() {
  digitalWrite(sensorPowerPin, HIGH);
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
float checkBattery() {
  int raw;
  float volt;

  raw = analogRead(A0);

  volt = raw / 1023.0;
  volt = volt * 4.2;

  volt = volt - 0.06;  // Correction factor

  return mapFloat(volt, 2.5, 4.2, 0.0, 100.0);
  // return volt;
}

float mapFloat(float val, float inMin, float inMax, float outMin, float outMax) {
  return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float getTemperature() {
  float temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;

  // if(temperature > 50)
  // {
  //   temperature = null;
  // }

  return temperature;
}