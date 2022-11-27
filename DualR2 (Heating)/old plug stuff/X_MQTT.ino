////////////////////////////////////////////////////////////////////////
//  Matthew Kavanagh 
// 
//  Nest
//  MQTT.ino
//  2017
//  *********
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
void startMQTT()
{
  mqtt.setServer(mqttServerIP, 1883);
  mqtt.setCallback(messageReceived);
}

void handleMQTT()
{
  if(!mqtt.connected())
  {
    digitalWrite(statusLED, ON); // Turn on LED

    if(mqtt.connect(nodeName, nodeName, 1, 0, disconnectMsg)) // strcat(nodeName, " Disconnected")
    {
      Serial << "| MQTT connection established |" << endl; // Dont publish here, causes crashes  
      subscribeToTopics();
      digitalWrite(statusLED, OFF); // Turn off LED
    } 

    else 
    { // Make the light flash here
      delay(500);
      Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;
      digitalWrite(statusLED, OFF);
      delay(500);
    }
  }
  mqtt.loop();
}

void messageReceived(char* topic, byte* payload, unsigned int length)
{
  printMessage(payload, length);

  if(!strcmp(topic, "Plug Control"))
  {
    if((char)payload[0] == 'T') changeRelayState();
    else if((char)payload[0] == '1') relayOn();
    else if((char)payload[0] == '0') relayOff();
  }
}

void printMessage(byte* payload, int length)
{
  for(int i = 0; i < length; i++)
  {
    Serial << (char)payload[i]; // Dont put an endl here
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
void subscribeToTopics()
{
  mqtt.subscribe("Plug Control");
}

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
void publishAll() // Send signal through as a value and process on the other end
{
  Serial << "Sending MQTT Data" << endl;

  const size_t capacity = JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  doc["Node"]     = nodeName;
  doc["state"]    = relayState;

  char buffer[512];
  
  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}