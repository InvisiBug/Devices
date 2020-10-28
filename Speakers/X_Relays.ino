////////////////////////////////////////////////////////////////////////
//
// ######                                    
// #     # ###### #        ##   #   #  ####  
// #     # #      #       #  #   # #  #      
// ######  #####  #      #    #   #    ####  
// #   #   #      #      ######   #        # 
// #    #  #      #      #    #   #   #    # 
// #     # ###### ###### #    #   #    ####  
//
////////////////////////////////////////////////////////////////////////
void powerOnSequence()
{
  digitalWrite(mixer, true);
  
  delay(750);
  
  digitalWrite(sub, true);

  delay(750);
  

  digitalWrite(leftSpeaker,  true);
  digitalWrite(rightSpeaker, true);

  publishAll();
}

void powerOffSequence()
{
  digitalWrite(leftSpeaker,  false);
  digitalWrite(rightSpeaker, false);

  delay(750);

  digitalWrite(sub, false);

  delay(750);

  digitalWrite(mixer, false);

  publishAll();
}