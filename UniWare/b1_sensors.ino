// BME280 Sensor
Adafruit_BME280 bme;


void setSensorsTypes() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& types_json = jsonBuffer.createObject();

    String btn_short_name = "button";
    JsonObject& btn_json = jsonBuffer.createObject();
    JsonObject& btn_pins_json = jsonBuffer.createObject();
    JsonObject& btn_pin1_json = jsonBuffer.createObject();
    
    btn_pin1_json["name"] = "Logic";
    btn_pins_json["logic"] = btn_pin1_json;
    btn_json["name"] = "Button (0/1 values)";
    btn_json["pins"] = btn_pins_json;

    String analog_short_name = "analog_input";
    JsonObject& analog_json = jsonBuffer.createObject();
    JsonObject& analog_pins_json = jsonBuffer.createObject();
    JsonObject& analog_pin1_json = jsonBuffer.createObject();
    
    analog_pin1_json["name"] = "Analog";
    analog_pins_json["analog"] = analog_pin1_json;
    analog_json["name"] = "Analog (from 0 to 255 values)";
    analog_json["pins"] = analog_pins_json;

    types_json[btn_short_name] = btn_json;
    types_json[analog_short_name] = analog_json;

    types_json.printTo(types);
}
