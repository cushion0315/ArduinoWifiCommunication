/*
  Esp8266.h - Library for flashing Morse code.
  Created by CuSHiON Chen, December 29, 2017.
  Released into the public domain.
*/
#ifndef Esp8266_h
#define Esp8266_h

#define ESP8266_USE_SOFTWARE_SERIAL

#ifdef ESP8266_USE_SOFTWARE_SERIAL
#include "SoftwareSerial.h"
#endif

#include "Arduino.h"
#include "Buffer.h"

const int _SERVER_PORT = 23;

class Esp8266
{
  public:
#ifdef ESP8266_USE_SOFTWARE_SERIAL
    Esp8266(SoftwareSerial &uart);
#else 
    Esp8266(HardwareSerial &uart);
#endif
    /**
     * init esp8266 serial
     * read serial until "ready" show up or read timeout.
     */
    int init(void);
    bool kick(void);
    int setMux(bool on = true);
    int setDInfo(bool on = true);
    int readData();
    //int send(const char* ,int);
    int sendData(const char*,int);
    int setServer(void);
    int wps(void);
    bool ready(void);
    bool isWifiConnected(void);
    unsigned long timeout;
    bool echo;
    void (*onTcpRecvive)(Buffer);
  private:
    Buffer recvBuffer;
    Buffer tcpBuffer[1];
    const char* currentTx;
    bool mux;
    bool dinfo;
    bool server;
    bool wifiConnected;
    /**
     * read tcp data until all package receive or timeout
     * if read all package, return 0;
     * if timeout, return -1;
     */
    int parseIPD();
    void clearRx();
    int echoOff();
    int readReady();
    bool check();
    unsigned long stopMillis;
    void timeoutStart(unsigned long onceTimeout = 0UL);
    bool isTimeout();
    void sendCmd(const char* command, const char* param = NULL);
    int readCharIntoBuffer();
    /**
     * read serail, 
     * if get something, return 1;
     * if tcp timeout, return -1;
     * this is the base function
     */
    int read(void);
    int readLine();
    int readResult();
    bool echoMode();
    
    
#ifdef ESP8266_USE_SOFTWARE_SERIAL
    SoftwareSerial *m_puart; /* The UART to communicate with ESP8266 */
#else
    HardwareSerial *m_puart; /* The UART to communicate with ESP8266 */
#endif

};

#endif
