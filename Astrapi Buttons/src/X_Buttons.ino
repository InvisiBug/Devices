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
  snprintf(msg, mqttLen, "%c %s", '1', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
}

void button1Held() {
  snprintf(msg, mqttLen, "%c %s", '0', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '2', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
}

void button2Held() {
  snprintf(msg, mqttLen, "%c %s", '8', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '3', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '4', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '5', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '6', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
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
  snprintf(msg, mqttLen, "%c %s", '7', "Astrapi Control");
  mqtt.publish("Astrapi Control", msg);
}

void button7Held() {
}