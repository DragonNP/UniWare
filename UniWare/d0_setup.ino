void setup() {
  Serial.begin(SPEED_SERIAL);
  Serial.println();

  if (!SPIFFS.begin()) {
    Serial.println("ERROR: An Error has occurred while mounting SPIFFS");
    return;
  }

  setSensorsTypes();
  loadSettings();
  WebStart();
  WiFiSetup();
}
