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
  const size_t capacity = JSON_OBJECT_SIZE(3);  // This should only be 3 not 6, think its a bug
  DynamicJsonDocument doc(capacity);

  doc["node"] = nodeName;
  doc["inlet"] = inletTemp;
  doc["outlet"] = outletTemp;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish(nodeName, buffer, n);
}