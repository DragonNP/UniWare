void useAPI() {
  Serial.println("Using API");
  
  useAPIget();
  useAPIpost();
}

void useAPIget() {
  // GET
  HttpServer.on("/api/device/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    String json = "";

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& device = jsonBuffer.createObject();
    device["name"] = device_name;

    device.printTo(json);
    request->send(200, "text/plane", json);
  });

  HttpServer.on("/api/wifi/get", HTTP_GET, [](AsyncWebServerRequest * request) {
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
}

void useAPIpost() {
  // POST
  HttpServer.on("/api/wifi/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    Serial.println("API: url:/api/wifi/set, method:post");
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

  HttpServer.on("/api/device/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    Serial.println("API: url:/api/device/set, method:post");
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
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
    delay(2000);
    ESP.restart();
  });
}
