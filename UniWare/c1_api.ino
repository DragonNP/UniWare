void useAPI() {
  DEBUG_PRINTLN("DBUG: Using API");

  useAPIget();
  useAPIpost();
}

void useAPIget() {
  // Device
  HttpServer.on("/api/device/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/device/get, method:get");

    String json = "";

    DynamicJsonDocument doc_device(1024);
    doc_device["name"] = device_name;

    serializeJson(doc_device, json);
    request->send(200, "text/plane", json);
  });

  // WiFi
  HttpServer.on("/api/wifi/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/wifi/get, method:get");

    String json = "";

    DynamicJsonDocument doc_wifi(1024);

    if (useAP) doc_wifi["use"] = "AP";
    else doc_wifi["use"] = "WiFi";
    doc_wifi["ssid"] = wifi_ssid;
    doc_wifi["passwd"] = wifi_psswd;
    doc_wifi["passwdAP"] = ap_psswd;

    serializeJson(doc_wifi, json);
    request->send(200, "text/plane", json);
  });

  // Sensors
  HttpServer.on("/api/sensors/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/sensors/get, method:get");

    String json = "";

    if (sensors == "") sensors = "{}";
    if (types == "") types = "{}";

    unsigned int length_sensors = sizeof(sensors);
    unsigned int length_types = sizeof(types);

    DynamicJsonDocument doc_sensors_types((length_sensors * 100) + (length_types * 100));
    DynamicJsonDocument doc_sensors(length_sensors * 100);
    DynamicJsonDocument doc_types(length_types * 100);

    auto error_sensors = deserializeJson(doc_sensors, sensors);
    auto error_types = deserializeJson(doc_types, types);

    if (error_sensors) {
      logFailedParse("/api/sensors/get", sensors, "sensors", error_sensors.c_str(), doc_sensors.capacity());
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    if (error_types) {
      logFailedParse("/api/sensors/get", types, "types", error_types.c_str(), doc_types.capacity());
      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }

    doc_sensors_types["types"] = doc_types;
    doc_sensors_types["sensors"] = doc_sensors;

    serializeJson(doc_sensors_types, json);
    request->send(200, "text/plane", json);
  });

  // MQTT
  HttpServer.on("/api/mqtt/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/mqtt/get, method:get");

    String json = "";

    DynamicJsonDocument doc(1024);

    if (useMQTT) doc["use"] = "true";
    else doc["use"] = "false";
    if (useMQTTAuth) doc["useAuth"] = "true";
    else doc["useAuth"] = "false";
    doc["server"] = mqtt_server;
    doc["port"] = String(mqtt_port);
    doc["user"] = mqtt_user;
    doc["passwd"] = mqtt_pass;
    doc["timeout_publish"] = String(mqtt_timeout_publish);

    serializeJson(doc, json);
    request->send(200, "text/plane", json);
  });
}

