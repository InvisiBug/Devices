////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh 
// 
//  Kavanet 
//  ino
//  2019
//  *********
//  29/01/2019  Forked from old system and cleaned up
//  05/02/2019  Added indicator light

// Add an interupt for the button
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
#include <PubSubClient.h> // MQTT
#include <ArduinoJson.h>  // Json Library
#include <ESP8266WiFi.h>  // WiFi
#include <ArduinoOTA.h>   // OTA
#include <WiFiClient.h>   // May not be needed
#include <Streaming.h>    // Serial Printouts
#include <NTPClient.h>    // Time
#include <string.h>       // Used for node disconnect message

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
#define NTP_ADDRESS  "uk.pool.ntp.org" // Uk time server
#define NTP_OFFSET   3600              // In seconds (3600 for uk)
#define NTP_INTERVAL 60 * 1000         // In miliseconds

#define relayPin  12 // 12 This pin also has a blue LED
#define statusLED 13 
#define buttonPin 0

#define ON  LOW // Check this later
#define OFF HIGH

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
// MQTT Client
WiFiClient espClient;
PubSubClient mqtt(espClient);

// Time Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

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
char* wifiSsid      = "I Don't Mind";
char* wifiPassword  = "Have2Biscuits";

char* nodeName      = "Plug";
char* disconnectMsg = "Plug Disconnected";
char* nodePassword  = "crm0xhvsmn";

char* mqttServerIP = "192.168.1.46";

// const char* mqttServerIP = "broker.mqttdashboard.com";  // Use this one for testing without local broker

long interval = (5 * 1000); // Update every 5 seconds
unsigned long previousMillis = 0;

bool relayState;
bool buttonState;
bool lastButtonState;

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
  pinMode(buttonPin,  INPUT); 
  pinMode(statusLED, OUTPUT); 
  pinMode(relayPin,  OUTPUT);

  Serial.begin(115200);
  Serial << "\n| " << nodeName << " |" << endl;

  startWifi();
  startMQTT();
  startOTA();
  startTime();
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
{
  handleMQTT();
  ArduinoOTA.handle();

  buttonState = !digitalRead(buttonPin);

  if(buttonState != lastButtonState)
  {
    if(buttonState == HIGH) 
    {      
     changeRelayState();
    } 
  }

  lastButtonState = buttonState;

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis >= interval) // 5 second update cycle
  {
    previousMillis = currentMillis;
    publishAll();
  }
}