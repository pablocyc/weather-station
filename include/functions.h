#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Arduino.h"
#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial

void SendFirebaseData (long sample, String path, float value);
void ReadWind ();
String readDate ();
String longMonth (int month);
byte * readSoftSerial(SoftwareSerial &softSerial);
#endif //FUNCTIONS_H
