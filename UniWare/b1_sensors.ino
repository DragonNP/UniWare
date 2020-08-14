// BME280 Sensor
Adafruit_BME280 bme;

void setSensorsTypes() {
  DEBUG_PRINTLN("DBUG: setting sensors type");

  DynamicJsonBuffer jsonBuffer;
  JsonObject& types_json = jsonBuffer.createObject();

  // Button
  String btn_short_name = "button";
  JsonObject& btn_json = jsonBuffer.createObject();
  JsonObject& btn_pins_json = jsonBuffer.createObject();
  JsonObject& btn_pin1_json = jsonBuffer.createObject();
  // Pins
  btn_pin1_json["name"] = "Logic";
  btn_pins_json["logic"] = btn_pin1_json;
  // Button name and pins
  btn_json["name"] = "Button (0/1 values)";
  btn_json["pins"] = btn_pins_json;

  // Analog
  String analog_short_name = "analog_input";
  JsonObject& analog_json = jsonBuffer.createObject();
  JsonObject& analog_pins_json = jsonBuffer.createObject();
  JsonObject& analog_pin1_json = jsonBuffer.createObject();
  // Pins
  analog_pin1_json["name"] = "Analog";
  analog_pins_json["analog"] = analog_pin1_json;
  // Analog name and pins
  analog_json["name"] = "Analog (from 0 to 255 values)";
  analog_json["pins"] = analog_pins_json;

  // Types
  types_json[btn_short_name] = btn_json;
  types_json[analog_short_name] = analog_json;
  
  // Save
  types_json.printTo(types);
}
