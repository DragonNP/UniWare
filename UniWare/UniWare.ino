/*
  Sketch for the project "UniWare"
  The source code on GitHub: https://github.com/DragonNP/UniWare
  Author: DragonNP, 2020
  https://github.com/DragonNP/

  Libs link:
  ------------------------------------------------------
  ESP8266WiFi       -    https://github.com/esp8266/Arduino
  GyverTimer        -    https://github.com/AlexGyver/GyverLibs/tree/master/GyverTimer
  PubSubClient      -    https://github.com/ianscrivener/pubsubclient
  Wire              -    https://github.com/esp8266/Arduino/tree/master/libraries/Wire
  Adafruit_BME280   -    https://github.com/adafruit/Adafruit_BME280_Library
  Adafruit_Sensor   -    https://github.com/adafruit/Adafruit_Sensor
  ESPAsyncTCP       -    https://github.com/me-no-dev/ESPAsyncTCP
  ESPAsyncWebServer -    https://github.com/me-no-dev/ESPAsyncWebServer
  ------------------------------------------------------
*/


// ============== LIBS ==============
// WiFi
#include <ESP8266WiFi.h>
// JSON
#include <ArduinoJson.h>
// MQTT
#include <GyverTimer.h>
#include <PubSubClient.h>
// BME280
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
// WEB
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Updater.h>
// File System
#include <FS.h>
// ============== LIBS ==============

// VARIABLES
#include "a1_vars.h"
