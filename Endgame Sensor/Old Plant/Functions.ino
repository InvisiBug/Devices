////////////////////////////////////////////////////////////////////////
//
//  #     #   ####### 
//  #  #  # # #       #
//  #  #  # # #       #
//  #  #  # # #####   #
//  #  #  # # #       #
//  #  #  # # #       #
//   ## ##  # #       #
//
////////////////////////////////////////////////////////////////////////
void startWifi()
{
  WiFi.mode(WIFI_OFF);  // Clears the last wifi credentials
  WiFi.mode(WIFI_STA);  // Wifi Modes (WIFI_OFF, WIFI_STA, WIFI_AP, WIFI_AP_STA)
  WiFi.begin(wifiSsid, wifiPassword); 

  while(WiFi.status() != WL_CONNECTED) // Wait for wifi connection
  {    
    Serial << ".";
    delay(500);
    digitalWrite(wifiLED, ON); // Turn on green led when connecting
  }
  digitalWrite(wifiLED, OFF); // Turn off green led when connection is established

  Serial << endl;
  Serial << "| Connected to "    << wifiSsid       << " |" << endl;
  Serial << "| IP address  : "   << WiFi.localIP() << " |" << endl;
}

////////////////////////////////////////////////////////////////////////
//
//  ####### #######    #    
//  #     #    #      # #   
//  #     #    #     #   #  
//  #     #    #    #     # 
//  #     #    #    ####### 
//  #     #    #    #     # 
//  #######    #    #     # 
//
////////////////////////////////////////////////////////////////////////
void startOTA()
{
  ArduinoOTA.setHostname(nodeName);
  ArduinoOTA.setPassword(nodePassword);

  Serial << "| OTA Ready |" << endl;

  ArduinoOTA.onStart([]()
  {
    Serial << "Start OTA Upload" << endl;
  });

  ArduinoOTA.onEnd([]()
  {
    Serial << "OTA Upload Finished" << endl;
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial << "Progress: " << progress / (total / 100) << "%" << endl;
  });

  ArduinoOTA.onError([](ota_error_t error) 
  {
    Serial.printf("Error[%u]: ", error);
    if      (error == OTA_AUTH_ERROR)    Serial << "Authentication Failed" << endl;
    else if (error == OTA_BEGIN_ERROR)   Serial << "Begin Failed"          << endl;
    else if (error == OTA_CONNECT_ERROR) Serial << "Connect Failed"        << endl;
    else if (error == OTA_RECEIVE_ERROR) Serial << "Receive Failed"        << endl;
    else if (error == OTA_END_ERROR)     Serial << "End Failed"            << endl;
  });
  
  ArduinoOTA.begin();
}

////////////////////////////////////////////////////////////////////////
//
//  #######                 
//     #    # #    # ###### 
//     #    # ##  ## #      
//     #    # # ## # #####  
//     #    # #    # #      
//     #    # #    # #      
//     #    # #    # ###### 
// 
////////////////////////////////////////////////////////////////////////
void startTime()
{
  timeClient.begin();
  timeClient.update();
  Serial << "| Time " << timeClient.getFormattedTime() << " |" << endl;
}

//////////////////////////////////////////////////////////////////////////
//
//   #####                                            
//  #     # ###### #    #  ####   ####  #####   ####  
//  #       #      ##   # #      #    # #    # #      
//   #####  #####  # #  #  ####  #    # #    #  ####  
//        # #      #  # #      # #    # #####       # 
//  #     # #      #   ## #    # #    # #   #  #    # 
//   #####  ###### #    #  ####   ####  #    #  #### 
//
//////////////////////////////////////////////////////////////////////////
void startSensors()
{
  pinMode(moisturePin, INPUT);
  pinMode(lightPin, INPUT);
  if(!bmp.begin(0x76)) 
  {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }

  // Update on cold boot
  getSensorData();
  serialPrintSensorData();
  publishSensorData();
}

void getSensorData()
{
  temperature = bmp.readTemperature();
  pressure    = bmp.readPressure();

  moisture    = map(analogRead(moisturePin), drySoil, wetSoil, 0, 100);
  light       = map(analogRead(lightPin),    night,   day,     0, 100);

  Serial << "Absolute Moisture: " << analogRead(moisturePin) << endl;

  // moisture = analogRead(moisturePin);
  // light    = analogRead(lightPin);
}

void serialPrintSensorData() // *NB* Add light 
{
	Serial << "Temperature : " << temperature << "*C\tPressure : " << pressure << "Pa\t" << "Soil Moisture: " << moisture << "%\t" << "Light: " << light << "%\t" << endl;
}

