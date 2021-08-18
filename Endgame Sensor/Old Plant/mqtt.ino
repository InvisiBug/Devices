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
  if (!mqtt.connected()) {
    digitalWrite(wifiLED, ON);

    if (mqtt.connect(nodeName, nodeName, 1, 0, "Plant Monitor Disconnected"))  // Attempt to connect using a login
    {
      Serial << "| MQTT connection established |" << endl;  // Dont publish here, causes crashes
      subscribeTo();
      digitalWrite(wifiLED, OFF);
    }

    else {  // Make the light flash here
      delay(250);
      Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;
      digitalWrite(wifiLED, OFF);
      delay(250);
    }
  }
  mqtt.loop();
}

void messageReceived(char* topic, byte* payload, unsigned int length) {
  printMessage(payload, length);

  // else if(!strcmp(topic, "Sitrep")) publishSensorData();
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
void subscribeTo() {
  mqtt.subscribe("Sitrep");
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
void publishSensorData() {
  Serial << "Sending MQTT Data" << endl;

  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  doc["node"] = "Plant Monitor";

  JsonObject data = doc.createNestedObject("data");
  data["temperature"] = temperature;
  data["pressure"] = pressure;
  data["moisture"] = moisture;
  data["light"] = light;

  char buffer[156];
  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}