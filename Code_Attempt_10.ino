/* Attempt #10 to code the system, MOCCA.
  Use 20 C to 24 C as default minimum and maximum temperatures comfortable.
  Use 60 % as maximum humidity level comfortable. */

// ==========================================================================================
// TEMPERATURE SENSING
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3                                                   // DS18B20s are on 3.
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress in1Therm = { 0x28, 0xE6, 0x46, 0x2C, 0x06, 0x00, 0x00, 0x43 };        // Blank.
DeviceAddress in2Therm = { 0x28, 0xB2, 0x64, 0x2B, 0x06, 0x00, 0x00, 0x4C };        // Orange.
DeviceAddress out1Therm = { 0x28, 0x74, 0x95, 0x2A, 0x06, 0x00, 0x00, 0x85 };       // Green.
DeviceAddress out2Therm = { 0x28, 0xAA, 0xCB, 0x2B, 0x06, 0x00, 0x00, 0xEA };       // Purple.
// ==========================================================================================
// UNCHANGING VARIABLES
int RedLED = 13, GreenLED = 12, YellowLED = 11;
int HUMmax = 60;
int ReqTemp, MinTemp = 20, MaxTemp = 24;
float inTemp1, inTemp2, outTemp1, outTemp2, inTemp, outTemp;
long previousMillis = 0, currentMillis = 0, interval = 2000;
// ==========================================================================================
// HUMIDITY SENSING
#include <dht.h>
#define dht_dpin A1                                                     // DHT11 is on pin A1.
dht DHT;
// ==========================================================================================
// LCD SETTINGS
#include <stdio.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// ==========================================================================================
// HARDWARE SETTINGS
#define rightKey  0
#define upKey     1
#define downKey   2
#define leftKey   3
#define selectKey 4
#define NUM_KEYS  5
// ==========================================================================================
// MENU PARAMETERS
#define minOpt1  14
#define maxOpt1  22
int valueOpt1 = 20;

#define minOpt2  22
#define maxOpt2  30
int valueOpt2 = 24;

#define programMode1 0
#define programMode2 1
int programMode = programMode1;

boolean inMainProgram = false;

int programmSequence = 0;
#define sequenceModeMinOption 0
#define sequenceModeMaxOption 1

// first menu line - description of menu level
char sequenceHeader[2][17] = {"Min Temp select ", "Max Temp select "};
// second menu line - mode select
char sequenceMinOptions[2][17] = {"      ", "      "};
// third menu line - minimum temperature settings
char sequenceMaxOptions[2][17] = {"      ", "      "};

// ==========================================================================================
// SETUP

void setup()
{
   Serial.begin(9600);
   sensors.begin();
   
   // set the resolution to 10 bit
   sensors.setResolution(in1Therm, 10);
   sensors.setResolution(in2Therm, 10);
   sensors.setResolution(out1Therm, 10);
   sensors.setResolution(out2Therm, 10);
   // sensors.setResolution(extraTherm, 10);
   
   Serial.print("Getting temperatures and humidity...\n\r");
   
   pinMode(RedLED, OUTPUT);
   pinMode(YellowLED, OUTPUT);
   pinMode(GreenLED, OUTPUT);
   
   // initial lcd display while initialize and pc detection
   lcd.begin(16, 2);
   lcd.clear();
   lcd.print("MOCCA v1.0      ");
   
   programmSequence = sequenceModeMinOption;
   inMainProgram = false;
   
   delay(2000);
   updateScreen();
}

// ==========================================================================================
// MAIN LOOP WITH NEW KEY DETECTION

int keyEvent  = -1;
int lastEvent = -1;
int countEvent = 0;

//Key message
char msgs[5][3] = {
   "> ",
   "^ ",
   "v ",
   "< "};

void updateScreen()
{
   lcd.clear();
   lcd.print(sequenceHeader[programmSequence]);
   lcd.setCursor(0,1);
   switch(programmSequence)
   {
       case sequenceModeMinOption:
           menuOptionMinSelect( keyEvent );
           break;
       case sequenceModeMaxOption:
           menuOptionMaxSelect( keyEvent );
           break;
   }
}

