#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Arduino.h"

void SendFirebaseData (long sample, String path, float value);
void ReadWind ();
#endif //FUNCTIONS_H
