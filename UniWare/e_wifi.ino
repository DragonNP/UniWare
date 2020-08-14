void WiFiSetup() {
  bool useCounter = true;
  
 if (wifi_ssid == "" || useAP) {
    // Log
    Serial.println("LOG: Using AP");
    
    // WiFi Setting
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.softAP(device_name, ap_psswd);
    useAP = true;
    
    // Log
    Serial.print("LOG: AP Name: ");
    Serial.println(device_name);
    Serial.print("LOG: Password: ");
    Serial.println(ap_psswd);
    Serial.print("LOG: Soft IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
  else {
    // Log
    Serial.print("LOG: Connecting to WiFi");
    
    // WiFi Setting
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_psswd);
    useAP = false;

    // Wating for connection
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (count == 25) {
        useAP = true;
        useCounter = false;
        count++;
        WiFiSetup();
      }
      
      delay(500);
      if (useCounter) Serial.print(".");
      if (useCounter) count++;
    }
    Serial.println(" Connected");
    
    // Log
    Serial.print("SSID: ");
    Serial.println(wifi_ssid);
    Serial.print("Local IP Address: ");
    Serial.println(WiFi.localIP());
  }
} 
