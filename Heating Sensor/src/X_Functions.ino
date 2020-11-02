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
//  #######
//     #    # #    # ######
//     #    # ##  ## #
//     #    # # ## # #####
//     #    # #    # #
//     #    # #    # #
//     #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
//void startTime()
//{
//  timeClient.begin();
//  timeClient.update();
//  timeClient.setTimeOffset(1); // Make this auto adjust for DST *NB*
//  Serial << "| Time " << timeClient.getFormattedTime() << " |" << endl;
//}

// int getTimestamp(long currentTime)
// {
//   // '2020-01-29T22:23:02+00:00'

//   // String month = "-" + String(month(currentTime));

//   int val = month(currentTime);

//   if()

//   // if(month.length )

//   // int timestamp = month.length();

//   // String timestamp;

//   // String timestamp = String(year(currentTime)) + "-" + String(month(currentTime)) + "-" + String(day(currentTime)) + "T" + String(hour(currentTime)) + ":" + String(minute(currentTime)) + ":" + String(second(currentTime));

//   return val;
// }

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
  digitalWrite(sensorPowerPin, HIGH);

  if (sensor.beginI2C() == false) {   //Begin communication over I2C
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
float mapFloat(float val, float inMin, float inMax, float outMin, float outMax) {
  return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float getTemperature() {
  float temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;
  return temperature;
}