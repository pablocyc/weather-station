/*
Weather Station
_________________________________________________________________
|                                                               |
|       author : Pablo Cardozo <pablocyc83@gmail.com>           |
|       Any feedback is welcome                                 |
|                                                               |
_________________________________________________________________

Materials :
1* ESP8266 (nodemcu v1.0) --> tested with IDE version 1.8.7 and 1.8.9
1* wind sensor --> RS485 MODBUS protocol of communication
1* wind direction --> RS485 MODBUS protocol of communication
1* MAX485 --> TTL to RS485
1* DS18B20--> 1-Wire Digital Thermometer
1* AHT10 --> Integrated temperature and humidity sensor, i2C
*/

#include "config.h"   // header include, host and token to Firebase, SSID and password to WiFi connection
#include <SoftwareSerial.h>  // https://github.com/PaulStoffregen/SoftwareSerial
#include "FirebaseESP8266.h"	// Install Firebase ESP8266 library
#include <ESP8266WiFi.h>
#include <DS3232RTC.h>      // https://github.com/JChristensen/DS3232RTC
#include <TimeLib.h>
#include <OneWire.h>        // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>    // https://github.com/milesburton/Arduino-Temperature-Control-Library

#define FIREBASE_HOST HostFirebase 
#define FIREBASE_AUTH TokenFirebase
#define WIFI_SSID     WifiSSID
#define WIFI_PASSWORD WifiPASS
#define ONE_WIRE_BUS  D3    // Data wire
#define RX            D5    //Serial Receive pin
#define TX            D6    //Serial Transmit pin
#define RTS_pin       9    //RS485 Direction control
#define RTS2_pin      10    //RS485 Direction control
#define Led_pin       D4    //LED
#define RS485Transmit    HIGH
#define RS485Receive     LOW

String pathHeaderFirebase = "/station-home/";


SoftwareSerial RS485Speed(RX, TX);

// Setup RTC
DS3232RTC RTC;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Number of temperature devices found
int numberOfDevices;

// We'll use this variable to store a found device address
DeviceAddress tempDeviceAddress; 

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseData ledData;

FirebaseJson json;

  float tempC = 17.3;
  String date = "2021-06-14T02:20:05Z";

  String pathHeaderTemp = pathHeaderFirebase + "temperature/";
  String pathTemp = pathHeaderTemp + "s1/" + "average";
  String pathDate = pathHeaderTemp + "s1/" + "date";

void setup() {
  // pinMode(RTS_pin, OUTPUT);
  // pinMode(RTS2_pin, OUTPUT);
  pinMode(Led_pin, OUTPUT);
  
  // Start the built-in serial port, for Serial Monitor
  Serial.begin(115200);
  Serial.println("Weather Station");

  // Start the Modbus serial Port, for anemometer
  RS485Speed.begin(9600);   
  digitalWrite(Led_pin, LOW);
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
  
  WiFi.begin(WifiSSID, WifiPASS);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(HostFirebase, TokenFirebase);
  Firebase.reconnectWiFi(true);
}

void loop() {
  
  if (Serial.available()) {
    char read = Serial.read();
    if (read == 's') {
      if (Firebase.setString(firebaseData, pathDate, date)) {
        Serial.println("PASSED_Date");
      }
      if (Firebase.setFloat(firebaseData, pathTemp, tempC)) {
        Serial.println("PASSED");
        Serial.println("PATH_TEMP: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
        Serial.println("ETag: " + firebaseData.ETag());
        Serial.println("------------------------------------");
        Serial.println();
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  }
}


