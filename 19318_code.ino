#include <ESP32Servo.h>
Servo myservo;

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include "DHT.h"
#define DHTPIN 18
#define DHTPIN2 5
#include <stdio.h>
#include <stdlib.h>
int servoPin = 19;
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

int sensorValue, gas, gasDiff, co2, h2o, moles;
int oldGas = 0;
int counter = 0;
int done = 0;
float h, t, f, h2, t2, f2, tempDiff, tempDiff1, tempDiff2;
float temp1 = 0;
float temp2 = 0;

char *txt1[300];

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

int co2Val, tempVal, h2oVal, humVal;
char snum[10];

// Initialize our values
//String txt2 = "As h2o levels increased by.....ppm, relative humidity was found to elevate by ...%";

void setup() {
  pinMode(21, OUTPUT);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);            // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000);  // attaches the servo on pin 18 to the servo object
  myservo.attach(9);
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  servoClose();

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);


  Serial.println(F("DHTxx test!"));
  dht.begin();  
  dht2.begin();
  while (millis() < 2000) {
    dht2Reading();
    dhtReading();
    gasReading();
    counter += 1;
    temp1 += t;
    Serial.print("temp1= ");
    Serial.print(t);
    temp2 += t2;
    Serial.print("\t temp2= ");
    Serial.println(t2);
    gas += sensorValue;
    Serial.println();
    delay(600);
  }
  temp1 /= counter;
  Serial.print("Temp1 average : ");
  Serial.print(temp1);
  temp2 /= counter;
  Serial.print("\t Temp2 average : ");
  Serial.println(temp2);
  gas /= counter;
  Serial.print("\t Gas average : ");
  Serial.println(gas);
  Serial.println();
  delay(20000);
  servoOpen();
  digitalWrite(21, OUTPUT);
}
void loop() {
  //fullcode
  char txt[500] = "As co2 increases by ";
  //co2Val
  char txt2[] = " ppm, it participates in trapping sunlight in the greanhouse. And thus, it elevates temperature levels by ";
  //tempVal
  char txt3[] = " As h2o levels increased by  ";
  //h2oVal
  char txt4[] = "  ppm relative humidity was found to elevate by ";
  //humVal

  // convert 123 to string [buf]

  dht2Reading();
  dhtReading();

  tempDiff1 = t - temp1;
  if (tempDiff1 < 0)
    tempDiff1 *= -1;
  Serial.print("Actual Temp1 : ");
  Serial.print(t);
  Serial.print("\t Temp Diff1= ");
  Serial.println(tempDiff1);
  tempDiff2 = t2 - temp2;
  if (tempDiff2 < 0)
    tempDiff2 *= -1;
  Serial.print("Actual Temp2 : ");
  Serial.print(t2);
  Serial.print("\t Temp Diff2= ");
  Serial.println(tempDiff2);
  tempDiff = tempDiff1 - tempDiff2;
  if (tempDiff < 0)
    tempDiff *= -1;
  Serial.print("Final Temrature difference = ");
  Serial.print(tempDiff);
  Serial.println(" °C");
  gasDiff = gas - oldGas;
  if (gasDiff < 0)
    gasDiff *= -1;
  Serial.print("Gass Diff= ");
  Serial.println(gasDiff);
  moles = gasDiff / 11;
  co2 = moles * 6;
  Serial.print("co2=");
  Serial.print(co2);
  Serial.println(" PPM");
  h2o = moles * 5;
  Serial.print("h2o=");
  Serial.print(h2o);
  Serial.println("PPM");
  oldGas = sensorValue;
  gasReading();


  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  co2Val = abs(co2 - co2Val);
  co2Val = co2;
  Serial.print("co2Val=");
  Serial.println(co2Val);

  itoa(co2Val, snum, 10);
  strncat(txt, snum, 10);

  strncat(txt, txt2, 200);
  // Serial.print("tempDiff= ");
  // Serial.println(tempDiff);
  tempVal = abs(tempDiff - tempVal);
  // Serial.print("tempValold=");
  // Serial.println(tempVal);

  tempVal = tempDiff;

  // Serial.print("tempVal=");
  // Serial.println(tempVal);

  itoa(tempVal, snum, 10);
  strncat(txt, snum, 10);

  strncat(txt, txt3, 200);

  h2oVal = abs(h2o - h2oVal);
  h2oVal = h2o;
  itoa(h2oVal, snum, 10);
  strncat(txt, snum, 10);

  strncat(txt, txt4, 200);

  humVal = abs(h - humVal);
  humVal = h;
  itoa(humVal, snum, 10);
  strncat(txt, snum, 10);

  // set the fields with the values
  ThingSpeak.setField(1, tempDiff);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, co2);
  ThingSpeak.setField(4, h2o);

  ThingSpeak.setStatus(txt);


  // figure out the status message

  // set the status

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  // change the values
  delay(5000);  // Wait 20 seconds to update the channel again
}

void dhtReading() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%\t Temperature1: "));
  Serial.print(t, 1);
  Serial.println(F(" °C "));
  Serial.println();
  delay(100);
}
void dht2Reading() {
  h2 = dht2.readHumidity();
  t2 = dht2.readTemperature();
  f2 = dht2.readTemperature(true);
  if (isnan(h2) || isnan(t2) || isnan(f2)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht2.computeHeatIndex(f2, h2);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht2.computeHeatIndex(t2, h2, false);

  //Serial.print(F("Humidity: "));
  //Serial.print(h);
  Serial.print(F("Temperature2: "));
  Serial.print(t2, 1);
  Serial.println(F(" °C "));
  Serial.println();
  delay(100);
}
void gasReading() {
  sensorValue = analogRead(36);
  Serial.print("AirQua=");
  Serial.print(sensorValue, DEC);
  Serial.println(" PPM");
  Serial.println();
  delay(100);
}
void servoOpen() {
  myservo.write(0);  // tell servo to go to position in variable 'pos'
}
void servoClose() {
  myservo.write(90);  // sets the servo position according to the scaled value
}
