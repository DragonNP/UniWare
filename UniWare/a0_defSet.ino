// ============ SETTINGS ============
// -- DEBUG --
#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif
// -- BME280 --
#define SEALEVELPRESSURE_HPA (1013.25)
#define HPA_TO_MMHG          0.7500638
//-- Serial --
#define SPEED_SERIAL 115200
// ============ SETTINGS ============
