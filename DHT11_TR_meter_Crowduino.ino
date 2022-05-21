/*
Project: Temperature and Humidity meter with seven segment display
Description: Uses the LedControl library and Adafruit's DHT sensor
library for reading temperature and humidity measurements from DHT11
sensor and displaying the result on MAX7219-driven seven segment LED
displays.

Posted on: www.embedded-lab.com
URL of the project: http://embedded-lab.com/blog/?p=8414
*/
#include "DHT.h"
#include "LedControl.h"

#define DHTPIN 12     // what pin we're connected to
#define VCCPIN 11     //
#define DHTTYPE DHT11   // DHT 11 

// Connect pin 1 (on the left) of the sensor to Pin 11 of Arduino
// Connect pin 2 of the sensor to whatever your Pin 12 of Arduino
// Connect pin 4 (on the right) of the sensor to GROUND
int tempC, rh;

DHT dht(DHTPIN, DHTTYPE);
LedControl lc=LedControl(5,7,6,0); // Pin 5 to Data In, 7 to Clk, 6 to LOAD
void setup() {
  pinMode(VCCPIN, OUTPUT);
  digitalWrite(VCCPIN, HIGH);
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
  dht.begin();
  
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,12);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  tempC = (int)t;
  rh   = (int)h;
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Humidity: "); 
    Serial.print(rh);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(tempC);
    Serial.println(" *C");
    Disp_Data();
    delay(1000);
  }

}


void Disp_Data(){
  int ones, tens;
  // First display temperature in row 1
  ones = tempC%10;
  tens = (tempC/10)%10;
  lc.setDigit(0,3,(byte)tens,false);
  lc.setDigit(0,2,(byte)ones,false);
  lc.setChar(0, 1, '/', false);
  lc.setChar(0, 0, 'C', false);
  
  // Now display RH in row 2
  ones = rh%10;
  tens = (rh/10)%10;
  lc.setDigit(0,7,(byte)tens,false);
  lc.setDigit(0,6,(byte)ones,false);
  lc.setChar(0, 5, ' ', false);
  lc.setChar(0, 4, 'P', false);
  
 }


