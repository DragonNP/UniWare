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
    DEBUG_PRINTLN("DBUG: Sending values to mqtt broker");

    client.loop();

    MQTTSend();
  }
}

void MQTTSend() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sensors);

  for (auto kv_sensor : root) {
    JsonObject& topics = jsonBuffer.parseObject(root[kv_sensor.key]["mqtt"].as<String>());

    for (auto kv : topics) {
      String type;
      String topic;
      bool use = false;

      JsonObject& vars = jsonBuffer.parseObject(kv.value.as<String>());
      JsonObject& root_values = jsonBuffer.parseObject(values);
      
      for (auto var : vars) {
        type = var.key;
        topic = var.value["topic"].as<String>();
        
        if (var.value["use"].as<String>() == "true") use = true;
      }
      
      if (use && (topic != "" && topic != " ")) {
        String value = root_values[kv_sensor.key][type].as<String>();
        client.publish(topic, String(value));
      }
    }
  }
}
