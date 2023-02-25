//This version is for a wemos D1 mini. After assembling the pcb-s solder the wemos d1 mini to the first module, and upload code
//For configuration short the D7 pin to ground on boot, and the wifi manager will load.
//You will be presented with a wireless hotspot named "GGL ARTNET CLOCK CONFIG ME". After connecting, you can configure the wifi network that has Art-Net data
//coming from the lighting network, and set the dmx address too. Once saved, on every boot the clock will try to connect to the network you provided.

// Install all these libraries...
#include <FastLED.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <EEPROM.h>

char dmxstart[5];
int startaddress;
IPAddress ip(EEPROM.read(1),EEPROM.read(2),EEPROM.read(3),EEPROM.read(4));
IPAddress gw(EEPROM.read(5),EEPROM.read(6),EEPROM.read(7),EEPROM.read(8));
IPAddress sn(EEPROM.read(9),EEPROM.read(10),EEPROM.read(11),EEPROM.read(12));

int ipa;
int ipb;
int ipc;
int ipd;
int gwa;
int gwb;
int gwc;
int gwd;
int sna;
int snb;
int snc;
int snd;


bool shouldSaveConfig = false;

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}



#define ARTNET_DATA 0x50
#define ARTNET_POLL 0x20
#define ARTNET_POLL_REPLY 0x21
#define ARTNET_PORT 6454
#define ARTNET_HEADER 17

WiFiUDP udp;

uint8_t uniData[514];
uint16_t uniSize;
uint8_t hData[ARTNET_HEADER + 1];
uint8_t net = 0;
uint8_t universe = 0;
uint8_t subnet = 0;

#define NUM_LEDS 84
#define DATA_PIN 13 //D7
CRGB leds[NUM_LEDS];

unsigned long lastpacket = 1000;
const long packetinterval = 1000;

int dmxcol[6] = {0, 0, 0, 0, 0, 0};

int inpin = 14;  //D5  wifi reset

int d1 = 0;
int d2 = 21;
int d3 = 42;
int d4 = 63;

byte digits[10][21] = {
     {1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,0,0,0} //0 
    ,{0,0,0 ,0,0,0 ,1,1,1 ,1,1,1 ,0,0,0 ,0,0,0 ,0,0,0} //1
    ,{1,1,1 ,1,1,1 ,0,0,0 ,1,1,1 ,1,1,1 ,0,0,0 ,1,1,1} //2
    ,{0,0,0 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,0,0,0 ,1,1,1} //3
    ,{0,0,0 ,0,0,0 ,1,1,1 ,1,1,1 ,0,0,0 ,1,1,1 ,1,1,1} //4
    ,{0,0,0 ,1,1,1 ,1,1,1 ,0,0,0 ,1,1,1 ,1,1,1 ,1,1,1} //5
    ,{1,1,1 ,1,1,1 ,1,1,1 ,0,0,0 ,1,1,1 ,1,1,1 ,1,1,1} //6
    ,{0,0,0 ,0,0,0 ,1,1,1 ,1,1,1 ,1,1,1 ,0,0,0 ,0,0,0} //7
    ,{1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1} //8
    ,{0,0,0 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1 ,1,1,1} //9
   };

