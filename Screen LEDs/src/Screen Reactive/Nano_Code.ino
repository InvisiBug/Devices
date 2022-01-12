////////////////////////////////////////////////////////////////////////
//
//  ###                                                  
//   #  #    #  ####  #      #    # #####  ######  ####  
//   #  ##   # #    # #      #    # #    # #      #      
//   #  # #  # #      #      #    # #    # #####   ####  
//   #  #  # # #      #      #    # #    # #           # 
//   #  #   ## #    # #      #    # #    # #      #    # 
//  ### #    #  ####  ######  ####  #####  ######  ####  
//
////////////////////////////////////////////////////////////////////////
#include <FastLED.h>

////////////////////////////////////////////////////////////////////////
//
//  ######                                                        
//  #     # ###### ###### # #    # # ##### #  ####  #    #  ####  
//  #     # #      #      # ##   # #   #   # #    # ##   # #      
//  #     # #####  #####  # # #  # #   #   # #    # # #  #  ####  
//  #     # #      #      # #  # # #   #   # #    # #  # #      # 
//  #     # #      #      # #   ## #   #   # #    # #   ## #    # 
//  ######  ###### #      # #    # #   #   #  ####  #    #  #### 
//
////////////////////////////////////////////////////////////////////////
#define totalLEDs 142

#define PIN 2  // 2 for nano

#define serialRate 1000000
////////////////////////////////////////////////////////////////////////
//
//  #     #                                                  
//  #     #   ##   #####  #####  #    #   ##   #####  ###### 
//  #     #  #  #  #    # #    # #    #  #  #  #    # #      
//  ####### #    # #    # #    # #    # #    # #    # #####  
//  #     # ###### #####  #    # # ## # ###### #####  #      
//  #     # #    # #   #  #    # ##  ## #    # #   #  #      
//  #     # #    # #    # #####  #    # #    # #    # ######
//
////////////////////////////////////////////////////////////////////////
// LEDs
CRGB currentLED[totalLEDs];

////////////////////////////////////////////////////////////////////////
//
//  #     #                                                    
//  #     #   ##   #####  #   ##   #####  #      ######  ####  
//  #     #  #  #  #    # #  #  #  #    # #      #      #      
//  #     # #    # #    # # #    # #####  #      #####   ####  
//   #   #  ###### #####  # ###### #    # #      #           # 
//    # #   #    # #   #  # #    # #    # #      #      #    # 
//     #    #    # #    # # #    # #####  ###### ######  #### 
//
////////////////////////////////////////////////////////////////////////
// Adalight sends a "Magic Word" (defined in /etc/boblight.conf) before sending the pixel data
uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;


////////////////////////////////////////////////////////////////////////
//
//  ######                                                #####                                          
//  #     # #####   ####   ####  #####    ##   #    #    #     # #####   ##   #####  ##### #    # #####  
//  #     # #    # #    # #    # #    #  #  #  ##  ##    #         #    #  #  #    #   #   #    # #    # 
//  ######  #    # #    # #      #    # #    # # ## #     #####    #   #    # #    #   #   #    # #    # 
//  #       #####  #    # #  ### #####  ###### #    #          #   #   ###### #####    #   #    # #####  
//  #       #   #  #    # #    # #   #  #    # #    #    #     #   #   #    # #   #    #   #    # #      
//  #       #    #  ####   ####  #    # #    # #    #     #####    #   #    # #    #   #    ####  #      
//
////////////////////////////////////////////////////////////////////////
void setup()
{
  
  FastLED.addLeds<WS2812, PIN, GRB>(leds, totalLEDs);
  
  // initial RGB flash
  LEDS.showColor(CRGB(10, 0, 0));
  delay(100);
  LEDS.showColor(CRGB(0, 10, 0));
  delay(100);
  LEDS.showColor(CRGB(0, 0, 10));
  delay(100);
  LEDS.showColor(CRGB(0, 0, 0));
  
  Serial.begin(serialRate);
  Serial.print("Ada\n"); // Send "Magic Word" string to host
  
}

///////////////////////////////////////////////////////////////////////
//
//  #     #                    ######                                     
//  ##   ##   ##   # #    #    #     # #####   ####   ####  #####    ##   #    # 
//  # # # #  #  #  # ##   #    #     # #    # #    # #    # #    #  #  #  ##  ## 
//  #  #  # #    # # # #  #    ######  #    # #    # #      #    # #    # # ## # 
//  #     # ###### # #  # #    #       #####  #    # #  ### #####  ###### #    # 
//  #     # #    # # #   ##    #       #   #  #    # #    # #   #  #    # #    # 
//  #     # #    # # #    #    #       #    #  ####   ####  #    # #    # #    # 
//
///////////////////////////////////////////////////////////////////////
void loop() 
{ // wait for first byte of Magic Word
  for(i = 0; i < sizeof prefix; ++i) 
  {
    waitLoop: while (!Serial.available()) ;;
    // Check next byte in Magic Word
    if(prefix[i] == Serial.read()) continue;
    // otherwise, start over
    i = 0;
    goto waitLoop;
  }
  
  // Hi, Lo, Checksum
  
  while (!Serial.available()) ;;
  hi=Serial.read();
  while (!Serial.available()) ;;
  lo=Serial.read();
  while (!Serial.available()) ;;
  chk=Serial.read();
  
  // if checksum does not match go back to wait
  if (chk != (hi ^ lo ^ 0x55))
  {
    i=0;
    goto waitLoop;
  }
  
  memset(currentLED, 0, totalLEDs * sizeof(struct CRGB));

  // read the transmission data and set LED values
  for (uint8_t i = 0; i < totalLEDs; i++) 
  {
    byte r, g, b;    
    while(!Serial.available());
    r = Serial.read();
    while(!Serial.available());
    g = Serial.read();
    while(!Serial.available());
    b = Serial.read();
    currentLED[i].r = r;
    currentLED[i].g = g;
    currentLED[i].b = b;
  }
  // shows new values
 FastLED.show();
}
