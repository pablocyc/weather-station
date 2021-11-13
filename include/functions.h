#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Arduino.h"

void SendFirebaseData (long sample, String path, float value);
void ReadWind ();
String readDate ();
String longMonth (int month);
#endif //FUNCTIONS_H
