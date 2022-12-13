
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include "DHT.h"
#define DHTPIN 5
#define DHTPIN2 18

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);




int sensorValue, gas, gasDiff, co2, h2o , mashy;
int oldGas = 0;
int counter = 0;
int done = 0;
float h, t, f, h2, t2, f2, tempDiff, tempDiff1, tempDiff2;
float temp1 = 0;
float temp2 = 0;
//String txt1 = "As h2o levels increased by.236 ppm, relative humidity was found to elevate by 42%";
char *txt1[] = {"As co2 increases by 425 ppm, it participates in trapping sunlight in the greanhouse. And thus, it elevates temperature levels by 1 degree Celsius.",
                "As h2o levels increased by 346 ppm, relative humidity was found to elevate by 38 percent"
               };
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
//String txt2 = "As h2o levels increased by.....ppm, relative humidity was found to elevate by ...%";

void setup() {
  Serial.begin(9600);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

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
    Serial.println(t);
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
  delay(1000);
}
void loop() {
  //fullcode
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
  gasDiff = gas - oldGas ;
  if (gasDiff < 0)
    gasDiff *= -1;
  Serial.print( "Gass Diff= " );
  Serial.println( gasDiff );
  mashy = gasDiff / 11;
  co2 = mashy * 6;
  Serial.print( "co2=" );
  Serial.print( co2 );
  Serial.println ( " PPM");
  h2o = mashy * 5;
  Serial.print( "h2o=" );
  Serial.print( h2o );
  Serial.println ( "PPM");
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

  // set the fields with the values
  ThingSpeak.setField(1, tempDiff);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, co2);
  ThingSpeak.setField(4, h2o);

  ThingSpeak.setStatus(txt1[1]);


  // figure out the status message

  // set the status

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  // change the values
  delay(5000); // Wait 20 seconds to update the channel again
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
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print(F("Humidity: "));
  //Serial.print(h);
  Serial.print(F("Temperature2: "));
  Serial.print(t, 1);
  Serial.println(F(" °C "));
  Serial.println();
  delay(100);

}
void gasReading()
{
  sensorValue = analogRead(36);
  Serial.print("AirQua=");
  Serial.print(sensorValue, DEC);
  Serial.println(" PPM");
  Serial.println();
  delay(100);

}
