void useAPI() {
  DEBUG_PRINTLN("INFO: Using API");

  useAPIget();
  useAPIpost();
}

void useAPIget() {
  // Device
  HttpServer.on("/api/device/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/device/get, method:get");

    String json = "";

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& device = jsonBuffer.createObject();
    device["name"] = device_name;

    device.printTo(json);
    request->send(200, "text/plane", json);
  });
  // WiFi
  HttpServer.on("/api/wifi/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/wifi/get, method:get");

    String json = "";

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& wifi = jsonBuffer.createObject();
    if (useAP) wifi["use"] = "AP";
    else wifi["use"] = "WiFi";
    wifi["ssid"] = wifi_ssid;
    wifi["passwd"] = wifi_psswd;
    wifi["passwdAP"] = ap_psswd;

    wifi.printTo(json);
    request->send(200, "text/plane", json);
  });
  // Sensors
  HttpServer.on("/api/sensors/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/sensors/get, method:get");

    String sensors_text = "";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& sensors_types_json = jsonBuffer.createObject();

    JsonObject& types_json = jsonBuffer.parseObject(types);
    JsonObject& sensors_json = jsonBuffer.parseObject(sensors);

    sensors_types_json["types"] = types_json;
    sensors_types_json["sensors"] = sensors_json;

    sensors_types_json.printTo(sensors_text);
    request->send(200, "text/plane", sensors_text);
  });
  // MQTT
  HttpServer.on("/api/mqtt/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/mqtt/get, method:get");

    String json = "";

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    if (useMQTT) root["use"] = "true";
    else root["use"] = "false";
    if (useMQTTAuth) root["useAuth"] = "true";
    else root["useAuth"] = "false";
    root["server"] = mqtt_server;
    root["port"] = String(mqtt_port);
    root["user"] = mqtt_user;
    root["passwd"] = mqtt_pass;
    root["timeout_publish"] = String(mqtt_timeout_publish);

    root.printTo(json);

    request->send(200, "text/plane", json);
  });
}

