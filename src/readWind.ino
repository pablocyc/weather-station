#include "functions.h"

#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial
#include <ESP8266WiFi.h>
#include <OneWire.h>        // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>    // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>

void ReadWind () {
  Serial.println("Not implemented");
}

byte * readSoftSerial(SoftwareSerial &softSerial)
{
  byte Anemometer_request[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};
  digitalWrite(RTS_pin, RS485Transmit);
  softSerial.write(Anemometer_request, sizeof(Anemometer_request));
  softSerial.flush();
  digitalWrite(RTS_pin, RS485Receive);

  static byte buff[8];
  softSerial.readBytes(buff, 8);
  return buff;
}
