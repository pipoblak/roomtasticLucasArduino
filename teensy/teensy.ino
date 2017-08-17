#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include <Arduino.h>
#include <Thread.h>
#include <ThreadController.h>

//THREADS
ThreadController controll = ThreadController();
Thread threadRead = Thread();
Thread threadLight = Thread();

//SETTING STRIPS
#define PIN 2
#define NUM_LEDS 300
#define PIN2 3
#define NUM_LEDS2 300

//ACTIVE STRIPS
boolean isActiveStrip1 = true;
boolean isActiveStrip2 = true;

boolean strip1On = true;
boolean strip2On = true;

//CREATING STRIPS
CRGB strip1[NUM_LEDS];
CRGB strip2[NUM_LEDS2];


//STRIP EVENT
int strip1Event=0;
int strip2Event=0;


//STRIP RGBS
int r1 = 255, g1 = 255, b1 = 0;
int r2 = 255, g2 = 255, b2 = 0;


//STRIP COUNTERS
int strip1Count;
int strip2Count;

int strip1CountJ;
int strip2CountJ;


//VELOCITY
int Speed = 5;

boolean blinkState=false;
int commandCount=0; 
boolean canRun=false;
String recivedDataStr = "";
void serialReadCall() {
 
  if (Serial2.available() > 0) {
    
    char recivedChar;
    while (Serial2.available() > 0) {
      recivedChar = Serial2.read();
      if(commandCount <1 ){
        recivedDataStr+=recivedChar;  
      }
      if(recivedChar=='|'){
        commandCount++;
      }
          
    }
    if((recivedDataStr.indexOf("B")<0 || recivedDataStr.indexOf("S") <0 || recivedDataStr.indexOf("|")<0) && recivedDataStr.indexOf(";")<0){
      canRun=false;   
    } 
    if(recivedDataStr.lastIndexOf("|")>1){
      canRun=true;
    }
    
     if (recivedDataStr.lastIndexOf("#") >= 0 && canRun) {
      Serial.println(recivedDataStr);
      String r="",g="",b="";
      r=recivedDataStr.substring(recivedDataStr.lastIndexOf("R")+1,recivedDataStr.lastIndexOf("G"));
      g=recivedDataStr.substring(recivedDataStr.lastIndexOf("G")+1,recivedDataStr.lastIndexOf("B"));
      b=recivedDataStr.substring(recivedDataStr.lastIndexOf("B")+1,recivedDataStr.lastIndexOf("S"));
      String s="";
      s=recivedDataStr.substring(recivedDataStr.lastIndexOf("S")+1,recivedDataStr.lastIndexOf("|"));
      if(s.length()<1 ){
        s="5";
      }
      int idStri = s.toInt();
      Serial.println("RED 1: " + r + " Green1: " + g + " Blue1: " + b + " Strip: " + s );

      if (idStri == 0 ) {
        r1 = r.toInt();
        g1 = g.toInt();
        b1 = b.toInt();
        r2 = r.toInt();
        g2 = g.toInt();
        b2 = b.toInt();

      }

      if (idStri == 1) {
        r1 = r.toInt();
        g1 = g.toInt();
        b1 = b.toInt();
      }
      else if (idStri == 2) {
        r2 = r.toInt();
        g2 = g.toInt();
        b2 = b.toInt();

      }
      recivedDataStr="";


    }//FIM DA LEITURA RGB

    //LEITURA DE EVENTO
    if (recivedDataStr.lastIndexOf(";") >= 0 && canRun) {
      Serial.println(recivedDataStr);
      String idEvent = "";
      String idStrip = "";
      idEvent=recivedDataStr.substring(recivedDataStr.lastIndexOf(";")+1,recivedDataStr.lastIndexOf("S"));
      idStrip=recivedDataStr.substring(recivedDataStr.lastIndexOf("S")+1,recivedDataStr.lastIndexOf("|"));
      int ID = idEvent.toInt();
      int idStri = idStrip.toInt();

      if (idStri == 1) {
        if(ID==3){
          if(!strip1On){
            ID=0;
            strip1On=true;
          }
          else{
            strip1On=false;
            }
        }
        
        strip1Event = ID;
        strip1Count = 0;
        strip1CountJ = 0;
        
      }
      else if (idStri == 2) {
        if(ID==3){
          if(!strip2On){
            ID=0;
            strip2On=true;
            }
          else{
            strip2On=false;
            }
        }       
        strip2Event = ID;
        strip2Count = 0;
        strip2CountJ = 0;
      }
      else if (idStri == 0) {
        if(ID==3){
          if(!strip1On || !strip2On){
            ID=0;
            strip1On=true;
            strip2On=true;
          }
          else{
            strip1On=false;
            strip2On=false;
            }
          
        }
        strip1Event = ID;
        strip2Event = ID;


        strip1Count = 0;
        strip2Count = 0;


        strip1CountJ = 0;
        strip2CountJ = 0;

      }
      recivedDataStr="";
    }
    //FIM EVENTO READ

  }

  commandCount=0;

}

