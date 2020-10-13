void ambientClicked() {
  Serial << "Ambient" << endl;
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

void speakersClicked() {
  Serial << "Speakers" << endl;
  if (audioOff) {
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Computer Audio Control", msg);
  } else {
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Computer Audio Control", msg);
  }
}

void floodlightClicked() {
  Serial << "FloodLight" << endl;
  if (plugOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Plug Control");
    mqtt.publish("Plug Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Plug Control");
    mqtt.publish("Plug Control", msg);
  }
}

void sunClicked() {
  Serial << "Sun" << endl;
  if (sunOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Sun Control");
    mqtt.publish("Sun Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Sun Control");
    mqtt.publish("Sun Control", msg);
  }
}

void deskLEDsClicked() {
  Serial << "Desk LEDs" << endl;
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

void lampClicked() {
  Serial << "Lamp" << endl;
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

void screenLEDsClicked() {
  Serial << "Screen LEDs" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  // Serial << screenLEDsOff << endl;

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

void speakersHeld() {  // Boost on and off
  mqtt.publish("Heating Button", "1");
}