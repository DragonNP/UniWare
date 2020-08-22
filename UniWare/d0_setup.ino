void setup() {
  Serial.begin(SPEED_SERIAL);
  Serial.println();

  if (!SPIFFS.begin()) {
    Serial.println("ERROR: An Error has occurred while mounting SPIFFS");
    return;
  }

  loadPins();
  loadSettings();
  loadSensorsTypes();
  loadSensors();
  MQTTStart();
  WebStart();
  WiFiSetup();
}