void useAPIpost() {
  // WiFi
  HttpServer.on("/api/wifi/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/wifi/set, method:post");
    bool saveFlag = false;

    // Parsing body
    DynamicJsonBuffer jsonBuffer;
    JsonArray& json = jsonBuffer.parseArray(request->arg("body"));

    if (!json.success()) {
      Serial.println("Failed to parse body");
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    else {
      for (int i = 0; i < json.size(); i++) {
        String name = json[i]["name"].as<const char*>();
        String value = json[i]["value"].as<const char*>();

        // Search names
        if (name == "use") {
          if (value == "AP") useAP = true;
          else if (value == "WiFi") useAP = false;
          saveFlag = true;
        }
        else if (name == "ssid") {
          wifi_ssid = value;
          saveFlag = true;
        }
        else if (name == "psswd") {
          wifi_psswd = value;
          saveFlag = true;
        }
        else if (name == "psswd_ap") {
          ap_psswd = value;
          saveFlag = true;
        }
      }
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
  // Device
  HttpServer.on("/api/device/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/device/set, method:post");

    DynamicJsonBuffer jsonBuffer;
    JsonArray& json = jsonBuffer.parseArray(request->arg("body"));
    bool saveFlag = false;

    if (!json.success()) {
      Serial.println("Failed to parse body");
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    else {
      for (int i = 0; i < json.size(); i++) {
        String name = json[i]["name"].as<const char*>();
        String value = json[i]["value"].as<const char*>();

        if (name == "name") {
          device_name = value;
          saveFlag = true;
        }
      }
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
  HttpServer.on("/api/device/reboot", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/device/reboot, method:post");

    request->send(200, "text/plane", "{\"status\":\"ok\"}");
    delay(2000);
    ESP.restart();
  });
  // Sensors
  HttpServer.on("/api/sensors/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/sensors/set, method:post");

    DynamicJsonBuffer jsonBuffer;
    JsonArray& json = jsonBuffer.parseArray(request->arg("body"));
    bool saveFlag = false;

    if (!json.success()) {
      Serial.println("Failed to parse body");
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    else {
      JsonObject& sensors_json = jsonBuffer.parseObject(sensors);

      for (int i = 0; i < json.size(); i++) {
        String name = json[i]["name"].as<const char*>();
        String value = json[i]["value"].as<const char*>();
        String id = name.substring(0, 5);

        String sensor = sensors_json[id].as<String>();
        if (sensor == "") sensor = "{}";

        JsonObject& sensor_json = jsonBuffer.parseObject(sensor);

        if (name.indexOf("_type") == 5) {
          sensor_json["type"] = value;
          saveFlag = true;
        }
        else if (name.indexOf("_pin_") == 5) {
          String pins_string = sensor_json["pins"].as<String>();
          if (pins_string == "") pins_string = "{}";
          String short_name = name.substring(10);

          JsonObject& pins = jsonBuffer.parseObject(pins_string);

          pins[short_name] = value;
          sensor_json["pins"] = pins;

          saveFlag = true;
        }
        else if (name.indexOf("_mqtt_") == 5) {
          String mqtt_str = sensor_json["mqtt"].as<String>();
          String topics_str = sensor_json["mqtt"]["topics"].as<String>();
          String current_topic_str;
          String short_name = name.substring(11);
          String use_or_topic = name.substring(11);

          short_name.replace("_use", "");
          short_name.replace("_topic", "");
          use_or_topic.replace(short_name, "");
          use_or_topic.replace("_", "");
          current_topic_str = sensor_json["mqtt"]["topics"][short_name].as<String>();

          if (mqtt_str == "") mqtt_str = "{}";
          if (topics_str == "") topics_str = "{}";
          if (current_topic_str == "") current_topic_str = "{}";
          
          JsonObject& mqtt = jsonBuffer.parseObject(mqtt_str);
          JsonObject& topics = jsonBuffer.parseObject(topics_str);
          JsonObject& current_topic = jsonBuffer.parseObject(current_topic_str);

          current_topic[use_or_topic] = value;
          topics[short_name] = current_topic;
          mqtt["topics"] = topics;
          sensor_json["mqtt"] = mqtt;

          saveFlag = true;
        }

        sensors_json[id] = sensor_json;
      }

      sensors = "";
      sensors_json.printTo(sensors);
      
      sensors_json.printTo(Serial);
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
  HttpServer.on("/api/sensors/delete", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/sensors/delete, method:post");

    DynamicJsonBuffer jsonBuffer;
    JsonArray& json = jsonBuffer.parseArray(request->arg("body"));
    bool saveFlag = false;

    if (!json.success()) {
      Serial.println("Failed to parse body");
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    else {
      JsonObject& sensors_json = jsonBuffer.parseObject(sensors);

      for (int i = 0; i < json.size(); i++) {
        String name = json[i]["name"].as<const char*>();
        String value = json[i]["value"].as<const char*>();

        if (name == "id") {
          sensors_json.remove(value);
          saveFlag = true;
        }
      }

      sensors = "";
      sensors_json.printTo(sensors);
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
  // MQTT
  HttpServer.on("/api/mqtt/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/mqtt/set, method:post");
    bool saveFlag = false;

    // Parsing body
    DynamicJsonBuffer jsonBuffer;
    JsonArray& json = jsonBuffer.parseArray(request->arg("body"));

    if (!json.success()) {
      Serial.println("Failed to parse body");
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    else {
      for (int i = 0; i < json.size(); i++) {
        String name = json[i]["name"].as<const char*>();
        String value = json[i]["value"].as<const char*>();

        // Search names
        if (name == "use") {
          if (value == "true") useMQTT = true;
          else if (value == "false") useMQTT = false;
          saveFlag = true;
        }
        else if (name == "useAuth") {
          if (value == "true") useMQTTAuth = true;
          else if (value == "false") useMQTTAuth = false;
          saveFlag = true;
        }
        else if (name == "server") {
          mqtt_server = value;
          saveFlag = true;
        }
        else if (name == "port") {
          mqtt_port = value.toInt();
          saveFlag = true;
        }
        else if (name == "user") {
          mqtt_user = value;
          saveFlag = true;
        }
        else if (name == "passwd") {
          mqtt_pass = value;
          saveFlag = true;
        }
        else if (name == "timeout_publish") {
          mqtt_timeout_publish = value.toInt();
          saveFlag = true;
        }
      }
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
}
