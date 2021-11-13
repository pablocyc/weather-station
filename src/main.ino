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

#include "functions.h" //NEW HEADER BY PEDRO
#include "config.h"   // header include, host and token to Firebase, SSID and password to WiFi connection
#include <EEPROM.h>
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
#define RX2           D7    //Serial Receive pin
#define TX2           D8    //Serial Transmit pin
#define RTS_pin       D0    //RS485 Direction control
#define RTS2_pin      10    //RS485 Direction control
#define Led_pin       D4    //LED
#define RS485Transmit    HIGH
#define RS485Receive     LOW

// Global Variables
// EEPROM -> [month, st, sh, sp, ss, sd]
byte EEmonth = 20, EEst = 21, EEsh = 22, EEsp = 23, EEss = 24, EEsd = 25;
String pathHeaderFirebase = "/station-home/";
String pathTemp = "temperature/";
String pathHum = "humidity/";
String pathPress = "pressure/";
String pathSpeed = "wind_speed/";
String pathDirection = "wind_direction/";
bool h12Flag, pmFlag;
bool century = false;
int st, sh, sp, ss, sd;
byte monthOld;
float tempOld;
float humOld;
float pressOld;
float speedOld;
float directionOld;
unsigned long currentTime;  //actual time for async reading sensors
unsigned long deltaTime = 2000;  //interval between sensors readings, default 2000ms


SoftwareSerial RS485Serial(RX, TX);
SoftwareSerial RS485Serial2(RX2, TX2);
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

void readEEPROM () {
  EEPROM.get(EEst, st);
  EEPROM.get(EEsh, sh);
  EEPROM.get(EEsp, sp);
  EEPROM.get(EEss, ss);
  EEPROM.get(EEsd, sd);
  EEPROM.get(EEmonth, monthOld);
  Serial.println(st);
  Serial.println(sh);
  Serial.println(sp);
  Serial.println(ss);
  Serial.println(sd);
  Serial.println(monthOld);
}

void setup() {
  EEPROM.begin(32);
  pinMode(RTS_pin, OUTPUT);
  // pinMode(RTS2_pin, OUTPUT);
  pinMode(Led_pin, OUTPUT);

  // Start the built-in serial port, for Serial Monitor
  Serial.begin(115200);
  Serial.println("Weather Station");

  // Start the Modbus serial Port, for anemometer
  RS485Serial.begin(9600);
  RS485Serial2.begin(9600);    // RS485Serial to Wind Direction.
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

  readEEPROM();
  // Start the I2C interface
	Wire.begin();

  if (!bme.begin(0x76, &Wire)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    while (1) delay(10);
  }

}

void loop() {
  if (Serial.available()) {
    char serialData = Serial.read();
    // Reset EEPROM
    if (serialData == 'r') {
      EEPROM.put(EEst, 0);
      EEPROM.put(EEsh, 0);
      EEPROM.put(EEsp, 0);
      EEPROM.put(EEss, 0);
      EEPROM.put(EEsd, 0);
      EEPROM.put(EEmonth, 6);
      bool ok = EEPROM.commit();
      Serial.println((ok) ? "Commit OK" : "Commit failed");
      Serial.println("--------RESET EEPROM------------");
      readEEPROM();
    }
  }

  if (millis() - currentTime > deltaTime) {
    currentTime = millis();
    readWind();
    float diff, read;
    read = bme.readTemperature();
    diff = abs(read - tempOld);
    if (diff >= 0.21) {
      tempOld = read;
      sendFirebase(st++, pathTemp, read);
      EEPROM.put(EEst, st);
      EEPROM.commit();
    }

    read = bme.readHumidity();
    diff = abs(read - humOld);
    if (diff >= 1.9) {
      humOld = read;
      sendFirebase(sh++, pathHum, read);
      EEPROM.put(EEsh, sh);
      EEPROM.commit();
    }
    read = bme.readPressure();
    diff = read - pressOld;
    if (diff < 0) diff *= -1;
    if ((read != pressOld) && (diff >= 33)) {
      pressOld = read;
      sendFirebase(sp++, pathPress, read);
      EEPROM.put(EEsp, sp);
      EEPROM.commit();
    }
  }
}


void readWind () {
  int diff;
  byte Anemometer_request[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};

  digitalWrite(RTS_pin, RS485Transmit);
  RS485Serial.write(Anemometer_request, sizeof(Anemometer_request));
  RS485Serial.flush();
  digitalWrite(RTS_pin, RS485Receive);

  byte Anemometer_buf[8];
  RS485Serial.readBytes(Anemometer_buf, 8);
  diff = abs(Anemometer_buf[4] - speedOld);

  if (diff >= 3) {
    speedOld = Anemometer_buf[4];
    sendFirebase(ss++, pathSpeed, Anemometer_buf[4] * 3.6); // Convert speed m/s to km/h
    EEPROM.put(EEss, ss);
    EEPROM.commit();
  }

  digitalWrite(RTS_pin, RS485Transmit);
  RS485Serial2.write(Anemometer_request, sizeof(Anemometer_request));
  RS485Serial2.flush();
  digitalWrite(RTS_pin, RS485Receive);

  byte Anemometer_dir_buf[8];
  RS485Serial2.readBytes(Anemometer_dir_buf, 8);

  float wind_deg;
  wind_deg = (Anemometer_dir_buf[3]>0) ? 256 + Anemometer_dir_buf[4] : Anemometer_dir_buf[4];
  diff = abs(wind_deg - directionOld);

  if (diff >= 4) {
    directionOld = wind_deg;
    sendFirebase(sd++, pathDirection, wind_deg);
    EEPROM.put(EEsd, sd);
    EEPROM.commit();
    Serial.print("WindDir - Send: ");
    Serial.println(wind_deg);
    Serial.println();
  }
}

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

