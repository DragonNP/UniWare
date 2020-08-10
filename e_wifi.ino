void WiFiSetup() {
 if (wifi_ssid == "" || wifi_psswd == "" || useAP) {
    // Log
    Serial.println("Using AP");
    
    // WiFi Setting
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.softAP(device_name, ap_psswd);
    
    // Log
    Serial.print("AP Name: ");
    Serial.println(device_name);
    Serial.print("Password: ");
    Serial.println(ap_psswd);
    Serial.print("Soft IP Address: ");
    Serial.println(WiFi.softAPIP());
  }
  else {
    // Log
    Serial.print("Connecting to WiFi");
    // WiFi Setting
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_psswd);

    // Wating for connection
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (count >= 120) {
        Serial.println(" password or SSID is uncorrect, reset");
        delay(1000);
        useAP = true;
        saveSettings();
        ESP.reset();
      }
      
      delay(500);
      Serial.print(".");
      count++;
    }
    Serial.println(" Connected");
    
    // Log
    Serial.print("SSID: ");
    Serial.println(wifi_ssid);
    Serial.print("Local IP Address: ");
    Serial.println(WiFi.localIP());
  }
} 
