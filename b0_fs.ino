bool loadSettings() {
  File configFile = SPIFFS.open("/settings.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  // Device
  device_name = json["device"]["name"].as<String>();
  // WiFi
  if (json["wifi"]["use"].as<String>() == "WiFi") useAP = false;
  else if (json["wifi"]["use"].as<String>() == "AP") useAP = true;
  wifi_ssid = json["wifi"]["ssid"].as<String>();
  wifi_psswd = json["wifi"]["psswd"].as<String>();
  ap_psswd = json["wifi"]["passwdAP"].as<String>();
  // NTP
  ntp_server = json["ntp"]["server"].as<String>();
  ntp_timezone = json["ntp"]["timezone"].as<String>();

  if (device_name == "") device_name = "UniWare_" + (WiFi.macAddress().substring(0, 5));   
  return true;
}

bool saveSettings() {
  DynamicJsonBuffer jsonBuffer;
  
  JsonObject& device = jsonBuffer.createObject();
  device["name"] = device_name;

  JsonObject& wifi = jsonBuffer.createObject();
  if (useAP) wifi["use"] = "AP";
  else wifi["use"] = "WiFi";
  wifi["ssid"] = wifi_ssid;
  wifi["psswd"] = wifi_psswd;
  wifi["psswdAP"] = ap_psswd;
  
  JsonObject& ntp = jsonBuffer.createObject();
  ntp["server"] = ntp_server;
  ntp["timezone"] = ntp_timezone;

  JsonObject& json = jsonBuffer.createObject();
  json["device"] = device;
  json["wifi"] = wifi;
  json["ntp"] = ntp;

  File configFile = SPIFFS.open("/settings.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
