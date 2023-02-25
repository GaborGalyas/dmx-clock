//This code is written for an arduino mini with wired dmx input. 
//This will need tinkering, you need to connect the arduino to the modules and implement the dmx receiving circuit.
//I will work on making a pcb design for a drop in replacement for the wemos d1 mini.


#include <FastLED.h>                     //You need to have this library installed
#include <Conceptinetics.h>              //You need to have this library installed
#define DMX_SLAVE_CHANNELS   8 
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );
int startaddress = 501;                  //For now, here is the fixed DMX address, still working on RDM support

#define NUM_LEDS 84
#define DATA_PIN 3                      //Data pin connected to panels
CRGB leds[NUM_LEDS];

unsigned long lastpacket = 1000;
const long packetinterval = 1000;

int dmxcol[6] = {0, 0, 0, 0, 0, 0};

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
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed 
    FastLED.setBrightness(255);
  FastLED.setMaxPowerInMilliWatts(5000);               //Maximum power for the display
    dmx_slave.enable ();  
    dmx_slave.setStartAddress (startaddress);
   thchase();
   thchase();
}
   

void loop() {
        dmxcol[0] = dmx_slave.getChannelValue (1);         
        dmxcol[1] = dmx_slave.getChannelValue (2);         
        dmxcol[2] = dmx_slave.getChannelValue (3);
        dmxcol[3] = dmx_slave.getChannelValue (4);
        dmxcol[4] = dmx_slave.getChannelValue (5);
        dmxcol[5] = dmx_slave.getChannelValue (6);
        module(dmx_slave.getChannelValue (7),dmx_slave.getChannelValue (8));      
        FastLED.show();
        delay(100);
}
void PrintNumber(byte number, int offset){
  if( number>10 )
    return;
  for(int i=0; i<21; i++){
    if( digits[number][i] )
    if ( offset >= 40){
      leds[i+offset].setRGB(dmxcol[3],dmxcol[4],dmxcol[5]);  
    }else{
      leds[i+offset].setRGB(dmxcol[0],dmxcol[1],dmxcol[2]);  
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