void lightCall() {

if(strip1Event==2)
  simpleRainbow(0, 1);
else if(strip1Event==0)
  setAll(r1, g1, b1, 1);
else if(strip1Event==1)
  setAll(r1/2, g1/2, b1/2, 1);
else if(strip1Event==3)
  setAll(0, 0, 0, 1);


//STRIP 2 EVENT
if(strip2Event==2)
  simpleRainbow(0, 2);
else if(strip2Event==0)
  setAll(r2, g2, b2, 2);
else if(strip2Event==1)
  setAll(r2/2, g2/2, b2/2, 2);
else if(strip2Event==3)
  setAll(0, 0, 0, 2);

}

//SETUP INFORMATIONS
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial.println("Teensy ON");
  Serial.println();
  //INITIALIZE ALL STRIPS
  FastLED.addLeds<WS2812B, PIN, GRB>(strip1, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, PIN2, GRB>(strip2, NUM_LEDS2).setCorrection( TypicalLEDStrip );

  //ADDING THREADS
  threadRead.onRun(serialReadCall);
  threadRead.setInterval(1);

  threadLight.onRun(lightCall);
  threadLight.setInterval(200);

  controll.add(&threadRead);
  controll.add(&threadLight);

}

void loop() {
  controll.run();
}

//SHOW SPECIFC STRIP
void showStrip() {
  FastLED.show();
}

//SET A SPECIFC PIXEL INTO A SPECIFC STRIP
void setPixel(int Pixel, byte red, byte green, byte blue, int stripID) {
  if (stripID == 1) {
    strip1[Pixel].r = red;
    strip1[Pixel].g = green;
    strip1[Pixel].b = blue;
  }
  else if (stripID == 2) {
    strip2[Pixel].r = red;
    strip2[Pixel].g = green;
    strip2[Pixel].b = blue;
  }
}

//SET ALL PIXELS TO A SPECIFC STRIP
void setAll(byte red, byte green, byte blue, int stripID) {
  int numberLeds;
  if (stripID == 1) {
    numberLeds = NUM_LEDS;
  }
  else if (stripID == 2) {
    numberLeds = NUM_LEDS2;
  }
  for (int i = 0; i < numberLeds; i++ ) {
    setPixel(i, red, green, blue, stripID);
  }
  showStrip();
}

//LED EFFECTS ------------------------------------------------------------------------------------------------- x x x x x x x x x x x x ---------------------------------------------------------------------------------------------------------------

//WHEEL FOR RAINBOW
byte * Wheel(byte WheelPos) {
  static byte c[3];
  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }
  return c;
}


void simpleRainbow(int SpeedDelay, int stripID) {
  byte *c;
  int contTempo;
  int numberLeds;
  int cont;

  if (stripID == 1) {
    contTempo = strip1Count;
    strip1Count++;
    numberLeds = NUM_LEDS;
    cont = strip1CountJ;
  }
  else if (stripID == 2) {
    contTempo = strip2Count;
    strip2Count++;
    numberLeds = NUM_LEDS2;
    cont = strip2CountJ;
  }

  if (contTempo >= SpeedDelay) {
    for (int i = 0; i < numberLeds; i++) {
      c = Wheel(((i * 256 / numberLeds) + cont) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2), stripID);
    }

    contTempo = 0;
    if (stripID == 1) {
      strip1Count = contTempo;
      strip1CountJ++;
    }
    else if (stripID == 2) {
      strip2Count = contTempo;
      strip2CountJ++;
    }
    showStrip();
  }


}

void blinkLed(){
  if(strip1Count<millis()){
    if(blinkState){
      setAll(0,0,0,1);
      setAll(0,0,0,2);
      blinkState=false;
    }
    else{
      setAll(r1,g1,b1,1);
      setAll(r2,g2,b2,2);
      blinkState=true;
      }
   
   strip1Count=millis() + 500;
  }

}
