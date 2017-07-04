

#include <doxygen.h>
#include <ESP8266.h>

/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
#include <SoftwareSerial.h>
#define SSID        "ImYourFather"
#define PASSWORD    "19870315"

String cmd = "";
uint8_t buffer[1024] = {0};
SoftwareSerial mySerial(10, 11); // rx:10,tx:11

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600); // serial port used for debugging
  mySerial.begin(9600);  // your ESP's baud rate might be different
  ESP8266 wifi(mySerial);
  //
  //    Serial.begin(9600);
  //    Serial.print("setup begin\r\n");
  //    Serial.print("FW Version: ");
  //    Serial.println(wifi.getVersion().c_str());
  //    if (wifi.setOprToStation()) {
  //      Serial.print("to station ok\r\n");
  //    } else {
  //      Serial.print("to station err\r\n");
  //    }
  //
  //    if (wifi.joinAP(SSID, PASSWORD)) {
  //        Serial.print("Join AP success\r\n");
  //        Serial.print("IP: ");
  //        Serial.println(wifi.getLocalIP().c_str());
  //    } else {
  //          Serial.print("Join AP failure\r\n");
  //    }
  //
  //    Serial.print("setup end\r\n");
}

// the loop routine runs over and over again forever:
void loop() {

  if (Serial.available()) {
    int c = Serial.read();
    cmd += (char)c;
  }
  if (cmd.endsWith("\r\n")) {
    execute(cmd);
  }
  uint8_t i;
}

void execute(String command){

  
}


