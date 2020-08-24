GTimer wifiConnectTimer;
bool isConnecting = false;
bool isAPStarted = false;
bool isFirstLog = true;

void WiFiSetup() {
  wifiConnectTimer.setInterval(1000);
  if (!useAP) wifiConnectTimer.start();

  wifiTraining();
  enableAP();
}

void WiFiUpdate() {
  if (!wifiConnectTimer.isReady()) return;

  bool isWifiConnected = getWiFiStatus();
  if (isWifiConnected && isFirstLog) {
    // WiFi
    disableAP();
    // Vars
    isFirstLog = false;
    useAP = false;
    // Logs
    Serial.println("INFO: Connected to WiFi");
    Serial.print("INFO: SSID: ");
    Serial.println(wifi_ssid);
    Serial.print("INFO: Local IP Address: ");
    Serial.println(WiFi.localIP());
  }
  else if (!isWifiConnected && !isAPStarted) enableAP();
  else if (!isWifiConnected && !isConnecting && !useAP) connectToWiFi();
}

void wifiTraining() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
}

// AP
void enableAP() {
  // Debug
  DEBUG_PRINTLN("DBUG: Starting AP");
  // WiFi
  WiFi.softAP(device_name, ap_psswd);
  // Vars
  isAPStarted = true;
  // Logs
  Serial.print("INFO: AP Name: ");
  Serial.println(device_name);
  Serial.print("INFO: Password: ");
  Serial.println(ap_psswd);
  Serial.print("INFO: Soft IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void disableAP() {
  // Debug
  DEBUG_PRINTLN("DBUG: Disabling AP mode");
  // WiFi
  WiFi.softAPdisconnect(true);
  // Var
  isAPStarted = false;
}

// WiFi
void connectToWiFi() {
  // Log
  DEBUG_PRINTLN("DBUG: Connecting to WiFi");

  // WiFi Setting
  WiFi.begin(wifi_ssid, wifi_psswd);

  isConnecting = true;
}

bool getWiFiStatus() {
  return WiFi.status() == WL_CONNECTED;
}
