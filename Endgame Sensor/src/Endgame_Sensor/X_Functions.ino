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
  pinMode(connectionLED, OUTPUT);
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

///////////////////////////////////////////////////////////////////////
//
// #######
// #       #    # #    #  ####  ##### #  ####  #    #  ####
// #       #    # ##   # #    #   #   # #    # ##   # #
// #####   #    # # #  # #        #   # #    # # #  #  ####
// #       #    # #  # # #        #   # #    # #  # #      #
// #       #    # #   ## #    #   #   # #    # #   ## #    #
// #        ####  #    #  ####    #   #  ####  #    #  ####
//
///////////////////////////////////////////////////////////////////////
int getBrightness() {
  // return map(analogRead(lightPin), dark, light, 0, 100);
  return analogRead(lightPin);
}

float getTemperature() {
  float temperature;

#ifdef SENSOR
  // BME280 sensor;
  temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;
#else
  temperature = 12.34;
#endif
  return temperature;
}

float getHumidity() {
  float humidity;

#ifdef SENSOR
  // BME280 sensor;
  // temperature = ((float)((int)(sensor.readTempC() * 10))) / 10;
  humidity = ((float)((int)(sensor.readFloatHumidity() * 10))) / 10;
#else
  humidity = 43.21;
#endif
  return humidity;
}

float getPressure() {
  float pressure;

#ifdef SENSOR
  pressure = round(sensor.readFloatPressure() * 100) / 100;
#else
  pressure = 123456;
#endif
  return pressure;
}

bool getMotion(bool LEDOut) {
  return digitalRead(motionPin);
}

bool sendOnMotion(bool LEDOut) {
  if (digitalRead(motionPin)) {
    if (!previousMotionState) {
      publish();
      previousMotionState = true;
      if (LEDOut) {
        digitalWrite(LED_BUILTIN, ON);
      }
    }
  } else {
    previousMotionState = false;

    if (LEDOut) {
      digitalWrite(LED_BUILTIN, OFF);
    }
  }
}