#include "functions.h"
#include "config.h"   // header include, host and token to Firebase, SSID and password to WiFi connection

#include "FirebaseESP8266.h"  // https://github.com/mobizt/Firebase-ESP8266

void SendFirebase (long sample, String path, float value) {
  Serial.println("Sending the data to Firebase DB");
}

String readDate () {
  char buff[21];
  snprintf_P(buff, sizeof(buff), PSTR("20%02d-%02i-%02iT%02d:%02i:%02iZ"), RTC.getYear(), RTC.getMonth(century), RTC.getDate(), RTC.getHour(h12Flag, pmFlag), RTC.getMinute(), RTC.getSecond());
  String UTC = String(buff);
  return UTC;
}

String longMonth (int month) {
  const char* months[12] = { "january",  "february", "march",  "abril",  "may",  "june",  "july",
                             "august",  "october",  "november",  "december" };
  if (month < 1 || month > 12) return "Month not found!";
  return months[month-1];
}
