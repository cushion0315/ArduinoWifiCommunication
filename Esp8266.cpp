#include "Esp8266.h"


#ifdef ESP8266_USE_SOFTWARE_SERIAL

Esp8266::Esp8266(SoftwareSerial &uart): m_puart(&uart)
{
  timeout = 2000UL;
  mux = false;
}

#else

Esp8266::Esp8266(HardwareSerial &uart): m_puart(&uart)
{
  timeout = 2000UL;
  mux = false;
}

#endif

/**
 * 
 */
int Esp8266::init(){
  if(readReady())
    return 1;
  return echoOff(); 
}

const char _ready[] = "[READY]";
int Esp8266::readReady(){
  timeoutStart(5000);
  int i=0;
  static const char _r[] = "ready\r\n";
  while(true){
    if(m_puart->available() >= 7){
      if(isTimeout()){
        return -1;
      }
      int c = m_puart->read();
//      Serial.print(c,HEX);
//      Serial.print(' ');
      if(c == _r[i]){
        if(i == 6){
          Serial.println(_ready);
          return 0;
        }else{
          i++;
        }
      }else if(c == _r[0]){
        i=1;
      }else{
        i=0;
      }
      
//      int c = m_puart->read();
//      //Serial.print((char)c);
//      if(c == _r[0])
//      {
//        start:
//        c = m_puart->read();
//        //Serial.print((char)c);
//        if(c == _r[1]){
//          c = m_puart->read();
//          //Serial.print((char)c);
//          if(c == _r[2]){
//            c = m_puart->read();
//            //Serial.print((char)c);
//            if(c == _r[3]){
//              c = m_puart->read();
//              //Serial.print((char)c);
//              if(c == _r[4]){
//                c = m_puart->read();
//                if(c == _r[5]){
//                  c = m_puart->read();
//                  if(c == _r[6]){
//                    Serial.println(_ready);
//                    return 0;
//                  }
//                }
//                
//              }
//            }
//          }
//        }
//        if(c == 'r'){
//          goto start;
//        }
//      }
    }
  }
}

void Esp8266::timeoutStart(unsigned long onceTimeout){
  unsigned long now = millis();
  stopMillis = now + (onceTimeout?onceTimeout:timeout);
//  Serial.print("stop:");
//  Serial.print(stopMillis);
//  Serial.print("millis:");
//  Serial.println(now);
}

bool Esp8266::isTimeout(){
  if((long)(millis() - stopMillis) >= 0){
    Serial.println("[!Timeout]");
    return true;
  }
  return false;
}


int Esp8266::echoOff(){
  sendCmd("ATE0");
  return readResult();
}

int Esp8266::setServer(){
  if(setMux(true)){
    return 1;
  }
    
  sendCmd("AT+CIPSERVER","=1,23");
  bool result = readResult();
  if(!result) server = true;
  return result;
}

int Esp8266::setDInfo(bool on = true){
  sendCmd("AT+CIPDINFO",on?"=1":"=0");
  bool result = readResult();
  if(!result) dinfo = on;
  return result;
}


int Esp8266::setMux(bool on){
  sendCmd("AT+CIPMUX",on?"=1":"=0");
  bool result = readResult();
  if(!result) mux = on;
  return result;
}

int Esp8266::wps(){
  sendCmd("AT+WPS");
  if(readResult()){
    return 1;
  }
  
  return 0;
}

char b[] = "=0,00";
int Esp8266::sendData(const char* data,int len){
  sprintf((b + 3),"%d",len);
  sendCmd("AT+CIPSENDEX","=0,<length>");
  
}


bool Esp8266::kick(){
  clearRx();
  sendCmd("AT");
  return readResult();
}

int Esp8266::readCharIntoBuffer(){
  int c = m_puart->read();
  recvBuffer.appendChar((char)c);
  return c;
}

int Esp8266::readData(){
  int result = read();
  if(result > 0) {
    Serial.print("[unhandle message:");
    Serial.print(recvBuffer.str());
    Serial.println("]");
    recvBuffer.clear();
    // get message;
  }else if(result == 0){
    // do nothing;
  }
  else if(result < 0){
    // tcp timeout, clear tcp connection.
    Serial.println("[tcp timeout]");
  }
}



/**
 * read serail, 
 * if get something, return 1;
 * if tcp timeout, return -1;
 */
int Esp8266::read(){
  while(m_puart->available()){
    readCharIntoBuffer();
    
    if(recvBuffer.startWith("+IPD",4)){ // +IPD tcp receive
      if(parseIPD() < 0){
        recvBuffer.clear();
        return -1;
      }
      recvBuffer.clear();
      return 0;
    }
    else if(recvBuffer.endLine()){ // endline message
      recvBuffer.trimEndLine();
      if(recvBuffer.empty()){
        //Serial.println("[empty]");
      }else{
        if(check()){
          recvBuffer.clear();
          return 0;
        }else{
          //Serial.println(recvBuffer.str());
          //recvBuffer.clear();
          return 1;
        }
      }
      return 0;
    }
  }
  return 0; 
}

