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
          digitalWrite(connectionLED, OFF);
        }

        else // Not connected
        {
          Serial << "| MQTT connection failed, rc=" << mqtt.state() << " Trying again |" << endl;

          digitalWrite(connectionLED, ON);
          delay(250);
          digitalWrite(connectionLED, OFF);
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
  printMessage(payload, length);

  if(payload[0] != '{')
  {
    if((char)payload[0] == '1')      powerOnSequence();
    else if((char)payload[0] == '0') powerOffSequence();
  }

  else
  {
    StaticJsonDocument <256> doc;
    deserializeJson(doc, payload, length);

    bool Left_Speaker  = doc["left"];
    bool Right_Speaker = doc["right"];
    bool Sub           = doc["sub"];
    bool Mixer         = doc["mixer"];

    digitalWrite(leftSpeaker, Left_Speaker);
    digitalWrite(rightSpeaker, Right_Speaker);
    digitalWrite(sub, Sub);
    digitalWrite(mixer, Mixer);     
  }
  
  publishAll();  
}

void printMessage(byte* payload, int length)
{
  for(int i = 0; i < length; i++)
  {
    Serial << (char) payload[i]; // No endl here
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
  const size_t capacity = JSON_OBJECT_SIZE(6);
  DynamicJsonDocument doc(capacity);

  doc["Node"]  = nodeName;
  doc["Left"]  = digitalRead(leftSpeaker)  ? true : false;
  doc["Right"] = digitalRead(rightSpeaker) ? true : false;
  doc["Sub"]   = digitalRead(sub)          ? true : false;
  doc["Mixer"] = digitalRead(mixer)        ? true : false;

  char buffer[700];
  
  size_t n = serializeJson(doc, buffer);

  mqtt.publish(nodeName, buffer, n);
}