void setup() {
 // WiFi.mode(WIFI_STA);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed 
    FastLED.setBrightness(255);
    thchase();
  Serial.begin(115200);
  EEPROM.begin(512);
    thchase();
    pinMode(inpin, INPUT_PULLUP);      
    WiFiManager wifiManager;
    wifiManager.setClass("invert"); // dark theme
    WiFiManagerParameter custom_dmxstart("Address", "Address", dmxstart, 5);
    wifiManager.addParameter(&custom_dmxstart);
     if (digitalRead(inpin) == LOW){
        wifiManager.resetSettings(); 
        EEPROM.write(1, 2);
        EEPROM.write(2, 12);
        EEPROM.write(3, 13);
        EEPROM.write(4, 14);
        EEPROM.write(5, 2);
        EEPROM.write(6, 0);
        EEPROM.write(7, 0);
        EEPROM.write(8, 1);
        EEPROM.write(9, 255);
        EEPROM.write(10, 0);
        EEPROM.write(11, 0);
        EEPROM.write(12, 0);
        EEPROM.write(13, 5);
        EEPROM.write(14, 0);
        EEPROM.write(15, 5);
        EEPROM.commit();
      }
ipa = EEPROM.read(1);
ipb = EEPROM.read(2);
ipc = EEPROM.read(3);
ipd = EEPROM.read(4);
gwa = EEPROM.read(5);
gwb = EEPROM.read(6);
gwc = EEPROM.read(7);
gwd = EEPROM.read(8);
sna = EEPROM.read(9);
snb = EEPROM.read(10);
snc = EEPROM.read(11);
snd = EEPROM.read(12);
getstart();
wifiManager.setSaveConfigCallback(saveConfigCallback);
wifiManager.setSTAStaticIPConfig(IPAddress(ipa,ipb,ipc,ipd), IPAddress(gwa, gwb, gwc, gwd), IPAddress(sna, snb, snc, snd));
thchase();
wifiManager.setConnectTimeout(200);
wifiManager.setTimeout(200);
thchase();
wifiManager.autoConnect("GGL ARTNET CLOCK CONFIG ME");
 if (shouldSaveConfig) {   
IPAddress ip = WiFi.localIP();
IPAddress gw = WiFi.gatewayIP();
IPAddress sn = WiFi.subnetMask();
    Serial.println("saving config");
    
strcpy(dmxstart, custom_dmxstart.getValue());

EEPROM.write(1, ip[0]);
EEPROM.write(2, ip[1]);
EEPROM.write(3, ip[2]);
EEPROM.write(4, ip[3]);
EEPROM.write(5, gw[0]);
EEPROM.write(6, gw[1]);
EEPROM.write(7, gw[2]);
EEPROM.write(8, gw[3]);
EEPROM.write(9, sn[0]);
EEPROM.write(10, sn[1]);
EEPROM.write(11, sn[2]);
EEPROM.write(12, sn[3]);
EEPROM.write(13, dmxstart[0]);
EEPROM.write(14, dmxstart[1]);
EEPROM.write(15, dmxstart[2]);
Serial.println("saving address");
Serial.println(dmxstart[0]);
Serial.println(dmxstart[1]);
Serial.println(dmxstart[2]);
EEPROM.commit();
configdash();
delay(2000);
// ESP.reset();
    //end save
  }

    ESP.wdtDisable(); 
    ESP.wdtEnable(WDTO_8S);
    getstart();
    Serial.print("Starting with: ");
    Serial.println(startaddress);
    udp.begin(ARTNET_PORT); // Open ArtNet port
//    WiFi.softAPdisconnect (true);
 WiFi.softAP("GGL CLOCK", "506c616e42", 1, 0);
}

void loop() {
  ESP.wdtFeed();
  lastpacket = lastpacket + 1;
  if (lastpacket >= packetinterval){
      nosig();
  }
  
   if (udp.parsePacket()) {
    udp.read(hData, ARTNET_HEADER + 1);
    if ( hData[0] == 'A' && hData[1] == 'r' && hData[2] == 't' && hData[3] == '-' && hData[4] == 'N' && hData[5] == 'e' && hData[6] == 't') {
      if ( hData[8] == 0x00 && hData[9] == ARTNET_DATA && hData[15] == net ) {
        if ( hData[14] == (subnet << 4) + universe ) { // UNIVERSE
          uniSize = (hData[16] << 8) + (hData[17]);
          udp.read(uniData, uniSize);
        dmxcol[0] = uniData[(startaddress-1)];         
        dmxcol[1] = uniData[startaddress];         
        dmxcol[2] = uniData[(startaddress+1)];
        dmxcol[3] = uniData[(startaddress+2)];
        dmxcol[4] = uniData[(startaddress+3)];
        dmxcol[5] = uniData[(startaddress+4)];
        module(uniData[(startaddress+5)],uniData[(startaddress+6)]);
        lastpacket = 0;        
        FastLED.show();
        }
      }
    }
  } 
  delay(10);
}

