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

  HttpServer.on("/api/ntp/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    String json = "";
    String default_timezones = "{\"0\":{\"name\":\"UTC+0:00\",\"isSelected\":\"false\"},\"60\":{\"name\":\"UTC+1:00\",\"isSelected\":\"false\"},\"120\":{\"name\":\"UTC+2:00\",\"isSelected\":\"false\"},\"180\":{\"name\":\"UTC+3:00\",\"isSelected\":\"false\"},\"210\":{\"name\":\"UTC+4:30\",\"isSelected\":\"false\"},\"240\":{\"name\":\"UTC+4:00\",\"isSelected\":\"false\"},\"300\":{\"name\":\"UTC+5:00\",\"isSelected\":\"false\"},\"330\":{\"name\":\"UTC+5:30\",\"isSelected\":\"false\"},\"345\":{\"name\":\"UTC+5:45\",\"isSelected\":\"false\"},\"360\":{\"name\":\"UTC+6:00\",\"isSelected\":\"false\"},\"390\":{\"name\":\"UTC+6:30\",\"isSelected\":\"false\"},\"420\":{\"name\":\"UTC+7:00\",\"isSelected\":\"false\"},\"480\":{\"name\":\"UTC+8:00\",\"isSelected\":\"false\"},\"525\":{\"name\":\"UTC+8:45\",\"isSelected\":\"false\"},\"540\":{\"name\":\"UTC+9:00\",\"isSelected\":\"false\"},\"570\":{\"name\":\"UTC+9:30\",\"isSelected\":\"false\"},\"600\":{\"name\":\"UTC+10:00\",\"isSelected\":\"false\"},\"630\":{\"name\":\"UTC+10:30\",\"isSelected\":\"false\"},\"660\":{\"name\":\"UTC+11:00\",\"isSelected\":\"false\"},\"720\":{\"name\":\"UTC+12:00\",\"isSelected\":\"false\"},\"765\":{\"name\":\"UTC+12:45\",\"isSelected\":\"false\"},\"780\":{\"name\":\"UTC+13:00\",\"isSelected\":\"false\"},\"840\":{\"name\":\"UTC+14:00\",\"isSelected\":\"false\"},\"-60\":{\"name\":\"UTC-1:00\",\"isSelected\":\"false\"},\"-120\":{\"name\":\"UTC-2:00\",\"isSelected\":\"false\"},\"-150\":{\"name\":\"UTC-2:30\",\"isSelected\":\"false\"},\"-180\":{\"name\":\"UTC-3:00\",\"isSelected\":\"false\"},\"-240\":{\"name\":\"UTC-4:00\",\"isSelected\":\"false\"},\"-300\":{\"name\":\"UTC-5:00\",\"isSelected\":\"false\"},\"-360\":{\"name\":\"UTC-6:00\",\"isSelected\":\"false\"},\"-420\":{\"name\":\"UTC-7:00\",\"isSelected\":\"false\"},\"-480\":{\"name\":\"UTC-8:00\",\"isSelected\":\"false\"},\"-540\":{\"name\":\"UTC-9:00\",\"isSelected\":\"false\"},\"-570\":{\"name\":\"UTC-9:30\",\"isSelected\":\"false\"},\"-600\":{\"name\":\"UTC-10:00\",\"isSelected\":\"false\"},\"-660\":{\"name\":\"UTC-11:00\",\"isSelected\":\"false\"},\"-720\":{\"name\":\"UTC-12:00\",\"isSelected\":\"false\"}}";
    
    DynamicJsonBuffer jsonBuffer;
    JsonObject& ntp = jsonBuffer.createObject();
    JsonObject& timezones = jsonBuffer.parseObject(default_timezones);

    timezones[ntp_timezone]["isSelected"] = "true";
    ntp["server"] = ntp_server;
    ntp["timezones"] = timezones;

    ntp.printTo(json);
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

  HttpServer.on("/api/ntp/set", HTTP_POST, [](AsyncWebServerRequest * request) {
    Serial.println("API: url:/api/ntp/set, method:post");
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

        if (name == "server") {
          ntp_server = value;
          saveFlag = true;
        }
        else if (name == "timezone") {
          ntp_timezone = value;
          saveFlag = true;
        }
      }
    }

    if (saveFlag) saveSettings();
    request->send(200, "text/plane", "{\"status\":\"ok\"}");
  });
}
