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
#include "FirebaseESP8266.h"	// https://github.com/mobizt/Firebase-ESP8266
#include <ESP8266WiFi.h>
#include <OneWire.h>        // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>    // https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DS3231.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>


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

// Global Variables
String pathHeaderFirebase = "/station-home/";
String pathTemp = "temperature/";
String pathHum = "humidity/";
String pathPress = "pressure/";
String pathSpeed = "wind_speed/";
String pathDirection = "wind_direction/";
bool h12Flag, pmFlag;
bool century = false;
long st, sh, sp, ss, sd;
int monthOld;
float tempOld;
float humOld;
float pressOld;
float speedOld;
float directionOld;


SoftwareSerial RS485Speed(RX, TX);
Adafruit_BME280 bme; // I2C

// Setup RTC
DS3231 RTC;

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

float tempC = 0;
// String sensorStatus = "temperature/";


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

  // Start the I2C interface
	Wire.begin();

  if (! bme.begin(0x76, &Wire)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    while (1) delay(10);
  }

}

void loop() {

  float diff, read;
  read = bme.readTemperature();
  diff = abs(read - tempOld);
  if ((read != tempOld) && (diff >= 0.2)) {
    tempOld = read;
    sendFirebase(st++, pathTemp, read);
  }
  read = bme.readHumidity();
  diff = abs(read - humOld);
  if ((read != humOld) && (diff >= 1)) {
    humOld = read;
    sendFirebase(sh++, pathHum, read);
  }
  read = bme.readPressure();
  diff = abs(read - pressOld);
  if ((read != pressOld) && (diff >= 10)) {
    pressOld = read;
    sendFirebase(sp++, pathPress, read);
  }
  

  Serial.print(F("Temperature = "));
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  Serial.print(F("Pressure = "));
  Serial.print(bme.readPressure());
  Serial.println(" Pa");

  Serial.print(F("Approx altitude = "));
  Serial.print(bme.readAltitude(1013.25)); /* Adjusted to local forecast! */
  Serial.println(" m");
  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Serial.println();
  delay(2000);
}

void sendFirebase (long sample, String path, float value) {
  int month = RTC.getMonth(century);
  if (month != monthOld) {
    monthOld = month;
    st, sh, sp, ss, sd = 0;
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
