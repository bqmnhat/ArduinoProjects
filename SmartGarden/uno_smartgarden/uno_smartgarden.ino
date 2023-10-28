#include <DHT.h>
#include <SoftwareSerial.h>

#define DHT11_PIN 4
#define DHTTYPE DHT11
#define SOISPin A0
#define LIGPIN 5
#define FANPIN 6
#define PUMPPIN 7
#define LEDPIN 8

const double HUM_THRES = 50;
const double TMP_THRES = 34;
const double SOI_THRES = 1000;
const double LIG_THRES = 1; //1: darkness

DHT dht(DHT11_PIN, DHTTYPE);
SoftwareSerial espSerial(2, 3); //RX TX
int count = 0;
void setup(){
  Serial.begin(115200);
  espSerial.begin(115200);
  dht.begin();
  pinMode(LIGPIN, INPUT);
  pinMode(FANPIN, OUTPUT);
  digitalWrite(FANPIN, HIGH); // Kích khi low
  pinMode(PUMPPIN, OUTPUT); 
  digitalWrite(PUMPPIN, HIGH);// Kích khi low
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW); // Kích khi HIGH
  delay(2000);
}

double ReadSoi() {
  double Soi = 0; 
  for (int i = 0; i <= 100; i++) 
  { 
    Soi = Soi + analogRead(SOISPin); 
    delay(1); 
  } 
  Soi = Soi/100.0; 
  return Soi;
}

double ReadHum() {
  double Hum = dht.readHumidity();
  return Hum;
}

double ReadTemp() {
  double Temp = dht.readTemperature();
  return Temp;
}

int ReadLig() {
  return digitalRead(LIGPIN);
}

void PrintVal(double Val, String CMD) {
  if (isnan(Val))
    return;
  String cmd = CMD + String(Val) + "#";
  Serial.println(cmd);
  espSerial.print(cmd);
}

//Print 4 commands at the same time
void PrintValV2(double HUM, double TEM, double SOI, int LIG) {
  if (isnan(HUM) && isnan(TEM) && isnan(SOI) && isnan(LIG))
    return;
  String cmd = "HUM:" + String(HUM) + "#" + "TEM:" + String(TEM) + "#" + "SOI:" + String(SOI) + "#" + "LIG:" + String(LIG) + "#$";
  Serial.println(cmd);
  espSerial.print(cmd);
}

void PrintToMONandESPV2(double HUM, double TEM, double SOI, int LIG) {
  PrintValV2(HUM, TEM, SOI, LIG);
}

void PrintToMONandESP(double HUM, double TEM, double SOI, int LIG) {
  PrintVal(HUM, "HUM:");
  PrintVal(TEM, "TEM:");
  PrintVal(SOI, "SOI:");
  PrintVal(LIG, "LIG:");
}

void ControlEnv(double HUM, double TEM, double SOI, int LIG){
  if(TEM >= TMP_THRES){
    digitalWrite(FANPIN, LOW); // Kích khi low
  }else{
    digitalWrite(FANPIN, HIGH); // 
  }

  if(SOI >= SOI_THRES){
    digitalWrite(PUMPPIN, LOW); // Kích khi low
  }else{
    digitalWrite(PUMPPIN, HIGH); // 
  }

  if(LIG == LIG_THRES){
    digitalWrite(LEDPIN, HIGH); // Kích khi HIGH
  }else{
    digitalWrite(LEDPIN, LOW); // 
  }
}

void loop(){  
  double HumVal = ReadHum(), TemVal = ReadTemp(), SoiVal = ReadSoi();
  int LigVal = ReadLig();
  ControlEnv(HumVal, TemVal, SoiVal, LigVal);
  //PrintToMONandESP(HumVal, TemVal, SoiVal, LigVal);
  PrintToMONandESPV2(HumVal, TemVal, SoiVal, LigVal);
  delay(15000); //delay 60s
}
