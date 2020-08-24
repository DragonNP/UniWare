// Vars
GTimer sensorTimer;

// Functions
// Button
void buttonInit(DynamicJsonDocument sensor);
int buttonRead(DynamicJsonDocument sensor);
// Analog
void analogInit(DynamicJsonDocument sensor);
int analogSensRead(DynamicJsonDocument sensor);
// Analog
void bme280Init(DynamicJsonDocument sensor);
float bme280Read(DynamicJsonDocument sensor, String type_value);

void loadPins() {
  // Debug
  DEBUG_PRINTLN("DBUG: Load pins");

  // Vars
  DynamicJsonDocument doc(1024);

  doc["A0"] = A0;
  doc["D0"] = 16;
  doc["D1"] = 5;
  doc["D2"] = 4;
  doc["D3"] = 0;
  doc["D4"] = 2;
  doc["D5"] = 14;
  doc["D6"] = 12;
  doc["D7"] = 13;
  doc["D8"] = 15;
  doc["D9"] = 3;
  doc["D10"] = 1;
  doc["RX"] = 3;
  doc["TX"] = 1;
  doc["S3"] = 10;
  doc["0x76"] = 0x76;
  doc["0x77"] = 0x77;

  // Save
  serializeJson(doc, pins);
}

void loadSensorsTypes() {
  // Debug
  DEBUG_PRINTLN("DBUG: Setting sensors type");

  // Vars
  DynamicJsonDocument doc(1024);

  // Button
  String btn_short_name = "button";
  // Pin
  doc[btn_short_name]["pins"]["logic"]["name"] = "Logic";
  // Topic
  doc[btn_short_name]["topics"]["logic"]["name"] = "Logic";
  // Name
  doc[btn_short_name]["name"] = "Button (0/1 values)";

  // Analog
  String analog_short_name = "analog";
  // Pin
  doc[analog_short_name]["pins"]["analog"]["name"] = "Analog";
  // Topic
  doc[analog_short_name]["topics"]["analog"]["name"] = "Analog";
  // Name
  doc[analog_short_name]["name"] = "Analog (from 0 to 1024 values)";

  // BME280
  String bme280_short_name = "bme280";
  // Pin
  doc[bme280_short_name]["pins"]["address"]["name"] = "I2C Address";
  // Topics
  doc[bme280_short_name]["topics"]["temperature"]["name"] = "Temperature";
  doc[bme280_short_name]["topics"]["humidity"]["name"] = "Humidity";
  doc[bme280_short_name]["topics"]["pressure"]["name"] = "Pressure";
  doc[bme280_short_name]["topics"]["altitude"]["name"] = "Altitude";
  // Name
  doc[bme280_short_name]["name"] = "BME280 (temp., hum., press. and altitude)";

  // Save
  serializeJson(doc, types);
}

void loadSensors() {
  // Debug
  DEBUG_PRINTLN("DBUG: Load sensors");

  // Vars
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc, sensors);
  JsonObject root = doc.as<JsonObject>();

  // Logic
  if (error) {
    Serial.print("FAIL: Failed to parse sensors: ");
    Serial.print(error.c_str());
    Serial.print(", capacity: ");
    Serial.print(doc.capacity());
    Serial.print(", sensors: ");
    Serial.println(sensors);

    return;
  }

  for (JsonPair kv : root) {
    DynamicJsonDocument value = kv.value();

    if (value["type"] == "button") buttonInit(value);
    else if (value["type"] == "analog") analogInit(value);
    else if (value["type"] == "bme280") bme280Init(value);
  }

  // Timer
  sensorTimer.setInterval(1500);
  sensorTimer.start();
}

