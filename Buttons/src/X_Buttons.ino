////////////////////////////////////////////////////////////////////////
//
//  ######                                        #
//  #     # #    # ##### #####  ####  #    #     ##
//  #     # #    #   #     #   #    # ##   #    # #
//  ######  #    #   #     #   #    # # #  #      #
//  #     # #    #   #     #   #    # #  # #      #
//  #     # #    #   #     #   #    # #   ##      #
//  ######   ####    #     #    ####  #    #    #####
//
///////////////////////////////////////////////////////////////////////
void button1Clicked() {
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

void button1Held() {
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                       #####
//  #     # #    # ##### #####  ####  #    #    #     #
//  #     # #    #   #     #   #    # ##   #          #
//  ######  #    #   #     #   #    # # #  #     #####
//  #     # #    #   #     #   #    # #  # #    #
//  #     # #    #   #     #   #    # #   ##    #
//  ######   ####    #     #    ####  #    #    #######
//
///////////////////////////////////////////////////////////////////////
void button2Clicked() {
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

void button2Held() {
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
////////////////////////////////////////////////////////////////////////
//
//  ######                                       #####
//  #     # #    # ##### #####  ####  #    #    #     #
//  #     # #    #   #     #   #    # ##   #          #
//  ######  #    #   #     #   #    # # #  #     #####
//  #     # #    #   #     #   #    # #  # #          #
//  #     # #    #   #     #   #    # #   ##    #     #
//  ######   ####    #     #    ####  #    #     #####
//
///////////////////////////////////////////////////////////////////////
void button3Clicked() {
  Serial << "Lamp" << endl;
  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  if (tableLampOff) {
    doc["red"] = tableLampRed;
    doc["green"] = tableLampGreen;
    doc["blue"] = tableLampBlue;
  } else {
    doc["red"] = 0;
    doc["green"] = 0;
    doc["blue"] = 0;
  }

  tableLampOff = !tableLampOff;

  char buffer[512];
  size_t n = serializeJson(doc, buffer);
  mqtt.publish("Table Lamp Control", buffer, n);
}

void button3Held() {
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                      #
//  #     # #    # ##### #####  ####  #    #    #    #
//  #     # #    #   #     #   #    # ##   #    #    #
//  ######  #    #   #     #   #    # # #  #    #    #
//  #     # #    #   #     #   #    # #  # #    #######
//  #     # #    #   #     #   #    # #   ##         #
//  ######   ####    #     #    ####  #    #         #
//
///////////////////////////////////////////////////////////////////////
void button4Clicked() {
  Serial << "Egg Chair" << endl;
  if (eggChair) {
    snprintf(msg, mqttLen, "%s", "{\"state\":\"off\"}");
    mqtt.publish("zigbee2mqtt/eggChair/set", msg);
  } else {
    snprintf(msg, mqttLen, "%s", "{\"state\":\"on\"}");
    mqtt.publish("zigbee2mqtt/eggChair/set", msg);
  }
}

void button4Held() {
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                      #######
//  #     # #    # ##### #####  ####  #    #    #
//  #     # #    #   #     #   #    # ##   #    #
//  ######  #    #   #     #   #    # # #  #    ######
//  #     # #    #   #     #   #    # #  # #          #
//  #     # #    #   #     #   #    # #   ##    #     #
//  ######   ####    #     #    ####  #    #     #####
//
///////////////////////////////////////////////////////////////////////
void button5Clicked() {
  Serial << "Floodlight" << endl;
  if (floodlightOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Floodlight Control");
    mqtt.publish("Floodlight Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Floodlight Control");
    mqtt.publish("Floodlight Control", msg);
  }
}

void button5Held() {
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                       #####
//  #     # #    # ##### #####  ####  #    #    #     #
//  #     # #    #   #     #   #    # ##   #    #
//  ######  #    #   #     #   #    # # #  #    ######
//  #     # #    #   #     #   #    # #  # #    #     #
//  #     # #    #   #     #   #    # #   ##    #     #
//  ######   ####    #     #    ####  #    #     #####
//
///////////////////////////////////////////////////////////////////////
void button6Clicked() {
  Serial << "Lamp" << endl;
  if (lampOff) {
    snprintf(msg, mqttLen, "%c %s", '1', "Study Lamp Control");
    mqtt.publish("Study Lamp Control", msg);
  } else {
    snprintf(msg, mqttLen, "%c %s", '0', "Study Lamp Control");
    mqtt.publish("Study Lamp Control", msg);
  }
}

void button6Held() {
}
////////////////////////////////////////////////////////////////////////
//
//  ######                                      #######
//  #     # #    # ##### #####  ####  #    #    #    #
//  #     # #    #   #     #   #    # ##   #        #
//  ######  #    #   #     #   #    # # #  #       #
//  #     # #    #   #     #   #    # #  # #      #
//  #     # #    #   #     #   #    # #   ##      #
//  ######   ####    #     #    ####  #    #      #
//
///////////////////////////////////////////////////////////////////////
void button7Clicked() {
  Serial << "Speakers" << endl;
  if (audioOff) {
    snprintf(msg, mqttLen, "%i", 1);
    mqtt.publish("Computer Audio Control", msg);
  } else {
    snprintf(msg, mqttLen, "%i", 0);
    mqtt.publish("Computer Audio Control", msg);
  }
}

void button7Held() {
}