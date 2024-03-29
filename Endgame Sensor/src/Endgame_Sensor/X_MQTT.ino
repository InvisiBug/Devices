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
// ######
// #     # ######  ####  ###### # #    # ###### #####
// #     # #      #    # #      # #    # #      #    #
// ######  #####  #      #####  # #    # #####  #    #
// #   #   #      #      #      # #    # #      #    #
// #    #  #      #    # #      #  #  #  #      #    #
// #     # ######  ####  ###### #   ##   ###### #####
//
////////////////////////////////////////////////////////////////////////
void messageReceived(char* topic, byte* payload, unsigned int length) {
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
void publish() {
  if (WiFiConnected) {
    StaticJsonDocument<192> doc;

    doc["node"] = nodeName;
    doc["temperature"] = getTemperature();
    doc["humidity"] = getHumidity();
    doc["pressure"] = getPressure();
    doc["brightness"] = getBrightness();
    doc["motion"] = getMotion(false);

    char buffer[192];
    size_t n = serializeJson(doc, buffer);
    mqtt.publish(nodeName, buffer, n);
    Serial << "Sending Data" << endl;

    delay(50);  // Think this stops the disconnect issue
  }

  // mqtt.beginPublish(nodeName, measureJson(doc));
  // WriteBufferingPrint bufferedClient(mqtt, 32);
  // serializeJson(doc, bufferedClient);
  // bufferedClient.flush();
  // mqtt.endPublish();

  // const size_t capacity = JSON_OBJECT_SIZE(7);  // This should only be 3 not 6, think its a bug
  // DynamicJsonDocument doc(capacity);

  // doc["node"] = nodeName;
  // doc["temperature"] = getTemperature();
  // doc["humidity"] = ((float)((int)(sensor.readFloatHumidity() * 10))) / 10;
  // doc["pressure"] = round(sensor.readFloatPressure() * 100) / 100;

  // char buffer[512];
  // size_t n = serializeJson(doc, buffer);
  // mqtt.publish(nodeName, buffer, n);
  // Serial << "Sending Data" << endl;

  // mqtt.publish(nodeName, "hello", 5);
  // Serial << "Sending Data" << endl;
}

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
  mqtt.setBufferSize(512);
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

      long now = millis();
      if (now - previousMillis >= interval) {
        previousMillis = now;

        publish();
      }
    }
  }
}

void printMessage(byte* payload, int length) {
  for (int i = 0; i < length; i++) {
    Serial << (char)payload[i];  // No endl here
  }
  Serial << endl;
}
