#include "comms.h"
#include "config.h"
#include "drive.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

// Interpreta un mensaje de texto recibido de la app.
// Para añadir comandos nuevos (luces, claxon...), agrégalos aquí.
static void handleCommand(const char* msg) {
  int x, y;
  if (sscanf(msg, "J,%d,%d", &x, &y) == 2) {
    driveSetJoystick(x, y);
    return;
  }
  // Ejemplo de comando futuro:
  // int on;
  // if (sscanf(msg, "L,%d", &on) == 1) { lightsSet(on); return; }
}

static void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                      AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("[comms] App conectada (#%u)\n", client->id());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("[comms] App desconectada (#%u)\n", client->id());
      driveStop();
      break;

    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;
      // Solo mensajes de texto completos (los nuestros son muy cortos)
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        char buf[48];
        size_t n = min(len, sizeof(buf) - 1);
        memcpy(buf, data, n);
        buf[n] = '\0';
        handleCommand(buf);
      }
      break;
    }

    default:
      break;
  }
}

void commsInit() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1),     // IP del carro
                    IPAddress(192, 168, 4, 1),     // puerta de enlace
                    IPAddress(255, 255, 255, 0),
                    IPAddress(192, 168, 4, 10)); // repartir ips desde .10
  WiFi.softAP(WIFI_SSID, WIFI_PASS, WIFI_CHANNEL, 0, WIFI_MAX_CLIENTS);
  WiFi.setSleep(false);   // menos latencia
  Serial.print("[comms] Red WiFi lista. IP: ");
  Serial.println(WiFi.softAPIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    req->send(200, "text/plain", "R6Controller OK");
  });
  server.begin();
}

void commsBroadcast(const char* text) {
  ws.textAll(text);
}

bool commsHasClients() {
  return ws.count() > 0;
}

void commsCleanup() {
  ws.cleanupClients();
}
