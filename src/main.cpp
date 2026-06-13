#include <WiFi.h>
#include <WebSocketsServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>
const char* ssid = "ESP32_AP";
const char* password = "12345678";

WebSocketsServer webSocket = WebSocketsServer(81);

String status = "off";
WebServer server(80);

int ledPin = 12;
int sensorNumber = 0;
int tuched=0;
bool waitingForT4 = false;
unsigned long touchTime = 0;

void handleData() {
  String json = "{";
  json += "\"number\":" + String(sensorNumber);
  json += "}";
  
  server.send(200, "application/json", json);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  
  String msg = String((char*)payload);

  StaticJsonDocument<200> doc;
  deserializeJson(doc, msg);

  String cmd = doc["cmd"];

  if (type == WStype_TEXT) {
    String msg = String((char*)payload);

    if (cmd == "led") {
      if (status== "off")
      {
         digitalWrite(ledPin, HIGH);
          status="on";
      }
      else{
          digitalWrite(ledPin, LOW);
           status="off";
      }
      String json =
"{\"status\":\"" + status +
"\",\"touch\":" + String(tuched) +
"}";
webSocket.sendTXT(num, json);      
    }
if (cmd =="reset")
{
digitalWrite(ledPin, LOW);
           status="off";
sensorNumber = 0;
tuched=0;
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

  server.on("/data", HTTP_GET, handleData);

  server.begin();
}

void loop() {

  webSocket.loop();
  server.handleClient();

  sensorNumber++;

  if (!waitingForT4 && touchRead(T3) < 40) {
    waitingForT4 = true;
    touchTime = millis();
  }

  if (waitingForT4 && millis() - touchTime >= 1000) {

    if (touchRead(T4) > 40) {
      tuched++;

      String json =
        "{\"status\":\"" + status +
        "\",\"touch\":" + String(tuched) +
        "}";

      webSocket.broadcastTXT(json); 
    }

    waitingForT4 = false;
  }
}