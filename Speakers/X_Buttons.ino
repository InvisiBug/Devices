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
void buttonOnePressed()
{
  digitalWrite(l1Relay, !digitalRead(l1Relay));
  publishAll();
}

void buttonTwoPressed()
{
  digitalWrite(l2Relay, !digitalRead(l2Relay));
  publishAll();
}

void buttonThreePressed()
{
  digitalWrite(l3Relay, !digitalRead(l3Relay));
  publishAll();
}

void buttonFourPressed()
{
  digitalWrite(l4Relay, !digitalRead(l4Relay));
  publishAll();
}