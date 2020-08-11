void setup() {
  Serial.begin(SPEED_SERIAL);
  Serial.println();

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  setSensorsTypes();
  loadSettings();
  WiFiSetup();
  WebStart();
}
