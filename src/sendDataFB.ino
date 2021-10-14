#include "functions.h"
#include "config.h"   // header include, host and token to Firebase, SSID and password to WiFi connection

#include "FirebaseESP8266.h"  // https://github.com/mobizt/Firebase-ESP8266

void SendFirebase (long sample, String path, float value) {
  Serial.println("Sending the data to Firebase DB");
}
