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

    if (mqtt.connect(nodeName, nodeName, 1, 0, disconnectMsg))  // Attempt to connect using a login
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
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);

  red = doc["red"];
  green = doc["green"];
  blue = doc["blue"];
  mode = doc["mode"];

  Serial << "Red :" << red << " Green :" << green << " Blue: " << blue << " Mode:" << mode << endl;

  publishAll();
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
  mqtt.subscribe("Table Lamp Control");
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
void publishAll()  // Send signal through as a value and process on the other end
{
  // Serial << "Sending MQTT Data" << endl;

  const size_t capacity = JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  Serial << nodeName << endl;

  doc["Node"] = nodeName;
  doc["red"] = red;
  doc["green"] = green;
  doc["blue"] = blue;
  // doc["Mode"]  = mode;

  char buffer[512];

  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}