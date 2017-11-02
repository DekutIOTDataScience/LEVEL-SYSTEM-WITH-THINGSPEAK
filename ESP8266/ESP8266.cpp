#include "ESP8266.h"
#define HTTPCMD "GET "
#define protocol " HTTP/1.0\n\n"


// Constructor
ESP8266::ESP8266(PinName tx, PinName rx, int br) : comm(tx, rx) {
    comm.baud(br);
}

// Destructor
ESP8266::~ESP8266() { }

// Add <CR> + <LF> at the end of the string
void ESP8266::AddEOL(char * s) {
    char k;
    k = strlen(s); // Finds position of NULL character
    s[k] = 0x0D; // switch NULL for <CR>
    s[k + 1] = 0x0A; // Add <LF>
    s[k + 2] = 0; // Add NULL at the end
}

// Add one ASCII character at the end of the string
void ESP8266::AddChar(char * s, char c) {
    char k;
    k = strlen(s);
    s[k] = c;
    s[k + 1] = 0;
}

// Converts integer number to null-terminated string
void ESP8266::itoa(int n, char * s) {
    char k = 0;
    char r[11];
    
    if(n == 0) {
        s[0] = '0';
        s[1] = 0;
    } else {
        while(n != 0) {
            r[k]= (n % 10) + '0';
            n = n / 10;
            k++;
        }
        while(k > 0) {
            s[n] = r[k - 1] + '0';
            n++;
            k--;
        }
        s[n] = 0;
    }
}

// Sends command to ESP8266. Receives the command string
void ESP8266::SendCMD(char * s) {
    AddEOL(s);
    comm.printf("%s", s);
}

// Resets the ESP8266
void ESP8266::Reset(void) {
    char rs[10];
    strcpy(rs, "AT+RST");
    SendCMD(rs);
}

// Receive reply until no character is received after a given timeout in miliseconds
bool ESP8266::RcvReply(char * r, int to) {
    Timer t;
    bool ended = 0;
    char c;
    
    strcpy(r, "");
    t.start();
    while(!ended) {
        if(comm.readable()) {
            c = comm.getc();
            AddChar(r, c);
            t.start();
        }
        if(t.read_ms() > to) {
                ended = 1;
        }
    }
    AddChar(r, 0x00);
    return ended;
}

// Gets the AP list. Parameter: the string to receive the list
void ESP8266::GetList(char * l) {
    char rs[15];
    strcpy(rs, "AT+CWLAP");
    SendCMD(rs);
    RcvReply(l, 5000); // Needs big timeout because it takes long to start replying
}

// Joins a Wifi AP. Parameters: SSID and Password (strings)
void ESP8266::Join(char * id, char * pwd) {
    char cmd[255];
    strcpy(cmd, "AT+CWJAP=");
    AddChar(cmd, 0x22);
    strcat(cmd, id);
    AddChar(cmd, 0x22);
    AddChar(cmd, 0x2C);
    AddChar(cmd, 0x22);
    strcat(cmd, pwd);
    AddChar(cmd, 0x22);
    SendCMD(cmd);
}

// Gets ESP IP. Parameter: string to contain IP
void ESP8266::GetIP(char * ip) {
    char cmd[15];
    strcpy(cmd, "AT+CIFSR");
    SendCMD(cmd);
    RcvReply(ip, 2000);
}

//Defines wifi mode; Parameter: mode; 1= STA, 2= AP, 3=both
void ESP8266::SetMode(char mode) {
    char cmd[15];
    strcpy(cmd, "AT+CWMODE=");
    mode = mode + 0x30; // Converts number into corresponding ASCII character
    AddChar(cmd, mode); // Completes command string
    SendCMD(cmd);
}

// Quits the AP
void ESP8266::Quit(void) {
    char cmd[15];
    strcpy(cmd, "AT+CWQAP");
    SendCMD(cmd);
}

// Sets single connection
void ESP8266::SetSingle(void) {
    char cmd[15];
    strcpy(cmd, "AT+CIPMUX=0");
    SendCMD(cmd);
}

// Sets multiple connection
void ESP8266::SetMultiple(void) {
    char rs[15];
    strcpy(rs, "AT+CIPMUX=1");
    SendCMD(rs);
}

// Gets connection status. Parameter: string to contain status
void ESP8266::GetConnStatus(char * st) {
    char cmd[15];
    strcpy(cmd, "AT+CIPSTATUS");
    SendCMD(cmd);
    RcvReply(st, 2000);
}

// Starts server mode. Parameter: port to be used
void ESP8266::StartServerMode(int port) {
    char rs[25];
    char t[4];
    strcpy(rs, "AT+CIPSERVER=1,");
    itoa(port, t);
    strcat(rs, t);
    SendCMD(rs);
}

// Close server mode.
void ESP8266::CloseServerMode(void) {
    char rs[20];
    strcpy(rs, "AT+CIPSERVER=0");
    SendCMD(rs);
}

void ESP8266::setTransparent(void){
    char rs[20];
    strcpy(rs, "AT+CIPMODE=0");
    SendCMD(rs);
}

void ESP8266::startTCPConn(char *IP, int port){
    char rs[100];
    sprintf(rs, "AT+CIPSTART=\"TCP\",\"%s\",%d", IP, port);
    SendCMD(rs);
}

void ESP8266::sendURL(char *URL, char *command){
    char url[300], snd[300], http_cmd[300];
    
    strcpy(http_cmd, HTTPCMD);
    
    strcat(http_cmd, URL);
    strcat(http_cmd, protocol);
    
    strcpy(url, http_cmd);
    sprintf(snd,"AT+CIPSENDEX=%d",strlen(url));
    strcpy(command, url);
    SendCMD(snd);
    wait(3);
    SendCMD(url);
}
    
    
