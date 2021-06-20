# Weather Station

## Descrition
Estación Meterológica, integrando sensores de velocidad y dirección de viento, temperatura y humedad.

## Components
- ESP8266 (nodemcu v1.0) --> tested with IDE version 1.8.7 and 1.8.9
- wind speed --> RS485 MODBUS protocol of communication
- wind direction --> RS485 MODBUS protocol of communication
- MAX485 --> TTL to RS485
- DS18B20--> 1-Wire Digital Thermometer
- AHT10 --> Integrated temperature and humidity sensor, i2C

## Configuration
| Type          | Pin           | Description           |
| ------------- |:-------------:| ---------------------:|
| Output        |   D0          | RS485 Controller      |
| SCL           |   D1          | I2C communication     |
| SDA           |   D2          | I2C communication     |
| Input/Output  |   D3          | OneWire Bus           |
| Output        |   D4          | LED                   |
| RX1           |   D5          | Serial Recive pin     |
| TX1           |   D6          | Serial Transmit pin   |
| RX2           |   D7          | Serial Recive pin     |
| TX2           |   D8          | Serial Transmit pin   |


## External libraries
- [WiFiManager](https://github.com/tzapu/WiFiManager) - *v0.16.0*
- [Firebase Client](https://github.com/mobizt/Firebase-ESP8266) - *v3.2.3*
- [OneWire](https://github.com/PaulStoffregen/OneWire) - *v2.3.5*
- [DS3231](https://github.com/NorthernWidget/DS3231) - *v1.0.7*
- [DallasTemperature](https://github.com/NorthernWidget/DS3231) - *v3.9.0*