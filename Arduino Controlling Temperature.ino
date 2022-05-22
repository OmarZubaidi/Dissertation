// http://www.instructables.com/id/Moisture-Test-Build-an-Arduino-Controlled-Low-Tem/step7/Program-the-Arduino-to-Control-the-Temperature/

int pin = 5; // analog pin
int putPin = 3; // humidity
int tempc = 0,tempf=0; // temperature variables
int samples[8]; // variables to make a better precision
int maxi = -100,mini = 100; // to start max/min temperature
int i;

float humi = 0;
float prehum = 0;
float humconst = 0;
float truehum = 0;
float pretruehum = 0;
long pretruehumconst = 0;
long valb = 0;

void setup()
{
    Serial.begin(9600); // start serial communication
}

void loop()
{

    for(i = 0;i<=7;i++){
        samples[i] = ( 5.0 * analogRead(pin) * 100.0) / 1024.0;
        tempc = tempc + samples[i];
        delay(1000);
    }

    tempc = tempc/8.0;
    tempf = (tempc * 9)/ 5 + 32;

    valb = analogRead(putPin); // humidity calculation
    prehum = (valb/5);
    humconst = (0.16/0.0062);
    humi = prehum - humconst;
    pretruehumconst = 0.00216*tempc;
    pretruehum = 1.0546-pretruehumconst;
    truehum = humi/pretruehum ;
    delay(1000);

    Serial.print(tempc,DEC);
    Serial.print(" Celsius, ");
    Serial.print ("Humidity: ");
    Serial.print ((long)truehum);
    Serial.println ("% ");

    tempc = 0;

    delay(1000); // delay before loop
}

// After everything is done click on the upload button which will take a little while to upload and when the
// uploading is done make sure you click on the Serial Communication to get the readings from the sensor