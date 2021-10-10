#include "arduino.h"

#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial
#include <ESP8266WiFi.h>
#include <OneWire.h>        // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>    // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>

void readWind () {
  Serial.println("Not implemented");
}
