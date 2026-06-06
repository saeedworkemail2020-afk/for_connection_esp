#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";

WebSocketsServer webSocket = WebSocketsServer(81);

String status = "off";

int ledPin = 12;
int counter = 0;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  
  // counter++;
  //     String text = "Message " + String(counter);
  //     webSocket.sendTXT(num, text);
  Serial.println(num);
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);

    if (msg == "led") {
      if (status== "off")
      {
         digitalWrite(ledPin, HIGH);
          status="on";
      }
      else{
          digitalWrite(ledPin, LOW);
           status="off";
      }
        webSocket.sendTXT(num, status);
      
    }

  }
}

void setup() {
  Serial.begin(921600);

  pinMode(ledPin, OUTPUT);

  WiFi.softAP(ssid, password);

  Serial.println("AP Started");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
   
}

void loop() {
  webSocket.loop();

}