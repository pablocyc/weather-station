#include "Arduino.h"
#include "config.h"   // header include, host and token to Firebase, SSID and password to WiFi connection

#include "FirebaseESP8266.h"  // https://github.com/mobizt/Firebase-ESP8266

#define FIREBASE_HOST HostFirebase 
#define FIREBASE_AUTH TokenFirebase

String pathHeaderFirebase = "/station-home/";
String pathTemp = "temperature/";
String pathHum = "humidity/";
String pathPress = "pressure/";
String pathSpeed = "wind_speed/";
String pathDirection = "wind_direction/";

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseData ledData;

FirebaseJson json;

void sendFirebase (long sample, String path, float value) {
  int month = RTC.getMonth(century);
  if (month != monthOld) {
    monthOld = month;
    EEPROM.put(EEmonth, monthOld);
    st, sh, sp, ss, sd = 0;
    EEPROM.put(EEst, st);
    EEPROM.put(EEsh, sh);
    EEPROM.put(EEsp, sp);
    EEPROM.put(EEss, ss);
    EEPROM.put(EEsd, sd);
    bool ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
  }
  String pathHeader = pathHeaderFirebase + longMonth(month) + "/" + path + sample + "/";
  String date = readDate();
  String pathDate = pathHeader + "date";
  String pathValue = pathHeader + "value";

  if (Firebase.setString(firebaseData, pathDate, date)) {
    Serial.println("PATH_DATE: " + firebaseData.dataPath());
  }
  if (Firebase.setFloat(firebaseData, pathValue, value)) {
    Serial.println("PATH_TEMP: " + firebaseData.dataPath());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

String readDate () {
  String date = "20" + String(RTC.getYear()) + "-" + isOneDigit(RTC.getMonth(century)) + "-" + isOneDigit(RTC.getDate());
  String time = isOneDigit(RTC.getHour(h12Flag, pmFlag)) + ":" + isOneDigit(RTC.getMinute()) + ":" + isOneDigit(RTC.getSecond());
  String UTC = date + "T" + time + "Z";
  return UTC;
}

String longMonth (int month) {
  switch (month) {
    case 1:
      return "january";
    case 2:
      return "february";
    case 3:
      return "march";
    case 4:
      return "abril";
    case 5:
      return "may";
    case 6:
      return "june";
    case 7:
      return "july";
    case 8:
      return "august";
    case 9:
      return "september";
    case 10:
      return "october";
    case 11:
      return "november";
    case 12:
      return "december";
  }
}

String isOneDigit(int digit) {
  String value = String(digit);
  if(digit < 10)
    value = "0" + value;

  return value;
}
