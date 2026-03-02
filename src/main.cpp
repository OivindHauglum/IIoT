#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <Adafruit_VL53L0X.h>

namespace Config {
constexpr const char* kApSsid = "IIoT-ESP32-C6";
constexpr const char* kApPassword = "iiotclass";
constexpr uint16_t kWebPort = 80;
constexpr uint16_t kWsPort = 81;
constexpr uint8_t kPotPin = A0;
constexpr uint32_t kSamplePeriodMs = 100;
constexpr float kDefaultAlpha = 0.2f;
}  // namespace Config

struct SensorPacket {
  uint16_t potRaw;
  uint16_t potFilt;
  uint16_t tofRaw;
  uint16_t tofFilt;
};

WebServer server(Config::kWebPort);
WebSocketsServer webSocket(Config::kWsPort);
Adafruit_VL53L0X lox;

float gAlpha = Config::kDefaultAlpha;
float gPotFiltered = 0.0f;
float gTofFiltered = 0.0f;
bool gTofAvailable = false;
uint32_t gLastSample = 0;

float applyIir(float previous, float input) {
  return (gAlpha * input) + ((1.0f - gAlpha) * previous);
}

uint16_t readTofMm() {
  if (!gTofAvailable) {
    return 0;
  }

  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    return static_cast<uint16_t>(measure.RangeMilliMeter);
  }

  return 0;
}

String makePayload(const SensorPacket& packet) {
  String json = "{";
  json += "\"potRaw\":" + String(packet.potRaw) + ",";
  json += "\"potFilt\":" + String(packet.potFilt) + ",";
  json += "\"tofRaw\":" + String(packet.tofRaw) + ",";
  json += "\"tofFilt\":" + String(packet.tofFilt);
  json += "}";
  return json;
}

void handleRoot() {
  File page = SPIFFS.open("/index.html", "r");
  if (!page) {
    server.send(500, "text/plain", "index.html not found");
    return;
  }

  server.streamFile(page, "text/html");
  page.close();
}

void handleAlphaGet() {
  String response = "{";
  response += "\"alpha\":" + String(gAlpha, 3);
  response += "}";
  server.send(200, "application/json", response);
}

void handleAlphaPost() {
  if (!server.hasArg("value")) {
    server.send(400, "application/json", "{\"error\":\"missing value\"}");
    return;
  }

  float requested = server.arg("value").toFloat();
  if (requested <= 0.0f || requested > 1.0f) {
    server.send(400, "application/json", "{\"error\":\"value must be in (0,1]\"}");
    return;
  }

  gAlpha = requested;
  handleAlphaGet();
}

void handleNotFound() {
  server.send(404, "application/json", "{\"error\":\"not found\"}");
}

void setupRoutes() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/alpha", HTTP_GET, handleAlphaGet);
  server.on("/alpha", HTTP_POST, handleAlphaPost);
  server.onNotFound(handleNotFound);
}

void onWebSocketEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.printf("WS client %u connected\n", clientId);
      break;
    case WStype_DISCONNECTED:
      Serial.printf("WS client %u disconnected\n", clientId);
      break;
    case WStype_TEXT:
      if (length > 0) {
        String cmd(reinterpret_cast<char*>(payload));
        cmd.trim();
        if (cmd == "ping") {
          webSocket.sendTXT(clientId, "pong");
        }
      }
      break;
    default:
      break;
  }
}

SensorPacket acquireAndProcess() {
  const uint16_t potRaw = analogRead(Config::kPotPin);
  const uint16_t tofRaw = readTofMm();

  gPotFiltered = applyIir(gPotFiltered, static_cast<float>(potRaw));
  gTofFiltered = applyIir(gTofFiltered, static_cast<float>(tofRaw));

  SensorPacket packet{};
  packet.potRaw = potRaw;
  packet.potFilt = static_cast<uint16_t>(gPotFiltered);
  packet.tofRaw = tofRaw;
  packet.tofFilt = static_cast<uint16_t>(gTofFiltered);
  return packet;
}

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);

  Wire.begin();
  gTofAvailable = lox.begin();

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(Config::kApSsid, Config::kApPassword);

  Serial.println("Access Point started");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  setupRoutes();
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  Serial.println("HTTP + WS online");
}

void loop() {
  server.handleClient();
  webSocket.loop();

  const uint32_t now = millis();
  if (now - gLastSample >= Config::kSamplePeriodMs) {
    gLastSample = now;

    SensorPacket packet = acquireAndProcess();
    String payload = makePayload(packet);

    webSocket.broadcastTXT(payload);

    Serial.print("TX: ");
    Serial.println(payload);
  }
}
