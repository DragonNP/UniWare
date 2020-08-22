void loadSettings() {
  DEBUG_PRINTLN("DBUG: opening /settings.json");
  File configFile = SPIFFS.open("/settings.json", "r");
  if (!configFile) {
    Serial.println("FAIL: Failed to open config file");
    return;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("FAIL: Config file size is too large");
    return;
  }

  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("FAIL: Failed to parse config file");

    device_name = "UniWare_" + WiFi.macAddress().substring(0, 3) + WiFi.macAddress().substring(15);
    sensors = "{}";

    return;
  }

  DEBUG_PRINTLN("DBUG: loads variables");
  // Device
  device_name = json["device"]["name"].as<String>();
  // WiFi
  if (json["wifi"]["use"].as<String>() == "WiFi") useAP = false;
  else useAP = true;
  wifi_ssid = json["wifi"]["ssid"].as<String>();
  wifi_psswd = json["wifi"]["psswd"].as<String>();
  ap_psswd = json["wifi"]["passwdAP"].as<String>();
  // Sensors
  sensors = json["sensors"].as<String>();
  // MQTT
  if (json["mqtt"]["use"].as<String>() == "true") useMQTT = true;
  else useAP = false;
  if (json["mqtt"]["useAuth"].as<String>() == "true") useMQTTAuth = true;
  else useMQTTAuth = false;
  mqtt_server = json["mqtt"]["server"].as<String>();
  mqtt_port = json["mqtt"]["port"].as<int>();
  mqtt_user = json["mqtt"]["user"].as<String>();
  mqtt_pass = json["mqtt"]["passwd"].as<String>();
  mqtt_timeout_publish = json["mqtt"]["timeout_publish"].as<int>();

  if (device_name == "") device_name = "UniWare_" + WiFi.macAddress().substring(0, 3) + WiFi.macAddress().substring(15);
  if (sensors == "") sensors = "{}";
}

void saveSettings() {
  DEBUG_PRINTLN("DBUG: saving settings to /settings.json");

  DynamicJsonBuffer jsonBuffer;

  JsonObject& device = jsonBuffer.createObject();
  device["name"] = device_name;

  JsonObject& wifi = jsonBuffer.createObject();
  if (useAP) wifi["use"] = "AP";
  else wifi["use"] = "WiFi";
  wifi["ssid"] = wifi_ssid;
  wifi["psswd"] = wifi_psswd;
  wifi["psswdAP"] = ap_psswd;
  
  JsonObject& mqtt = jsonBuffer.createObject();
  if (useMQTT) mqtt["use"] = "true";
  else mqtt["use"] = "false";
  if (useMQTTAuth) mqtt["useAuth"] = "true";
  else mqtt["useAuth"] = "false";
  mqtt["server"] = mqtt_server;
  mqtt["port"] = String(mqtt_port);
  mqtt["user"] = mqtt_user;
  mqtt["passwd"] = mqtt_pass;
  mqtt["timeout_publish"] = String(mqtt_timeout_publish);

  JsonObject& sensors_json = jsonBuffer.parseObject(sensors);

  JsonObject& json = jsonBuffer.createObject();
  json["device"] = device;
  json["wifi"] = wifi;
  json["sensors"] = sensors_json;
  json["mqtt"] = mqtt;

  File configFile = SPIFFS.open("/settings.json", "w");
  if (!configFile) {
    Serial.println("FAIL: Failed to open config file for writing");
    return;
  }
  
  json.printTo(configFile);
}
