#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

const char* accessPointSSID = "Sala1307";
const char* accessPointPassword = "Senhateste";

IPAddress local_IP(192,168,1,22);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);

AsyncWebServer webServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume... Restarting in 5 seconds!");
    delay(5000);
    ESP.restart();
  }
}

void initAccessPoint() {
  Serial.print("Setting up Access Point ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Starting Access Point ... ");
  Serial.println(WiFi.softAP(accessPointSSID, accessPointPassword) ? "Ready" : "Failed!");

  Serial.print("IP address = ");
  Serial.println(WiFi.softAPIP());
}

void initWebServer() {
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {    // define here wat the webserver needs to do
    request->send(SPIFFS, "/webpage.html", "text/html");           
  });

  webServer.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found. Just '/' endpoint exists!");
  });

  webServer.serveStatic("/", SPIFFS, "/");
}

void initWebSocket() {
  webSocket.begin();                                 
  webSocket.onEvent = onWebSocketEvent;

  webServer.begin();
}

void setup() {  
  Serial.begin(115200);
  delay(500);

  initSPIFFS();
  initAccessPoint();
  initWebServer();
  delay(500);
  initWebSocket();
}

void loop() {
  webSocket.loop();
}

void sendCurrentStatus() {
  Serial.println("Send Current Status to WEB -> Não implementado");
}

void handleValidEvents(char* event_type, int event_value) {
  switch(event_type) {
    case "isRegistrationMode":
      Serial.print("Set Registration Mode");
      Serial.println(event_value == 1);
      break;
    case "isDoorOpen":
      Serial.print("Set Door Status");
      Serial.println(event_value == 1);
      break;
    case "getStatus":
      sendCurrentStatus();
      break;
  };
}

void onWebSocketEvent(byte clientId, WStype_t eventType, uint8_t *payload) {
  Serial.print("New WebSocket Event received from client with id " + String(clientId) + " -> ");
  switch (eventType) {
    case WStype_DISCONNECTED:
      Serial.println("Client with ID" + String(clientId) + " disconnected now!");
      break;
    case WStype_CONNECTED:
      Serial.println("Client with ID " + String(clientId) + " connected now!");
      break;
    case WStype_TEXT:
      handleWebSocketTextEvent(payload);
      break;
  };
};

void handleWebSocketTextEvent(payload) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("Error on deserialize Json content: "));
    Serial.println(error.f_str());
    return;
  }
  
  const char* event_type = doc["eventType"];
  const int event_value = doc["value"];

  Serial.println("Type: " + String(event_type));
  Serial.println("Value: " + String(event_value));

  handleValidEvents(event_type, event_value);
  return;
}
