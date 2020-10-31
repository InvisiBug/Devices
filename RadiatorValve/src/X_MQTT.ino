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
        } else {  // Not connected
          Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;
          digitalWrite(connectionLED, ON);
          delay(250);
          digitalWrite(connectionLED, OFF);
        }
      }
    } else {  // Connected
      mqtt.loop();
    }
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  // Serial << "Message Received" << endl;
  // printMessage(payload, length);

  if (!strcmp(topic, "Heating Control")) {
    if ((char)payload[0] == '1') {
      relayOn();
    } else if ((char)payload[0] == '0') {
      relayOff();
    }
  }
}

void printMessage(byte* payload, int length) {
  for (int i = 0; i < length; i++) {
    Serial << (char)payload[i];  // Dont put an endl here
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
  mqtt.subscribe(controlTopic);
}

////////////////////////////////////////////////////////////////////////
//
// ######
// #     # #    # #####  #      #  ####  #    #
// #     # #    # #    # #      # #      #    #
// ######  #    # #####  #      #  ####  ######
// #       #    # #    # #      #      # #    #
// #       #    # #    # #      # #    # #    #
// #        ####  #####  ###### #  ####  #    #
//
////////////////////////////////////////////////////////////////////////
void publishAll() {
  // Serial << "Sending MQTT Data" << endl;

  const size_t capacity = JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  doc["node"] = nodeName;
  doc["state"] = relayState;

  char buffer[512];

  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}