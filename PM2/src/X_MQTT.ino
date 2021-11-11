////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh
//
//  Nest
//  MQTT.ino
//  2017
//  *********
////////////////////////////////////////////////////////////////////////
//
//  #     #  #####  ####### #######
//  ##   ## #     #    #       #
//  # # # # #     #    #       #
//  #  #  # #     #    #       #
//  #     # #   # #    #       #
//  #     # #    #     #       #
//  #     #  #### #    #       #
//
///////////////////////////////////////////////////////////////////////
void startMQTT() {
  mqtt.setServer(mqttServerIP, 1883);
  mqtt.setCallback(messageReceived);
}

void handleMQTT() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!mqtt.connected()) {
      long now = millis();
      if (now - lastMQTTReconnectAttempt > connectionTimeout) {
        lastMQTTReconnectAttempt = now;

        if (mqtt.connect(nodeName, nodeName, 1, 0, disconnectMsg)) {  // Attempt to connect using a login
          Serial << "| MQTT connection established |" << endl;        // Dont publish here, causes crashes
          subscribeToTopics();
          digitalWrite(connectionLED, OFF);
        }

        else {  // Not connected
          Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;

          digitalWrite(connectionLED, ON);
          delay(250);
          digitalWrite(connectionLED, OFF);
        }
      }
    }

    else {  // Connected
      mqtt.loop();
    }
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
}

void printMessage(byte* payload, int length) {
  for (int i = 0; i < length; i++) {
    Serial << (char)payload[i];  // No endl here
  }
  Serial << endl;
}

////////////////////////////////////////////////////////////////////////
//
//   #####
//  #     # #    # #####   ####   ####  #####  # #####  ######
//  #       #    # #    # #      #    # #    # # #    # #
//   #####  #    # #####   ####  #      #    # # #####  #####
//        # #    # #    #      # #      #####  # #    # #
//  #     # #    # #    # #    # #    # #   #  # #    # #
//   #####   ####  #####   ####   ####  #    # # #####  ######
//
////////////////////////////////////////////////////////////////////////
void subscribeToTopics() {
}

////////////////////////////////////////////////////////////////////////
//
//  ######
//  #     #   ##   #####   ##
//  #     #  #  #    #    #  #
//  #     # #    #   #   #    #
//  #     # ######   #   ######
//  #     # #    #   #   #    #
//  ######  #    #   #   #    #
//
////////////////////////////////////////////////////////////////////////
void publishSensors() {
  pms.read();

  StaticJsonDocument<256> doc;

  doc["node"] = "pm2";
  doc["pm1"] = pms.pm01;
  doc["pm2.5"] = pms.pm25;
  doc["pm10"] = pms.pm10;

  JsonObject _100cc = doc.createNestedObject("#/100cc");
  _100cc["1"] = pms.n1p0;
  _100cc["5"] = pms.n5p0;
  _100cc["10"] = pms.n10p0;
  _100cc["0.3"] = pms.n0p3;
  _100cc["0.5"] = pms.n0p5;
  _100cc["2.5"] = pms.n2p5;
  doc["co2"] = ag.getCO2_Raw();

  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish(nodeName, buffer, n);

  Serial << buffer << endl;
}