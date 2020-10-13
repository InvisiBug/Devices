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
void buttonOnePressed() {
  if (ambientMode) {
    ambientMode = false;
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Screen LEDs Control", msg);
  } else {
    ambientMode = true;
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Screen LEDs Control", msg);
  }
}

void buttonTwoPressed() {
  if (audioOff) {
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Computer Audio Control", msg);
  } else {
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Computer Audio Control", msg);
  }
}

void buttonThreePressed() {
  if (plugOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Plug Control");
    mqtt.publish("Plug Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Plug Control");  // Sub
    mqtt.publish("Plug Control", msg);
  }
}

void buttonFourPressed() {
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  if (deskOff) {
    doc["red"] = deskLEDsRed;
    doc["green"] = deskLEDsGreen;
    doc["blue"] = deskLEDsBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  deskOff = !deskOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Desk LED Control", buffer, n);
}

void buttonFivePressed() {
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  if (lampOff) {
    doc["red"] = tableLampRed;
    doc["green"] = tableLampGreen;
    doc["blue"] = tableLampBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  lampOff = !lampOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Table Lamp Control", buffer, n);
}

void buttonSixPressed() {
  Serial << "Button Six" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  Serial << screenLEDsOff << endl;

  if (screenLEDsOff) {
    doc["red"] = screenLEDsRed;
    doc["green"] = screenLEDsGreen;
    doc["blue"] = screenLEDsBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  screenLEDsOff = !screenLEDsOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Screen LEDs Control", buffer, n);
}