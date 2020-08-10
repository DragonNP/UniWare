// Server
AsyncWebServer HttpServer(80);

void useCSS() {
  Serial.println("Using CSS");

  HttpServer.serveStatic("css/dashboard.min.css", SPIFFS, "/dashboard.min.css");
  HttpServer.serveStatic("favicons/favicon.png", SPIFFS, "/favicon.png");

  HttpServer.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (useAP) {
      request->send(SPIFFS, "/bootstrap.min.css");
    } else {
      request->redirect("https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css");
    }
  });
}

void useJS() {
  Serial.println("Using JS");

  HttpServer.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (useAP) {
      request->send(SPIFFS, "/bootstrap.min.js");
    } else {
      request->redirect("https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/js/bootstrap.min.js");
    }
  });
  HttpServer.on("/js/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (useAP) {
      request->send(SPIFFS, "/jquery.min.js");
    } else {
      request->redirect("https://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js");
    }
  });
}

void usePages() {
  Serial.println("Using custom pages");

  HttpServer.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->redirect("/device");
  });
  // Device
  HttpServer.on("/device", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/device.html", "text/html");
  });
  // Sensors
  HttpServer.on("/sensors", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/sensors.html", "text/html");
  });
  // WiFi
  HttpServer.on("/wifi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/wifi.html", "text/html");
  });
  // MQTT
  HttpServer.on("/mqtt", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/mqtt.html", "text/html");
  });
  // NTP
  HttpServer.on("/ntp", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/ntp.html", "text/html");
  });
}

void WebStart() {
  useCSS();
  useJS();
  usePages();
  useAPI();

  HttpServer.begin();
  Serial.println("HTTP HttpServer started");
}
