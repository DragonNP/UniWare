// ============ VARIABLES ===========
String device_name;
// WiFi
String wifi_ssid;
String wifi_psswd;
// AP WiFi
String ap_psswd;
bool   useAP = false;
// MQTT
String mqtt_server;
int    mqtt_port;
String mqtt_user;
String mqtt_pass;
int    mqtt_timeout_publish;
bool   useMQTT = false;
bool   useMQTTAuth = false;
// NTP
String ntp_server;
String ntp_timezone;

// Timers
GTimer timerConnect(MS);
GTimer timerPublish(MS);
GTimer timerReader(MS);
// ============ VARIABLES ===========