void loop()
{
   int keyEvent = detectKey();
   if (keyEvent >= 0)
   {
       switch (keyEvent)
       {
           case upKey:
               if (!inMainProgram)
               {
                   if (programmSequence > sequenceModeMinOption)
                       if(programMode==0)
                       {
                         programmSequence--;
                       }
                   updateScreen();
               }
               break;
           case downKey:
               if (!inMainProgram)
               {
                   if (programmSequence < sequenceModeMaxOption)
                       if(programMode==0)
                       {
                         programmSequence++;
                       }
                   updateScreen();
               }
               break;
           case rightKey:
           case leftKey:
               if (!inMainProgram)
               {
                   switch (programmSequence)
                   {
                       case sequenceModeMinOption:
                           menuOptionMinSelect( keyEvent );
                           break;
                       case sequenceModeMaxOption:
                           menuOptionMaxSelect( keyEvent );
                           break;
                   }
               }
               break;
           case selectKey:
               lcd.clear();
               if (lastEvent != keyEvent)
               {
                   lastEvent = keyEvent;
                   inMainProgram = true;
               }
               break;
       }
   }
   
      if (!inMainProgram && currentMillis - previousMillis > interval)
      {
        updateScreen();                     // This shows the menu until selectKey is pressed.
      }
   
// ==========================================================================================
// This part of the loop controls the sensors and outputs.
   if (inMainProgram)
   {
   sensors.requestTemperatures();
   unsigned long currentMillis = millis();
   
   if (currentMillis - previousMillis > interval){
     previousMillis = currentMillis;
     
     inTemp1 = sensors.getTempC(in1Therm);
     inTemp2 = sensors.getTempC(in2Therm);
     outTemp1 = sensors.getTempC(out1Therm);
     outTemp2 = sensors.getTempC(out2Therm);
   
   if (inTemp1 == -127.00)
   {
     Serial.print("Error getting inside temperature (1)\n\r");
     lcd.setCursor(0,0);
     lcd.print("Err I-1");
   }
   else if (inTemp2 == -127.00)
   {
     Serial.print("Error getting inside temperature (2)\n\r");
     lcd.setCursor(0,0);
     lcd.print("Err I-2");
   }
   else
   {
     inTemp = (inTemp1 + inTemp2) / 2;
     if (inTemp > MaxTemp + 1 || inTemp < MinTemp - 1)
     {
     Serial.print("Inside temperature is: ");
     Serial.print(inTemp);
     Serial.print("\n\r");
     lcd.setCursor(0,0);
     lcd.print("I-T:");
     char cbuf[3] = "  ";
     lcd.setCursor(4,0);
     sprintf(cbuf,"%2d",inTemp);
     lcd.print(inTemp);
     lcd.setCursor(9,0);
     lcd.print("C");
     }
   }
   
   if (outTemp1 == -127.00)
   {
     Serial.print("Error getting outside temperature (1)\n\r");
     lcd.setCursor(0,1);
     lcd.print("Err O-1");
   }
   else if (outTemp2 == -127.00)
   {
     Serial.print("Error getting outside temperature (2)\n\r");
     lcd.setCursor(0,1);
     lcd.print("Err O-2");
   }
   else
   {
     if (inTemp > MaxTemp + 1 || inTemp < MinTemp - 1)
     {
     outTemp = (outTemp1 + outTemp2) / 2;
     Serial.print("Outside temperature is: ");
     Serial.print(outTemp);
     Serial.print("\n\r");
     lcd.setCursor(0,1);
     lcd.print("O-T:");
     char cbuf[3] = "  ";
     lcd.setCursor(4,1);
     sprintf(cbuf,"%2d",outTemp);
     lcd.print(outTemp);
     lcd.setCursor(9,1);
     lcd.print("C");
     }
     else
     {
       lcd.setCursor(0,0);
       lcd.print("  Open a  ");
       lcd.setCursor(0,1);
       lcd.print("  window  ");
     }
   }
   
   // HUMIDITY SENSING
   
   DHT.read11(dht_dpin);
   Serial.print("Humidity = ");
   Serial.print(DHT.humidity);
   Serial.print("%\n\r");
   lcd.setCursor(11,0);
   lcd.print("H(%):");
   char cbuf[3] = "  ";
   lcd.setCursor(11,1);
   sprintf(cbuf,"%2d",DHT.humidity);
   lcd.print(DHT.humidity);
   
   digitalWrite(YellowLED, LOW);
   
   // Minimum and Maximum
   
   MinTemp = valueOpt1;
   MaxTemp = valueOpt2;
   
   // TEMPERATURE COMPARISON
   
   if (outTemp < MinTemp)
   {
     ReqTemp = MinTemp;
   }
   else if (outTemp > MaxTemp)
   {
     ReqTemp = MaxTemp;
   }
   else
   {
     ReqTemp = outTemp;
   }
   
   // LOGIC TESTS (LEDs instead of devices)
   // Red = Heater, Green = Cooler, Yellow = Dehumidifier
   
   if (inTemp < MinTemp && inTemp < ReqTemp)
   {
     digitalWrite(RedLED, HIGH);
     digitalWrite(GreenLED, LOW);
     Serial.print("Switch on Heater\n\n\r");
   }
   else if (inTemp > MaxTemp && inTemp > ReqTemp)
   {
     digitalWrite(RedLED, LOW);
     digitalWrite(GreenLED, HIGH);
     Serial.print("Switch on Cooler\n\n\r");
   }
   else
   {
     digitalWrite(RedLED, LOW);
     digitalWrite(GreenLED, LOW);
     Serial.print("Open a window\n\n\r");
   }
   
   if (DHT.humidity > HUMmax)
   {
     digitalWrite(YellowLED, HIGH);
     Serial.print("Switch on Dehumidifier\n\n\r");
   }
   else
   {
     digitalWrite(YellowLED, LOW);
   }
   }                                                    // This bracket ends the timinig loop.
   }
}

