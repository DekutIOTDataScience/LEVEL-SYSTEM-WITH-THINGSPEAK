#ifndef ESP8266_H
#define ESP8266_H

#include <string>
#include "mbed.h"

class ESP8266
{
public:
/**
  * ESP8266 constructor
  *
  * @param tx TX pin
  * @param rx RX pin
  * @param br Baud Rate
  */
  ESP8266(PinName tx, PinName rx, int br);

  /**
  * ESP8266 destructor
  */
  ~ESP8266();

void SendCMD(char * s);
void Reset(void);
bool RcvReply(char * r, int to);
void GetList(char * l);
void Join(char * id, char * pwd);
void GetIP(char * ip);
void SetMode(char mode);
void Quit(void);
void SetSingle(void);
void SetMultiple(void);
void GetConnStatus(char * st);
void StartServerMode(int port);
void CloseServerMode(void);
void setTransparent(void);
void startTCPConn(char * IP, int port);
void sendURL(char *URL, char *command);

private:
Serial comm;
void AddEOL(char * s);
void AddChar(char * s, char c);
void itoa(int c, char s[]);

};
  
#endif
/*
    COMMAND TABLE
    Basic:
    AT: Just to generate "OK" reply
    Wifi:
    AT+RST:  restart the module
    AT+CWMODE: define wifi mode; AT+CWMODE=<mode> 1= Sta, 2= AP, 3=both; Inquiry: AT+CWMODE? or AT+CWMODE=?
    AT+CWJAP: join the AP wifi; AT+ CWJAP =<ssid>,< pwd > - ssid = ssid, pwd = wifi password, both between quotes; Inquiry: AT+ CWJAP?
    AT+CWLAP: list the AP wifi
    AT+CWQAP: quit the AP wifi; Inquiry: AT+CWQAP=?  
    * AT+CWSAP: set the parameters of AP; AT+CWSAP= <ssid>,<pwd>,<chl>,<ecn> - ssid, pwd, chl = channel, ecn = encryption; Inquiry: AT+CWJAP?
    TCP/IP:
    AT+CIPSTATUS: get the connection status
    * AT+CIPSTART: set up TCP or UDP connection 1)single connection (+CIPMUX=0) AT+CIPSTART= <type>,<addr>,<port>; 2) multiple connection (+CIPMUX=1) AT+CIPSTART= <id><type>,<addr>, <port> - id = 0-4, type = TCP/UDP, addr = IP address, port= port; Inquiry: AT+CIPSTART=?
    * AT+CIPSEND: send data; 1)single connection(+CIPMUX=0) AT+CIPSEND=<length>; 2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>; Inquiry: AT+CIPSEND=?
    * AT+CIPCLOSE: close TCP or UDP connection; AT+CIPCLOSE=<id> or AT+CIPCLOSE; Inquiry: AT+CIPCLOSE=?
    AT+CIFSR: Get IP address; Inquiry: AT+ CIFSR=?
    AT+CIPMUX:  set mutiple connection; AT+ CIPMUX=<mode> - 0 for single connection 1 for mutiple connection; Inquiry: AT+CIPMUX?
    AT+CIPSERVER: set as server; AT+ CIPSERVER= <mode>[,<port> ] - mode 0 to close server mode, mode 1 to open; port = port; Inquiry: AT+CIFSR=?
    * +IPD: received data
*/