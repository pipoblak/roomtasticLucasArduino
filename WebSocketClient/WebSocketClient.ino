#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

#include <Hash.h>

#include <SoftwareSerial.h>

SoftwareSerial USE_SERIAL(D1, D2); // RX, TX

WebSocketsClient webSocket;
long msSend;
long ms;
String owner="lucas";

//Wifi Settings
const char* ssid     = "QG BOLADAO";
const char* password = "boladomesmo";

#define RELAY D1
#define COOLER D5


boolean power;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {


    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
				
			    // send message to server when Connected
				//webSocket.sendTXT("Connected");
            }
            break;
        case WStype_TEXT:
            USE_SERIAL.println((char*) payload);
            Serial.println((char*) payload);
            if(payload[0]=='$'){
              webSocket.sendTXT("{'Device':{'id':'0','name':'powerController','macAddress':'"+WiFi.macAddress()+"','owner':'"+ owner + "'}}");
            }
            if(payload[0]=='!'){
              msSend=millis();
              webSocket.sendTXT("ping");
            }
            if(payload[0]=='p'){
              ms=millis() - msSend;
              String ping = String(ms);
              Serial.println(ping + " ms" );
              webSocket.sendTXT(ping + " ms" );
            }
           else if(payload[0]=='0'){
            Serial.println("HIGH");
            digitalWrite(RELAY, HIGH);
            }
           else if(payload[0]=='1'){
            Serial.println("LOW");
            digitalWrite(RELAY, LOW);
            }
           else if(payload[0]=='%'){
            String command = (char*)payload;
            char char_array[lenght];
            command.toCharArray(char_array, lenght + 1);
            int percentage = command.substring(1).toInt();
            Serial.println((percentage*999)/100);
            analogWrite(COOLER,(percentage*999)/100);
            
            }

            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary lenght: %u\n", lenght);
            hexdump(payload, lenght);

            // send data to server
            // webSocket.sendBIN(payload, lenght);
            break;
    }

}

void setup() {
    // USE_SERIAL.begin(921600);
    Serial.begin(115200);
    USE_SERIAL.begin(115200);


    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();


    //INITIALIZING WIFI
      USE_SERIAL.println();
      USE_SERIAL.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);


    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      USE_SERIAL.print(".");
    }


    webSocket.begin("192.168.15.21", 82);
    USE_SERIAL.print(WiFi.macAddress());
    webSocket.onEvent(webSocketEvent);
    pinMode(RELAY,OUTPUT);
    pinMode(COOLER, OUTPUT);

}

void loop() {
    webSocket.loop();
}
