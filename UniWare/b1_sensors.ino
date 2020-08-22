// Vars
GTimer sensorTimer;
// BME280 Sensor
Adafruit_BME280 bme;

// Functions
// Button
void buttonInit(JsonObject& sensor);
int buttonRead(JsonObject& sensor);
// Analog
void analogInit(JsonObject& sensor);
int analogSensRead(JsonObject& sensor);

void loadPins() {
  // Debug
  DEBUG_PRINTLN("DBUG: load pins");
  // Vars
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["A0"] = A0;
  root["D0"] = 16;
  root["D1"] = 5;
  root["D2"] = 4;
  root["D3"] = 0;
  root["D4"] = 2;
  root["D5"] = 14;
  root["D6"] = 12;
  root["D7"] = 13;
  root["D8"] = 15;
  root["D9"] = 3;
  root["D10"] = 1;
  root["RX"] = 3;
  root["TX"] = 1;
  root["S3"] = 10;

  // Save
  root.printTo(pins);
}

void loadSensorsTypes() {
  // Debug
  DEBUG_PRINTLN("DBUG: setting sensors type");
  // Vars
  DynamicJsonBuffer jsonBuffer;
  JsonObject& types_json = jsonBuffer.createObject();

  // Button
  String btn_short_name = "button";
  JsonObject& btn_json = jsonBuffer.createObject();
  JsonObject& btn_pins_json = jsonBuffer.createObject();
  JsonObject& btn_pin1_json = jsonBuffer.createObject();
  JsonObject& btn_topics_json = jsonBuffer.createObject();
  JsonObject& btn_topic1_json = jsonBuffer.createObject();
  // Pins
  btn_pin1_json["name"] = "Logic";
  btn_pins_json["logic"] = btn_pin1_json;
  // Topics
  btn_topic1_json["name"] = "Logic";
  btn_topics_json["logic"] = btn_topic1_json;
  // Button name and pins, topics
  btn_json["name"] = "Button (0/1 values)";
  btn_json["pins"] = btn_pins_json;
  btn_json["topics"] = btn_topics_json;

  // Analog
  String analog_short_name = "analog";
  JsonObject& analog_json = jsonBuffer.createObject();
  JsonObject& analog_pins_json = jsonBuffer.createObject();
  JsonObject& analog_pin1_json = jsonBuffer.createObject();
  JsonObject& analog_topics_json = jsonBuffer.createObject();
  JsonObject& analog_topic1_json = jsonBuffer.createObject();
  // Pins
  analog_pin1_json["name"] = "Analog";
  analog_pins_json["analog"] = analog_pin1_json;
  // Topics
  analog_topic1_json["name"] = "Analog";
  analog_topics_json["analog"] = analog_topic1_json;
  // Analog name and pins
  analog_json["name"] = "Analog (from 0 to 1024 values)";
  analog_json["pins"] = analog_pins_json;
  analog_json["topics"] = analog_topics_json;

  // Types
  types_json[btn_short_name] = btn_json;
  types_json[analog_short_name] = analog_json;

  // Save
  types_json.printTo(types);
}

void loadSensors() {
  // Debug
  DEBUG_PRINTLN("DBUG: load sensors");
  // Vars
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sensors);
  // Logic
  for (auto kv : root) {
    if (kv.value["type"] == "button") buttonInit(kv.value);
    else if (kv.value["type"] == "analog") analogInit(kv.value);
  }
  // Timer
  sensorTimer.setInterval(1500);
  sensorTimer.start();
}

void SensorsUpdate() {
  if (!sensorTimer.isReady()) return;
  // Vars
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sensors);
  JsonObject& root_values = jsonBuffer.parseObject(values);
  JsonObject& root_types = jsonBuffer.parseObject(types);

  // Logic
  for (auto kv : root) {
    if (kv.value["type"] == "button") {
      String type = root[kv.key]["type"].as<String>();
      JsonObject& json = jsonBuffer.createObject();
      JsonObject& types_json = jsonBuffer.parseObject(root_types[type]["topics"].as<String>());

      for (auto kv_topic : types_json) {
        int value = buttonRead(kv.value);
        json[kv_topic.key] = String(value);
        root_values[kv.key] = json;
      }
    }
    else if (kv.value["type"] == "analog") {
      String type = kv.value["type"];

      JsonObject& json = jsonBuffer.createObject();
      JsonObject& types_json = jsonBuffer.parseObject(root_types[type]["topics"].as<String>());

      for (auto kv_topic : types_json) {
        int value = analogSensRead(kv.value);
        json[kv_topic.key] = String(value);
        root_values[kv.key] = json;
      }
    }
  }
  // Save
  values = "";
  root_values.printTo(values);
}


void buttonInit(JsonObject& sensor) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pins_json = jsonBuffer.parseObject(pins);

  String pin_name = sensor["pins"]["logic"].as<String>();
  int pin = pins_json[pin_name].as<int>();

  pinMode(pin, INPUT);
}

int buttonRead(JsonObject& sensor) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pins_json = jsonBuffer.parseObject(pins);

  String pin_name = sensor["pins"]["logic"].as<String>();
  int pin = pins_json[pin_name].as<int>();
  bool value = digitalRead(pin);

  return int(value);
}


void analogInit(JsonObject& sensor) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pins_json = jsonBuffer.parseObject(pins);

  String pin_name = sensor["pins"]["analog"].as<String>();
  int pin = pins_json[pin_name].as<int>();
  pinMode(pin, INPUT);
}

int analogSensRead(JsonObject& sensor) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& pins_json = jsonBuffer.parseObject(pins);

  String pin_name = sensor["pins"]["analog"].as<String>();
  int pin = pins_json[pin_name].as<int>();
  int value = analogRead(pin);

  return value;
}
