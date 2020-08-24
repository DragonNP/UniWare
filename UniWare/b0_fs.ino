void loadSettings() {
  DEBUG_PRINTLN("DBUG: Opening /settings.json");
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

  DynamicJsonDocument doc(size+500);
  auto error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.print("FAIL: Failed to parse config file: ");
    Serial.println(error.c_str());

    device_name = "UniWare_" + WiFi.macAddress().substring(0, 3) + WiFi.macAddress().substring(15);
    sensors = "{}";

    return;
  }

  DEBUG_PRINTLN("DBUG: Loads variables");

  // Device
  device_name = doc["device"]["name"].as<String>();

  // WiFi
  if (doc["wifi"]["use"].as<String>() == "WiFi") useAP = false;
  else useAP = true;
  wifi_ssid = doc["wifi"]["ssid"].as<String>();
  wifi_psswd = doc["wifi"]["psswd"].as<String>();
  ap_psswd = doc["wifi"]["passwdAP"].as<String>();

  // Sensors
  sensors = doc["sensors"].as<String>();

  // MQTT
  if (doc["mqtt"]["use"].as<String>() == "true") useMQTT = true;
  else useAP = false;
  if (doc["mqtt"]["useAuth"].as<String>() == "true") useMQTTAuth = true;
  else useMQTTAuth = false;
  mqtt_server = doc["mqtt"]["server"].as<String>();
  mqtt_port = doc["mqtt"]["port"].as<int>();
  mqtt_user = doc["mqtt"]["user"].as<String>();
  mqtt_pass = doc["mqtt"]["passwd"].as<String>();
  mqtt_timeout_publish = doc["mqtt"]["timeout_publish"].as<int>();

  // Load defaults
  if ((device_name == "null") || (device_name == "")) device_name = "UniWare_" + WiFi.macAddress().substring(0, 3) + WiFi.macAddress().substring(15);
  if ((sensors == "null") || (sensors == "")) sensors = "{}";
  if (ap_psswd.length() < 8) ap_psswd = "";
}

void saveSettings() {
  DEBUG_PRINTLN("DBUG: Saving settings to /settings.json");

  DynamicJsonDocument doc_root(2048);

  // Device
  unsigned int length_device = sizeof(device_name);
  DynamicJsonDocument doc_device(length_device + 200);
  doc_device["name"] = device_name;

  // WiFi
  DynamicJsonDocument doc_wifi(2048);
  if (useAP) doc_wifi["use"] = "AP";
  else doc_wifi["use"] = "WiFi";
  doc_wifi["ssid"] = wifi_ssid;
  doc_wifi["psswd"] = wifi_psswd;
  doc_wifi["psswdAP"] = ap_psswd;

  // MQTT
  DynamicJsonDocument doc_mqtt(2048);
  if (useMQTT) doc_mqtt["use"] = "true";
  else doc_mqtt["use"] = "false";
  if (useMQTTAuth) doc_mqtt["useAuth"] = "true";
  else doc_mqtt["useAuth"] = "false";
  doc_mqtt["server"] = mqtt_server;
  doc_mqtt["port"] = String(mqtt_port);
  doc_mqtt["user"] = mqtt_user;
  doc_mqtt["passwd"] = mqtt_pass;
  doc_mqtt["timeout_publish"] = String(mqtt_timeout_publish);

  // Sensors
  unsigned int length_sensors = sizeof(device_name);
  DynamicJsonDocument doc_sensors(length_sensors + 200);
  deserializeJson(doc_sensors, sensors);

  // Save other docs to doc root
  doc_root["device"] = doc_device;
  doc_root["wifi"] = doc_wifi;
  doc_root["sensors"] = doc_sensors;
  doc_root["mqtt"] = doc_mqtt;

  File configFile = SPIFFS.open("/settings.json", "w");
  if (!configFile) {
    Serial.println("FAIL: Failed to open config file for writing");
    return;
  }

  // Saving doc root to spiffs
  serializeJson(doc_root, configFile);
}
