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
#define NUM_LEDS 900
#define PIN2 3
#define NUM_LEDS2 300

//ACTIVE STRIPS
boolean isActiveStrip1 = true;
boolean isActiveStrip2 = true;


//CREATING STRIPS
CRGB strip1[NUM_LEDS];
CRGB strip2[NUM_LEDS2];


//STRIP EVENT
int strip1Event=1;
int strip2Event=1;


//STRIP RGBS
int r1 = 255, g1 = 255, b1 = 0;
int r2 = 255, g2 = 255, b2 = 255;


//STRIP COUNTERS
int strip1Count;
int strip2Count;

int strip1CountJ;
int strip2CountJ;


//VELOCITY
int Speed = 5;

boolean blinkState=false;
int commandCount=0; 
void serialReadCall() {
  if (Serial2.available() > 0) {
    String recivedDataStr = "";
    char recivedChar;
    
    noInterrupts();
    while (Serial2.available() > 0 && commandCount < 1 ) {
      recivedChar = Serial2.read();
      if (recivedChar != '\n') {
        // Concatena valores
        if(commandCount <1 ){
          recivedDataStr+=recivedChar;  
        }
        if(recivedChar=='|'){
          commandCount++;
          }
        
        
        delay(15);
      }
    }
    interrupts();

    delay(10);

    Serial.println(recivedDataStr);
//    if (recivedDataStr.indexOf("$") >= 0) {
//       Serial.println("MAGAL Device");
//    }
    //LEITURA RGB #R255G255B255S0
     if (recivedDataStr.indexOf("#") >= 0) {
      String r="",g="",b="";
      r=recivedDataStr.substring(recivedDataStr.indexOf("R")+1,recivedDataStr.indexOf("G"));
      g=recivedDataStr.substring(recivedDataStr.indexOf("G")+1,recivedDataStr.indexOf("B"));
      b=recivedDataStr.substring(recivedDataStr.indexOf("B")+1,recivedDataStr.indexOf("S"));
      String s="";
      s=recivedDataStr.substring(recivedDataStr.indexOf("S")+1,recivedDataStr.indexOf("|"));
      if(s.length()<1 ){
        s=5;
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


    }//FIM DA LEITURA RGB

    //LEITURA DE EVENTO
    if (recivedDataStr.indexOf("@") >= 0) {
      int indexId = recivedDataStr.indexOf("@");
      int indexD = recivedDataStr.indexOf("D");
      int indexS = recivedDataStr.indexOf("S");
      int indexSize = recivedDataStr.length();
      int str_len = recivedDataStr.length() + 1;
      char char_array[str_len];
      recivedDataStr.toCharArray(char_array, str_len);
      String Speed = "";
      String idEvent = "";
      String idStrip = "";

      for (int cont = indexId + 1; cont < indexD; cont++) {
        idEvent.concat(char_array[cont]);

      }

      for (int cont = indexD + 1; cont < indexS; cont++) {
        Speed.concat(char_array[cont]);

      }

      for (int cont = indexS + 1; cont < indexSize; cont++) {
        idStrip.concat(char_array[cont]);

      }
      int ID = idEvent.toInt();

      int idStri = idStrip.toInt();


      if (idStri == 1) {
        strip1Event = ID;
        strip1Count = 0;
        strip1CountJ = 0;
      }
      else if (idStri == 2) {
        strip2Event = ID;
        strip2Count = 0;
        strip2CountJ = 0;
      }
      else if (idStri == 0) {
        strip1Event = ID;
        strip2Event = ID;


        strip1Count = 0;
        strip2Count = 0;


        strip1CountJ = 0;
        strip2CountJ = 0;

      }
    }
    //FIM EVENTO READ

  }
    commandCount=0;

}

void lightCall() {

if(strip1Event==0)
  simpleRainbow(0, 1);
else if(strip1Event==1)
  setAll(r1, g1, b1, 1);
else if(strip1Event==2)
  blinkLed();


  //STRIP 2 EVENT
if(strip2Event==0)
  simpleRainbow(0, 2);
else if(strip2Event==1)
  setAll(r2, g2, b2, 2);

}

//SETUP INFORMATIONS
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  Serial.println("Teensy ON");
  Serial.println();

  //INITIALIZE ALL STRIPS
  FastLED.addLeds<WS2812B, PIN, GRB>(strip1, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<WS2812B, PIN2, GRB>(strip2, NUM_LEDS2).setCorrection( TypicalLEDStrip );

  //ADDING THREADS
  threadRead.onRun(serialReadCall);
  threadRead.setInterval(1);

  threadLight.onRun(lightCall);
  threadLight.setInterval(1);

  controll.add(&threadRead);
  controll.add(&threadLight);

}

void loop() {
  controll.run();
  yield();
}

//SHOW SPECIFC STRIP
void showStrip() {
  FastLED.show();
  yield();
}

//SET A SPECIFC PIXEL INTO A SPECIFC STRIP
void setPixel(int Pixel, byte red, byte green, byte blue, int stripID) {
  yield();
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
  yield();
  int numberLeds;
  if (stripID == 1) {
    numberLeds = NUM_LEDS;
  }
  else if (stripID == 2) {
    numberLeds = NUM_LEDS2;
  }
  for (int i = 0; i < numberLeds; i++ ) {
    yield();
    setPixel(i, red, green, blue, stripID);
  }
  showStrip();
}

//LED EFFECTS ------------------------------------------------------------------------------------------------- x x x x x x x x x x x x ---------------------------------------------------------------------------------------------------------------

//WHEEL FOR RAINBOW
byte * Wheel(byte WheelPos) {
  static byte c[3];
  yield();
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
      yield();
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
