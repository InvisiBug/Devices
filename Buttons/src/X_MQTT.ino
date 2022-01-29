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
    } else {
      mqtt.loop();
    }
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  // printMessage(payload, length);

  if (!strcmp(topic, "Computer Power")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["state"] == false) {
      computerOff = true;
    } else {
      computerOff = false;
    }

    // Serial << "Computer: " << !computerOff << endl;
  }

  if (!strcmp(topic, "Plug")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["state"] == false) {
      floodlightOff = true;
    } else {
      floodlightOff = false;
    }

    // Serial << "Plug: " << !floodlightOff << endl;
  }

  if (!strcmp(topic, "Sun")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);
    // Serial << doc["state"] << endl;

    if (doc["state"] == false) {
      sunOff = true;
    } else {
      sunOff = false;
    }

    // Serial << "Sun: " << !sunOff << endl;
  }

  if (!strcmp(topic, "Computer Audio")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if (doc["Mixer"] == false) {
      audioOff = true;
    } else {
      audioOff = false;
    }

    // Serial << "Audio : " << !audioOff << endl;
  }

  if (!strcmp(topic, "Desk LEDs")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      deskOff = false;

      deskLEDsRed = doc["red"];
      deskLEDsGreen = doc["green"];
      deskLEDsBlue = doc["blue"];
      // Serial << topic << "\t Red: " << deskLEDsRed << "\t Green: " << deskLEDsGreen << "\t Blue: " << deskLEDsBlue << endl;
    } else {
      deskOff = true;
    }
  }

  if (!strcmp(topic, "Table Lamp")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      lampOff = false;

      tableLampRed = doc["red"];
      tableLampGreen = doc["green"];
      tableLampBlue = doc["blue"];
      // Serial << topic << "\t Red: " << tableLampRed << "\t Green: " << tableLampGreen << "\t Blue: " << tableLampBlue << endl;
    } else {
      lampOff = true;
    }
  }

  if (!strcmp(topic, "Screen LEDs")) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);

    if ((doc["red"] > 0) or (doc["green"] > 0) or (doc["blue"] > 0)) {
      screenLEDsOff = false;

      screenLEDsRed = doc["red"];
      screenLEDsGreen = doc["green"];
      screenLEDsBlue = doc["blue"];

      // Serial << topic << "\t Red: " << screenLEDsRed << "\t Green: " << screenLEDsGreen << "\t Blue: " << screenLEDsBlue << endl;
    } else if (doc["mode"] == 0) {
      screenLEDsOff = true;
      ambientMode = false;
    } else if (doc["mode"] == 1) {
      screenLEDsOff = false;
      ambientMode = true;
    } else {
      screenLEDsOff = true;
    }
  }
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
  // mqtt.subscribe("Computer Audio");
  // mqtt.subscribe("Computer Power");
  // mqtt.subscribe("Screen LEDs");
  // mqtt.subscribe("Table Lamp");
  // mqtt.subscribe("Desk LEDs");
  // mqtt.subscribe("Sun");

  mqtt.subscribe("#");
}