void PrintNumber(byte number, int offset){
  if( number>10 )
    return;
  for(int i=0; i<21; i++){
    if( digits[number][i] )
    if ( offset >= 40){
      leds[i+offset].setRGB(dmxcol[3],dmxcol[4],dmxcol[5]);  //Switch the LED on
    }else{
      leds[i+offset].setRGB(dmxcol[0],dmxcol[1],dmxcol[2]);  //Switch the LED on
    }
      
    else
      leds[i+offset] = CRGB::Black; // Switch the LED off
  }
}

void module( int mins, int secs){
uint16_t totaltime = (mins << 8) + (secs);  
uint16_t mapval = map(totaltime, 0, 65535, 0, 3600);
int dmin = mapval/60;
int dsec = mapval - (dmin * 60);
    PrintNumber(((dmin/10)%10),d1);
    PrintNumber((dmin%10),d2);   
    PrintNumber(((dsec/10)%10),d3);
    PrintNumber((dsec%10),d4);
}
void thchase() {
    leds[18+d1] = CRGB::Red;
    leds[19+d1] = CRGB::Red;
    leds[20+d1] = CRGB::Red;
    leds[18+d2] = CRGB::Red;
    leds[19+d2] = CRGB::Red;
    leds[20+d2] = CRGB::Red;
    leds[18+d3] = CRGB::Red;
    leds[19+d3] = CRGB::Red;
    leds[20+d3] = CRGB::Red;
    leds[18+d4] = CRGB::Red;
    leds[19+d4] = CRGB::Red;
    leds[20+d4] = CRGB::Red;
    for(int p = 0; p < 18; p++) {
      leds[p+d1] = CRGB::Blue;
      leds[p+d1].maximizeBrightness();  
      leds[p+d2] = CRGB::Blue;
      leds[p+d2].maximizeBrightness();
      leds[p+d3] = CRGB::Blue;
      leds[p+d3].maximizeBrightness();
      leds[p+d4] = CRGB::Blue;
      leds[p+d4].maximizeBrightness();
      FastLED.show();
      FastLED.delay(30);
      leds[p+d1].setRGB(200,50,0);
      leds[p+d1].fadeLightBy( 128 );
      leds[p+d2].setRGB(200,50,0);
      leds[p+d2].fadeLightBy( 128 );
      leds[p+d3].setRGB(200,50,0);
      leds[p+d3].fadeLightBy( 128 );
      leds[p+d4].setRGB(200,50,0);
      leds[p+d4].fadeLightBy( 128 );
     }
}
void nosig(){
  for(int l = 0; l < 84; l++) {
      leds[l] = CRGB::Black;
  }
  for(int n = 18; n < 21; n++) {
      leds[n+d1] = CRGB::Red;
      leds[n+d2] = CRGB::Red;
      leds[n+d3] = CRGB::Red;
      leds[n+d4] = CRGB::Red;
     } 
      FastLED.show();
}
void configdash(){
  for(int l = 0; l < 84; l++) {
      leds[l] = CRGB::Black;
  }
  for(int n = 18; n < 21; n++) {
      leds[n+d2] = CRGB::Green;
      leds[n+d3] = CRGB::Green;
     } 
      FastLED.show();
}
void getstart(){
  char buffer[4];
buffer[0] = EEPROM.read(13);
buffer[1] = EEPROM.read(14);
buffer[2] = EEPROM.read(15);
buffer[3] = '\0';
startaddress = atoi(buffer); 
}
