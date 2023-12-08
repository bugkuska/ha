// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include "DHT.h"
#define DHTPIN 18      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

//SoilMoisture
#define INPUT_1 35

//============Modbus Master=============//
#include <ModbusMaster.h>
#define RX1 26  //RO,RX
#define TX1 27  //DI,TX
//============Modbus Master=============//
//=============Modbus Object============//
ModbusMaster node1;  //XY-MD02 Slave ID1
//=============Modbus Object============//

//=========IO connect to relay==========//
#define relay1 32
#define relay2 33
//=========IO connect to relay==========//

//===========Setup Function=============//
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX1, TX1);  //RX1=26 ,TX1=27
  node1.begin(1, Serial2);                    //XY-MD02 Slave ID1

  dht.begin();

  //Set Mode
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  //Set default status
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
}
//===========Setup Function=============//

//=============Loop Function============//
void loop() {
  //Modbus Sensor
  uint8_t result1;
  float temp1 = (node1.getResponseBuffer(0) / 10.0f);
  float humi1 = (node1.getResponseBuffer(1) / 10.0f);
  Serial.println("Get XY-MD02 Data1");
  result1 = node1.readInputRegisters(0x0001, 2);  // Read 2 registers starting at 1)
  if (result1 == node1.ku8MBSuccess) {
    Serial.print("Temp1: ");
    Serial.println(node1.getResponseBuffer(0) / 10.0f);
    Serial.print("Humi1: ");
    Serial.println(node1.getResponseBuffer(1) / 10.0f);
  }
  delay(1000);
//DHT
// Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

//SoilMoisture
float moisture_percentage1;
  int sensor_analog1;
  sensor_analog1 = analogRead(INPUT_1);
  Serial.print("Law Soil data 1:");
  Serial.println(sensor_analog1);
  moisture_percentage1 = (100 - ((sensor_analog1 / 4095.00) * 100));

  Serial.print("Moisture Percentage 1= ");
  Serial.print(moisture_percentage1);
  Serial.print("%\n\n");
  delay(1000);

  digitalWrite(relay1, HIGH);
  delay(1000);
  digitalWrite(relay1, LOW);
  delay(1000);

  digitalWrite(relay2, HIGH);
  delay(1000);
  digitalWrite(relay2, LOW);
  delay(1000);


  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  delay(1000);

  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  delay(1000);

}
//=============Loop Function============//
