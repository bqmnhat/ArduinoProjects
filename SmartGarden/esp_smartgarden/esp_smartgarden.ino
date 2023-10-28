#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "ThingSpeak.h"

const int HUM_FIELD = 1;
const int TMP_FIELD = 2;
const int SOI_FIELD = 3;
const int LIG_FIELD = 4;

const String HUM_CMD = "HUM";
const String TMP_CMD = "TEM";
const String SOI_CMD = "SOI";
const String LIG_CMD = "LIG";

unsigned long myChannelNumber = 1679843;
const char * myWriteAPIKey = "YUSHY2W2TOY9Z6MA";

ESP8266WiFiMulti WiFiMulti;
WiFiClient  client;
String allData, cmdReceived;
char c;
String url = "https://api.thingspeak.com/update?api_key=YUSHY2W2TOY9Z6MA";

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("mydlink", "Dongden06");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  
}


void sendToThingSpeak(String cmd){
  String op, value;
  int x;
  
  op = cmd.substring(0, cmd.indexOf(':'));
  value = cmd.substring(cmd.indexOf(':')+1, cmd.length());

  Serial.println("OP: " + op);
  Serial.println("Value: " + value);
  if(op==HUM_CMD){
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    x = ThingSpeak.writeField(myChannelNumber, HUM_FIELD, value, myWriteAPIKey);
  }
  if(op==TMP_CMD){
    x = ThingSpeak.writeField(myChannelNumber, TMP_FIELD, value, myWriteAPIKey);
  }
  if(op==SOI_CMD){
    x = ThingSpeak.writeField(myChannelNumber, SOI_FIELD, value, myWriteAPIKey);
  }
  if(op==LIG_CMD){
    x = ThingSpeak.writeField(myChannelNumber, LIG_FIELD, value, myWriteAPIKey);
  }
  if(x == 200){
    Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
  delay(15000);
}

//Send 4 fields at the same time
//cmd: HUM:xx#TEM:yy#SOI:zz#LIG:tt#
void sendToThingSpeakV2(String cmd){
  String op, value;
  int x;
  while(cmd.length()>0){
    op = cmd.substring(0,cmd.indexOf(':'));
    value = cmd.substring(cmd.indexOf(':')+1, cmd.indexOf('#'));
    cmd = cmd.substring(cmd.indexOf('#')+1, cmd.length());
    Serial.println(op + String(':') + value);
    if(op==HUM_CMD){
      x = ThingSpeak.setField(1, value);
    }
    if(op==TMP_CMD){
      x = ThingSpeak.setField(2, value);
    }
    if(op==SOI_CMD){
      x = ThingSpeak.setField(3, value);
    }
    if(op==LIG_CMD){
      x = ThingSpeak.setField(4, value);
    }
  }
  x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(15000);
}


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.print("Wifi connected!!!\n");
    Serial.print("Checking data on Serial port\n");
    cmdReceived = "";
    while (Serial.available()) {
      c = Serial.read();
      if (c=='$'){
        //Process the received command: cmdReceived
        Serial.print("cmd Received: \n");
        Serial.println(cmdReceived);
        sendToThingSpeakV2(cmdReceived);
        Serial.print("-------------------\n");
        //Reset cmdReceived
        cmdReceived = "";
      }else{
        cmdReceived += c;
      }
    }
    Serial.print("No data available on Serial port\n");
  }else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  Serial.println("Wait 1s before next round...");
  delay(1000);
}
