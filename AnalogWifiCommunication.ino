/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
#include <SoftwareSerial.h>
#include "Esp8266.h"
#include "Buffer.h"

#define CH_PD 3
#define RST 4
#define WPS 7
#define WIFI 6

#define SSRX 10
#define SSTX 11

Buffer EspBuffer;

Buffer SerialBuffer;

SoftwareSerial mySerial(SSRX, SSTX); // rx:10,tx:11

Esp8266 esp(mySerial);

bool directmode = false;

unsigned long wpsTimeout = 0;

const char _rest_esp[] = "reset ESP";
/**
 * reset esp until read ready without timeout
 */
void resetEsp() {
  do{
    Serial.println(_rest_esp);
    digitalWrite (RST, LOW);
    delay(500);
    digitalWrite (RST, HIGH);
  }while(esp.init());
}

void onTcpRecvive (Buffer);

const char _start_up[] = "Start up.";
// the setup routine runs once when you press reset:
void setup() {
  pinMode (RST, OUTPUT);
  pinMode (CH_PD, OUTPUT);
  pinMode (WIFI,OUTPUT);
  pinMode (WPS,INPUT);
  digitalWrite(CH_PD, HIGH);
  digitalWrite(RST,HIGH);
  
  
  Serial.begin(9600); // serial port used for debugging
  Serial.println(_start_up);
  
  mySerial.begin(9600);  // your ESP's baud rate might be different
  
  resetEsp();
  server();
  esp.onTcpRecvive = &onTcpRecvive;
}

// the loop routine runs over and over again forever:
void loop() {  
  if (Serial.available()) {
    int c = Serial.read();
    SerialBuffer.appendChar((char)c);
  }

  if(mySerial.available())
    if(directmode){
      outputDirect();
    }else{
      esp.readData();
    }

//  if(digitalRead(WPS) == HIGH && ((long)(millis() - wpsTimeout) >= 0)){
//    wpsTimeout = millis() + 1000UL;
//    esp.wps();
//  }

//  if(esp.isWifiConnected()){
//    digitalWrite(WIFI,HIGH);
//  }
  
  if (SerialBuffer.endLine()) {
    if(directmode){
      executeDirect();
    }else{
      execute();
    }
  }

  
}

const char _got_tcp[] = "[GOT TCP]{";
const char _end[] = "}";
void onTcpRecvive (Buffer tcpBuffer){
  Serial.print(_got_tcp);
  Serial.print(tcpBuffer.str());
  Serial.println(_end);
  if(tcpBuffer.endLine()){
    
    tcpBuffer.clear();
  }
}


void outputDirect(){
  while(mySerial.available()){
    int c = mySerial.read();
    EspBuffer.appendChar((char)c);
    if(EspBuffer.endLine()){
      Serial.print(EspBuffer.str());
      EspBuffer.clear();
      return;
    }
  }   
}

void executeDirect(){
  Serial.print("execute direct:");
  Serial.print(SerialBuffer.str());
  if(SerialBuffer.equals("exit\r\n",6)){
    Serial.println("exit direct mode");
    directmode = false;
  }else{
    mySerial.print(SerialBuffer.str());
    
  }
  SerialBuffer.clear();
}

bool cmdIs(const char* cmd,int len){
  return SerialBuffer.equals(cmd,len);
}

void execute(){
  SerialBuffer.trimEndLine();
  Serial.print("execute cmd:");
  Serial.println(SerialBuffer.str());

  if(cmdIs("test",4)){
    test();
  }else if(cmdIs("AT",2)){
    directmode = true;
    Serial.println("enter direct mode.");
  }else if(cmdIs("reset",5)){
    resetEsp();
  }else if(cmdIs("server",6)){
    server();
  }else if(cmdIs("wps",3)){
    esp.wps();
  }else{
    Serial.print("command: ");
    Serial.print(SerialBuffer.str());
    Serial.println(" not exist.");
  }
  SerialBuffer.clear();
}

void test(){
  if(!esp.kick()) 
    Serial.print("kick OK");
  else
    Serial.print("kick Error");
}

const char _server_start[] = "[SERVER START]";

void server(){
  if(!esp.setServer()){
    Serial.println(_server_start);
  }
}