// ==========================================================================================
// Menu tools

void menuModeSelect( int keyEvent )
{
   if (currentMillis - previousMillis < interval){
   switch (keyEvent)
   {
       case rightKey:
           if (programMode < programMode2)
               programMode++;
           break;
       case leftKey:
           if (programMode > programMode1)
               programMode--;
           break;
   }
   lcd.setCursor(0,1);
   }
}

void menuOptionMinSelect( int keyEvent )
{
   if (currentMillis - previousMillis < interval){
   char cbuf[3] = "  ";
   lcd.setCursor(0,1);
   lcd.print(sequenceMinOptions[programMode]);
   switch (keyEvent)
   {
       case rightKey:
                   if (valueOpt1 < maxOpt1)
                       valueOpt1++;
                   break;
           
       case leftKey:

                   if (valueOpt1 > minOpt1)
                       valueOpt1--;
                   break;
   }
           if (valueOpt1 > minOpt1)
               lcd.print("<");
           else
               lcd.print(" ");
               sprintf(cbuf,"%2d",valueOpt1);
               lcd.print(cbuf);
           if (valueOpt1 < maxOpt1)
               lcd.print(">");
           else
               lcd.print(" ");
   }
}

void menuOptionMaxSelect( int keyEvent )
{
   if (currentMillis - previousMillis < interval){
   char cbuf[3] = "  ";
   lcd.setCursor(0,1);
   lcd.print(sequenceMaxOptions[programMode]);
   switch (keyEvent)
   {
       case rightKey:
                   if (valueOpt2 < maxOpt2)
                       valueOpt2++;
                   break;
           
       case leftKey:

                   if (valueOpt2 > minOpt2)
                       valueOpt2--;
                   break;
   }
           if (valueOpt2 > minOpt2)
               lcd.print("<");
           else
               lcd.print(" ");
               sprintf(cbuf,"%2d",valueOpt2);
               lcd.print(cbuf);
           if (valueOpt2 < maxOpt2)
               lcd.print(">");
           else
               lcd.print(" ");
   }
}

// ==========================================================================================
// LCD TOOLS

void clearLine(int line)
{
   lcd.setCursor(0,line);
   lcd.print("                ");
   lcd.setCursor(0,line);
}

// ==========================================================================================
// DEFINE A CUSTOM CHAR IN LCD
int defineCharacter(int ascii, int *data) {
   int baseAddress = (ascii * 8) + 64;  
   // baseAddress = 64 | (ascii << 3);
   lcd.command(baseAddress);
   for (int i = 0; i < 8; i++)
       lcd.write(data[i]);
   lcd.command(128);
   return ascii;
}

// ==========================================================================================
// CONVERT ADC VALUE TO KEY NUMBER

int adc_key_val[NUM_KEYS] ={ 30, 150, 360, 535, 760 };
int get_key(unsigned int input)
{
   int k;
   for (k = 0; k < NUM_KEYS; k++)
   {
       if (input < adc_key_val[k])
           return k;
   }
   if (k >= NUM_KEYS)
       k = -1; // No valid key pressed
   return k;
}

// ==========================================================================================
// NEW KEY DETECTION ROUTINE, WITHOUT DELAYS

int     lastKeyEvent = 0;
int     curKeyEvent  = 0;
int     keyToReturn  = 0;
boolean keyToProcess = false;
int     adc_key_in   = 0;

int detectKey()
{
   keyToReturn = -1;
   adc_key_in = analogRead(0);         // read the value from the sensor  
   curKeyEvent = get_key(adc_key_in);      // convert into key press
   if (curKeyEvent != lastKeyEvent)
   {
       if (!keyToProcess)
       {
           lastKeyEvent = curKeyEvent;
           keyToProcess = true;
       }
       else
       {
           keyToReturn = lastKeyEvent;
           lastKeyEvent = -1;
           keyToProcess = false;
       }
   }
   return keyToReturn;
}

// ==========================================================================================