void useAPIpost() {
  // WiFi
  HttpServer.on("/api/wifi/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/wifi/set, method:post");

    bool saveFlag = false;
    String body = request->arg("body");

    unsigned int length_body = sizeof(body);
    DynamicJsonDocument doc_body(length_body * 100);
    auto error_body = deserializeJson(doc_body, body);

    if (error_body) {
      logFailedParse("/api/wifi/set", body, "body", error_body.c_str(), doc_body.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }

    JsonObject root_body = doc_body.as<JsonObject>();

    for (JsonPair kv : root_body) {
      String name = kv.key().c_str();
      String value = kv.value().as<char*>();

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

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });

  // Device
  HttpServer.on("/api/device/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/device/set, method:post");

    bool saveFlag = false;
    String body = request->arg("body");

    unsigned int length_body = sizeof(body);
    DynamicJsonDocument doc_body(length_body * 100);
    auto error_body = deserializeJson(doc_body, body);

    if (error_body) {
      logFailedParse("/api/device/set", body, "body", error_body.c_str(), doc_body.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }

    JsonObject root_body = doc_body.as<JsonObject>();

    for (JsonPair kv : root_body) {
      String name = kv.key().c_str();
      String value = kv.value().as<char*>();

      if (name == "name") {
        device_name = value;
        saveFlag = true;
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

    bool saveFlag = false;
    String body = request->arg("body");

    unsigned int length_body = sizeof(body);
    unsigned int length_sensors = sizeof(sensors);

    DynamicJsonDocument doc_body(length_body * 100);
    DynamicJsonDocument doc_sensors(length_sensors * 100);

    auto error_body = deserializeJson(doc_body, body);
    auto error_sensors = deserializeJson(doc_sensors, sensors);

    if (error_body) {
      logFailedParse("/api/sensors/set", body, "body", error_body.c_str(), doc_body.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    if (error_sensors) {
      logFailedParse("/api/sensors/set", sensors, "sensors", error_sensors.c_str(), doc_sensors.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }

    JsonObject root_body = doc_body.as<JsonObject>();

    for (JsonPair kv : root_body) {
      String name = kv.key().c_str();
      String value = kv.value().as<char*>();
      String id = name.substring(0, 5);

      if (name.indexOf("_type") == 5) {
        doc_sensors[id]["type"] = value;

        saveFlag = true;
      }
      else if (name.indexOf("_pin_") == 5) {
        String short_name = name.substring(10);

        doc_sensors[id]["pins"][short_name] = value;

        saveFlag = true;
      }
      else if (name.indexOf("_mqtt_") == 5) {
        String short_name = name.substring(11);
        String key = name.substring(11);

        short_name.replace("_use", "");
        short_name.replace("_topic", "");
        key.replace(short_name, "");
        key.replace("_", "");

        doc_sensors[id]["mqtt"]["topics"][short_name][key] = value;

        saveFlag = true;
      }

      sensors = "";
      serializeJson(doc_sensors, sensors);
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
  HttpServer.on("/api/sensors/delete", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/sensors/delete, method:post");

    bool saveFlag = false;
    String body = request->arg("body");

    unsigned int length_body = sizeof(body);
    unsigned int length_sensors = sizeof(sensors);

    DynamicJsonDocument doc_body(length_body * 100);
    DynamicJsonDocument doc_sensors(length_sensors * 100);

    auto error_body = deserializeJson(doc_body, body);
    auto error_sensors = deserializeJson(doc_sensors, sensors);

    if (error_body) {
      logFailedParse("/api/sensors/delete", body, "body", error_body.c_str(), doc_body.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
    if (error_sensors) {
      logFailedParse("/api/sensors/delete", sensors, "sensors", error_sensors.c_str(), doc_sensors.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }
      
    JsonObject root_body = doc_body.as<JsonObject>();

    for (JsonPair kv : root_body) {
      String name = kv.key().c_str();
      String value = kv.value().as<char*>();

      if (name == "id") {
        doc_sensors.remove(value);
        saveFlag = true;
      }
    }

    sensors = "";
    serializeJson(doc_sensors, sensors);

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });

  // MQTT
  HttpServer.on("/api/mqtt/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    DEBUG_PRINTLN("DBUG: API: url:/api/mqtt/set, method:post");

    bool saveFlag = false;
    String body = request->arg("body");

    unsigned int length_body = sizeof(body);
    DynamicJsonDocument doc_body(length_body * 100);
    auto error_body = deserializeJson(doc_body, body);

    if (error_body) {
      logFailedParse("/api/mqtt/set", body, "body", error_body.c_str(), doc_body.capacity());

      return request->send(503, "text/plane", "{\"status\":\"error\"}");
    }

    JsonObject root_body = doc_body.as<JsonObject>();

    for (JsonPair kv : root_body) {
      String name = kv.key().c_str();
      String value = kv.value().as<char*>();

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

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
}