void SensorsUpdate() {
  if (!sensorTimer.isReady()) return;

  unsigned int length_sensors = sizeof(sensors);
  unsigned int length_values = sizeof(values);
  unsigned int length_types = sizeof(types);

  DynamicJsonDocument doc_sensors(length_sensors + 500);
  DynamicJsonDocument doc_values(length_values + 500);
  DynamicJsonDocument doc_types(length_types + 500);

  deserializeJson(doc_sensors, sensors);
  deserializeJson(doc_values, values);
  deserializeJson(doc_types, types);

  JsonObject root_sensors = doc_sensors.as<JsonObject>();

  // Logic
  for (JsonPair kv_sensors : root_sensors) {
    DynamicJsonDocument value_sensors = kv_sensors.value();
    String key_sensors = kv_sensors.key().c_str();

    if (value_sensors["type"] == "button") {
      String type = value_sensors["type"];

      DynamicJsonDocument json(1024);
      DynamicJsonDocument doc_topics = doc_types[type]["topics"];

      JsonObject root_topics = doc_topics.as<JsonObject>();

      for (JsonPair topic_kv : root_topics) {
        String topic_key = topic_kv.key().c_str();
        int read_value = buttonRead(value_sensors);

        json[topic_key] = String(read_value);
        doc_values[topic_key] = json;
      }
    }
    else if (value_sensors["type"] == "analog") {
      String type = value_sensors["type"];

      DynamicJsonDocument json(1024);
      DynamicJsonDocument doc_topics = doc_types[type]["topics"];

      JsonObject root_topics = doc_topics.as<JsonObject>();

      for (JsonPair topic_kv : root_topics) {
        DynamicJsonDocument topic_value = topic_kv.value();
        String topic_key = topic_kv.key().c_str();

        int read_value = analogSensRead(topic_value);

        json[topic_key] = String(read_value);
        doc_values[key_sensors] = json;
      }
    }
    else if (value_sensors["type"] == "bme280") {
      String type = value_sensors["type"];

      DynamicJsonDocument json(1024);
      DynamicJsonDocument doc_topics = doc_types[type]["topics"];

      JsonObject root_topics = doc_topics.as<JsonObject>();

      for (JsonPair topic_kv : root_topics) {
        DynamicJsonDocument topic_value = topic_kv.value();
        String topic_key = topic_kv.key().c_str();

        float read_value = bme280Read(topic_value, topic_key);

        json[topic_key] = String(read_value);
        doc_values[topic_key] = json;
      }
    }
  }

  // Save
  values = "";
  serializeJson(doc_values, values);
}


void buttonInit(DynamicJsonDocument sensor) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  String pin_name = sensor["pins"]["logic"].as<String>();
  int pin = doc_pins[pin_name].as<int>();

  pinMode(pin, INPUT);
}

int buttonRead(DynamicJsonDocument sensor) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  String pin_name = sensor["pins"]["logic"].as<String>();
  int pin = doc_pins[pin_name].as<int>();
  bool value = digitalRead(pin);

  return int(value);
}


void analogInit(DynamicJsonDocument sensor) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  String pin_name = sensor["pins"]["analog"].as<String>();
  int pin = doc_pins[pin_name].as<int>();
  pinMode(pin, INPUT);
}

int analogSensRead(DynamicJsonDocument sensor) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  String pin_name = sensor["pins"]["analog"].as<String>();
  int pin = doc_pins[pin_name].as<int>();
  int value = analogRead(pin);

  return value;
}


void bme280Init(DynamicJsonDocument sensor) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  Adafruit_BME280 bme;

  String pin_name = sensor["pins"]["i2c"].as<String>();
  int pin = doc_pins[pin_name].as<int>();

  bme.begin(pin);
}

float bme280Read(DynamicJsonDocument sensor, String type_value) {
  unsigned int length_pins = sizeof(pins);
  DynamicJsonDocument doc_pins(length_pins + 200);
  deserializeJson(doc_pins, pins);

  Adafruit_BME280 bme;

  String pin_name = sensor["pins"]["address"].as<String>();
  int pin = doc_pins[pin_name].as<int>();
  float value;

  bme.begin(pin);
  if (type_value == "temperature") value = bme.readTemperature();
  if (type_value == "humidity") value = bme.readHumidity();
  if (type_value == "pressure") value = bme.readPressure() / 100.0F * HPA_TO_MMHG;
  if (type_value == "altitude") value = bme.readAltitude(SEALEVELPRESSURE_HPA);

  return value;
}
