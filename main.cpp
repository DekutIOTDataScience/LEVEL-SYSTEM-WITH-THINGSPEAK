#include "mbed.h"
#include "hcsr04.h"
#include "ESP8266.h"
HCSR04 sensor(PB_8, PB_9);
Serial pc(USBTX, USBRX);

static EventQueue eventQueue;               // An event queue
static Thread eventThread;                  // An RTOS thread to process events in


//wifi UART port and baud rate
ESP8266 wifi(D8, D2, 115200); 

//buffers for wifi library
char resp[1000];
char http_cmd[300], comm[300];

int timeout = 8000; //timeout for wifi commands

//SSID and password for connection
#define SSID "wamae" 
#define PASS "wamae6719"  

//Remote IP
#define IP "184.106.153.149"

//global variable
long distance; 

//Update key for thingspeak
char* Update_Key = "1LXHUMOQRIL6QX03";
 
//Wifi init function
void wifi_initialize(void){
    
    pc.printf("******** Resetting wifi module ********\r\n");
    wifi.Reset();
    
    //wait for 5 seconds for response, else display no response receiveed
    if (wifi.RcvReply(resp, 5000))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response");
    
    pc.printf("******** Setting Station mode of wifi with AP ********\r\n");
    wifi.SetMode(1);    // set transparent  mode
    if (wifi.RcvReply(resp, timeout))    //receive a response from ESP
        pc.printf("%s",resp);    //Print the response onscreen
    else
        pc.printf("No response while setting mode. \r\n");
    
    pc.printf("******** Joining network with SSID and PASS ********\r\n");
    wifi.Join(SSID, PASS);     
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);   
    else
        pc.printf("No response while connecting to network \r\n");
        
    pc.printf("******** Getting IP and MAC of module ********\r\n");
    wifi.GetIP(resp);     
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while getting IP \r\n");
    
    pc.printf("******** Setting WIFI UART passthrough ********\r\n");
    wifi.setTransparent();          
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while setting wifi passthrough. \r\n");
    wait(1);    
    
    pc.printf("******** Setting single connection mode ********\r\n");
    wifi.SetSingle();             
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while setting single connection \r\n");
    wait(1);
}

void wifi_send(){
    sensor.start();
    wait_ms(100); 
    long distance=sensor.get_dist_cm();
    pc.printf("distance = %dcm\n",distance);
    
    pc.printf("******** Starting TCP connection on IP and port ********\r\n");
    wifi.startTCPConn(IP,80);    //cipstart
    wifi.RcvReply(resp, timeout);
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while starting TCP connection \r\n");
    wait(0.25);
    
    //create link 
    sprintf(http_cmd,"/update?api_key=%s&field1=%d",Update_Key,distance); 
    pc.printf(http_cmd);
    
    pc.printf("******** Sending URL to wifi ********\r\n");
    wifi.sendURL(http_cmd, comm);   //cipsend and get command
    if (wifi.RcvReply(resp, timeout))    
        pc.printf("%s",resp);    
    else
        pc.printf("No response while sending URL \r\n");
}

void update_ThingSpeak()
{
        //pc.printf("Current distance is = %d \r\n", distance);
        eventQueue.call_in(500, &wifi_send);
        wait(0.25);
    
    }
    //to be called when wifi fails to initialize
void use_sdCard(){
    
    }
    //will use this in the future to initiate internet after an interrupt
//void timeout_start()
//{
    //printf("Start measuring in 5 seconds...\n");

    //eventQueue.call_in(5000, &start_measuring);
//}

    
int main() {
    pc.baud(115200);
    wifi_initialize();
      // Using an event queue is a very useful abstraction around many microcontroller 'problems', like dealing with ISRs
    // see https://developer.mbed.org/blog/entry/Simplify-your-code-with-mbed-events/
    eventThread.start(callback(&eventQueue, &EventQueue::dispatch_forever));
    // Read the moisture data every second
    //eventQueue.call_every(1000, &update_dist);
    //update the moisture value to Thingspeak Servers
    eventQueue.call_every(1000, &update_ThingSpeak);
    // Create an 'interrupt', this is an object that can fire whenever state changes however Ill use a digital pin instead of a button
    //InterruptIn btn(USER_BUTTON);
    //btn.fall(eventQueue.event(&timeout_start));
    while(1){
  wait(osWaitForever);
  }
}
