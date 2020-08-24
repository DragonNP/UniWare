GTimer mqttPublishTimer;
GTimer mqttConnectTimer;
// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void MQTTSetup() {
  if (!useMQTT) return;

  mqttConnectTimer.setInterval(1000);
  mqttConnectTimer.start();

  if (mqtt_timeout_publish >= 500) mqttPublishTimer.setInterval(mqtt_timeout_publish);
  else Serial.println("FAIL: mqtt timeout publish < 500ms");

  DEBUG_PRINTLN("DBUG: Connecting to MQTT Broker");

  MQTTConnect();
  MQTTUpdate();
}

bool MQTTConnect() {
  bool result = false;
  client.set_server(mqtt_server, mqtt_port);

  if (useMQTTAuth) result = client.connect(MQTT::Connect(device_name).set_auth(mqtt_user, mqtt_pass));
  else result = client.connect(device_name);

  return result;
}

void MQTTUpdate() {
  if (!client.connected() && mqttConnectTimer.isReady()) {
    bool result = MQTTConnect();
    if (!result) return;

    Serial.println("INFO: Connected to MQTT Broker");
    mqttConnectTimer.stop();
  }
  else if (!client.connected() && !mqttConnectTimer.isEnabled()) {
    mqttConnectTimer.setInterval(1000);
    mqttConnectTimer.start();

    MQTTConnect();
  }
  else if (client.connected() && mqttPublishTimer.isReady()) {
    client.loop();

    MQTTSend();
  }
}

void MQTTSend() {
  DEBUG_PRINTLN("DBUG: Sending values to mqtt broker");

  unsigned int length_sensors = sizeof(sensors);

  DynamicJsonDocument doc_sensors(length_sensors + 500);
  deserializeJson(doc_sensors, sensors);

  JsonObject root_sensors = doc_sensors.as<JsonObject>();

  for (JsonPair kv_sensor : root_sensors) {
    DynamicJsonDocument value_sensor = kv_sensor.value();
    String key_sensor = kv_sensor.key().c_str();

    DynamicJsonDocument doc_topics = value_sensor["mqtt"];
    JsonObject root_topics = doc_topics.as<JsonObject>();

    for (JsonPair kv_topics : root_topics) {
      unsigned int length_values = sizeof(values);
      DynamicJsonDocument doc_vars = kv_topics.value();
      DynamicJsonDocument doc_values(length_values + 500);

      deserializeJson(doc_values, values);

      JsonObject root_vars = doc_vars.as<JsonObject>();

      for (JsonPair var : root_vars) {
        String type = var.key().c_str();
        String topic = var.value()["topic"].as<String>();
        bool use = false;

        if (var.value()["use"].as<String>() == "true") use = true;

        if (use && (topic != "" && topic != " ")) {
          String key = kv_sensor.key().c_str();

          String value = doc_values[key][type].as<String>();

          client.publish(topic, value);
        }
      }
    }
  }
}
