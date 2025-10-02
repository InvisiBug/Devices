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
  // Serial << "Message" << endl;
  printMessage(payload, length);

  // if (length > 1)  // Colours
  // {
  //   // StaticJsonDocument<256> doc;
  //   // deserializeJson(doc, payload, length);

  //   JsonDocument doc;
  //   deserializeJson(doc, payload, length);

  //   red = doc["red"];
  //   green = doc["green"];
  //   blue = doc["blue"];

  //   Serial << "Red :" << red << " Green :" << green << " Blue: " << blue << endl;

  //   for (int i = 0; i < totalLEDs; i++) {
  //     currentLED[i].setRGB(red, green, blue);
  //   }
  //   FastLED.show();

  //   publishAll();
  // }

  // else  // Ambient Mode
  {
    char state = (char)payload[0];

    if (state == '0') {
      Serial << "Colours" << endl;
      mode = 0;

      digitalWrite(ambientPin, true);  // Off, Inverse Logic

      delay(500);

      for (int i = 0; i < totalLEDs; i++) {
        currentLED[i].setRGB(red, green, blue);
      }
      FastLED.show();

      publishAll();
    }

    else if (state == '1')  // 1 *NB* Check this
    {
      Serial << "Ambient" << endl;
      mode = 1;

      digitalWrite(ambientPin, false);  // On, Inverse Logic
      publishAll();
    }

    else if (state == '2')  // 2 *NB* Check this
    {
      Serial << "Rainbow" << endl;
      mode = 2;

      digitalWrite(ambientPin, true);  // Off, Inverse Logic

      publishAll();
    }

    else if (state == '3')  // 2 *NB* Check this
    {
      Serial << "Fade" << endl;
      mode = 3;

      digitalWrite(ambientPin, true);  // Off, Inverse Logic

      publishAll();
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
//  ######
//  #     # #    # #####  #      #  ####  #    #
//  #     # #    # #    # #      # #      #    #
//  ######  #    # #####  #      #  ####  ######
//  #       #    # #    # #      #      # #    #
//  #       #    # #    # #      # #    # #    #
//  #        ####  #####  ###### #  ####  #    #
//
////////////////////////////////////////////////////////////////////////
void publishAll() {
  // const size_t capacity = JSON_OBJECT_SIZE(6);
  // DynamicJsonDocument doc(capacity);

  // JsonDocument doc;

  // doc["Node"] = nodeName;
  // doc["red"] = red;
  // doc["green"] = green;
  // doc["blue"] = blue;
  // doc["mode"] = mode;

  // char buffer[512];

  // // size_t n = serializeJson(doc, buffer);
  // size_t n = serializeJson(doc, buffer);

  // Serial << buffer << endl;

  mqtt.publish(nodeName, "buffer", 10);
}