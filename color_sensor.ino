#include<LiquidCrystal.h>
LiquidCrystal lcd(42, 43, 44, 46, 48, 50);
const int S0 = 2;
const int S1 = 3;
const int outPut = 4;
const int S2 = 5;
const int S3 = 6;
unsigned int frequency = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(outPut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
}
void loop()
{
  Serial.print("R=");
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  frequency = pulseIn(outPut, LOW);
  Serial.print(frequency);
  Serial.print("\t");
  delay(500);
  Serial.print("B=");
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(outPut, LOW);
  Serial.print(frequency);
  Serial.print("\t");
  delay(500);
  Serial.print("G=");
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(outPut, LOW);
  Serial.print(frequency);
  Serial.print("\t");
  delay(500);
  Serial.print("\n");
}
