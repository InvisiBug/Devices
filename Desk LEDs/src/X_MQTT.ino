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
  if(WiFi.status() == WL_CONNECTED)
  {
    if(!mqtt.connected())
    {
      long now = millis();
      if(now - lastMQTTReconnectAttempt > connectionTimeout) 
      {
        lastMQTTReconnectAttempt = now;

        if(mqtt.connect(nodeName, nodeName, 1, 0, disconnectMsg)) // Attempt to connect using a login
        {
          Serial << "| MQTT connection established |" << endl; // Dont publish here, causes crashes  
          subscribeToTopics();
        }

        else // Not connected
        {
          Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;

          delay(250);
        }
      }
    } 

    else // Connected
    {
      mqtt.loop();
    }
  }
}

void messageReceived(char* topic, byte* payload, unsigned int length)
{
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);

  red   = map(doc["red"],   0, 255, 0, 1024);
  green = map(doc["green"], 0, 255, 0, 1024);
  blue  = map(doc["blue"],  0, 255, 0, 1024);

  publishAll();

  Serial << "Red :" << red << " Green :" << green << " Blue: " << blue << endl;
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
  mqtt.subscribe(controlTopic);
}

////////////////////////////////////////////////////////////////////////
//
//  ######                                       
//  #     # #    # #####  #      #  ####  #    # 
//  #     # #    # #    # #      # #      #    # 
//  ######  #    # #####  #      #  ####  ###### 
//  #       #    # #    # #      #      # #    # 
//  #       #    # #    # #      # #    # #    # 
//  #        ####  #####  ###### #  ####  #    # 
//
////////////////////////////////////////////////////////////////////////
void publishAll() // Send signal through as 6a value and process on the other end
{
  // Serial << "Sending MQTT Data" << endl;

  const size_t capacity = JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);

  doc["Node"]  = nodeName;
  doc["red"]   = map(red,   0, 1024, 0, 255);
  doc["green"] = map(green, 0, 1024, 0, 255);
  doc["blue"]  = map(blue,  0, 1024, 0, 255);

  char buffer[512];
  
  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}