/**
 * check if special info.
 */
bool Esp8266::check(){
  
  if(recvBuffer.endWith(",CONNECT",8)){
    int id = atoi(recvBuffer.str());
    tcpBuffer[id].clear();
    Serial.print("[INCOM CONNECTION id=");
    Serial.print(id);
    Serial.println("]");
    return true;
  }else if(recvBuffer.endWith(",CLOSED",7)){
    int id = atoi(recvBuffer.str());
    Serial.print("[CLOSE CONNECTION id=");
    Serial.print(id);
    Serial.println("]");
    return true;
  }else if(recvBuffer.equals("wps started",11)){
    Serial.println("[WPS STARTED]");
    return true;
  }else if(recvBuffer.equals("WIFI DISCONNECT",15)){
    Serial.println("[WIFI DISCONNECT]");
    wifiConnected = false;
    return true;
  }else if(recvBuffer.equals("wps fail",8)){
    Serial.println("[WPS FAIL]");
    return true;
  }else if(recvBuffer.equals("wps success,connecting ap ...",29)){
    Serial.println("[WPS SUCCESS]");
    return true;
  }else if(recvBuffer.equals("WIFI CONNECTED",14)){
    Serial.println("[WIFI CONNECTED]");
    wifiConnected = true;
    return true;
  }else if(recvBuffer.equals("ATE0",4)){
    Serial.println("[ECHO OFF]");
    echo = false;
    return true;
  }else if(recvBuffer.equals("WIFI GOT IP",11)){
    Serial.println("[WIFI GOT IP]");
    return true;
  }
  return false;
}

bool Esp8266::isWifiConnected(void){
  return wifiConnected;
}


/**
 * read tcp data until all package receive or timeout
 * if read all package, return 0;
 * if timeout, return -1;
 */
int Esp8266::parseIPD(){
  //Serial.println("parse IPD");
  int id = 0, len = -1;
  timeoutStart(5000);
  while(len != 0){
    if(isTimeout()){
      return -1;
    }
    if(m_puart->available()){
      int c = readCharIntoBuffer();
      if(len < 0){ // parse first part.
        if(recvBuffer.endWith(":",1)){
          Serial.print("end with ':'");
          Serial.println(recvBuffer.str());
          const char* ipd = recvBuffer.nextToken(",");
          Serial.println(ipd);
          if(mux){
            auto idtext = recvBuffer.nextToken(",");
            Serial.println(idtext);
            int id = atoi(idtext);
          }
          if(dinfo){
            len = atoi(recvBuffer.nextToken(",")); // get length;
            const char* ip = recvBuffer.nextToken(",");
            const char* port = recvBuffer.nextToken(":");
            Serial.print("receive from: ip=");
            Serial.print(ip);
            Serial.print(", port=");
            Serial.println(port);
          }else{
            len = atoi(recvBuffer.nextToken(":")); // get length
          }
          Serial.print("len:");
          Serial.println(len);
        }
      }else if(len > 0){ // read given len.
        if(id==0){
          tcpBuffer[id].appendChar(c);
          len--;
        }
      }
    }
  }  
  //Serial.println(tcpBuffer[id].str());
  
  if(onTcpRecvive) onTcpRecvive(tcpBuffer[id]);

  return 0;
}

void Esp8266::clearRx(){
  while(m_puart->available()) {
    read();
  }
  recvBuffer.clear();  
}

void Esp8266::sendCmd(const char* command, const char* param){
  currentTx = command;
  m_puart->print(command);
  if(param){
    m_puart->print(param);
  }
  m_puart->print("\r\n");
}

int Esp8266::readLine(){
  do{
    if(read() > 0) return 0;
  }while(!isTimeout());
  return -1;
}



int Esp8266::readResult(){
  timeoutStart(5000);
  do{
    if(readLine() < 0) return -1;
    if(recvBuffer.equals("OK",2)){
      recvBuffer.clear();
      return 0;
    }
    if(recvBuffer.equals("ERROR",5)){
      recvBuffer.clear();
      return 1;
    }
    Serial.print("[drop line:");
    Serial.print(recvBuffer.str());
    Serial.println("]");
    int i=0;
    const unsigned char * str = recvBuffer.str();
    while(true){
      if(str[i] != 0){
        Serial.print(str[i++],HEX);
        Serial.print(' ');
      }
      else{
        Serial.println();
        break;
      }
    }
    recvBuffer.clear();
  }while(!isTimeout());
  return -1;
}





