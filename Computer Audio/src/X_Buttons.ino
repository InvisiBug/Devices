////////////////////////////////////////////////////////////////////////
//
// ######
// #     # #    # ##### #####  ####  #    #  ####
// #     # #    #   #     #   #    # ##   # #
// ######  #    #   #     #   #    # # #  #  ####
// #     # #    #   #     #   #    # #  # #      #
// #     # #    #   #     #   #    # #   ## #    #
// ######   ####    #     #    ####  #    #  ####
//
////////////////////////////////////////////////////////////////////////
void button1Clicked() {
  digitalWrite(l1Relay, !digitalRead(l1Relay));
  publishAll();
}

void button2Clicked() {
  digitalWrite(l2Relay, !digitalRead(l2Relay));
  publishAll();
}

void button3Clicked() {
  digitalWrite(l3Relay, !digitalRead(l3Relay));
  publishAll();
}

void button4Clicked() {
  digitalWrite(l4Relay, !digitalRead(l4Relay));
  publishAll();
}