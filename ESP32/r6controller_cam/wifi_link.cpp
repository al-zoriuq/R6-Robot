#include "wifi_link.h"
#include "config.h"
#include <WiFi.h>

static const IPAddress LOCAL_IP(192, 168, 4, 2);
static const IPAddress GATEWAY(192, 168, 4, 1);
static const IPAddress SUBNET(255, 255, 255, 0);

// Modo de prueba: la cámara crea la red ella misma, conservando su IP 192.168.4.2
// para que la app la encuentre en la misma dirección de siempre.
static void startOwnNetwork() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(LOCAL_IP, LOCAL_IP, SUBNET);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  WiFi.setSleep(false);
  Serial.print("[wifi] MODO PRUEBA: red propia creada. IP: ");
  Serial.println(WiFi.softAPIP());
}

void wifiConnect() {
  if (STANDALONE_TEST) {
    startOwnNetwork();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.config(LOCAL_IP, GATEWAY, SUBNET);
  WiFi.setSleep(false);   // menos retraso en el video
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("[wifi] Conectando a ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.print("\n[wifi] Conectada. IP: ");
  Serial.println(WiFi.localIP());
}

void wifiKeepAlive() {
  if (STANDALONE_TEST) return;   // en modo prueba la red es propia, no hay que reconectar

  static unsigned long lastTry = 0;
  if (WiFi.status() != WL_CONNECTED && millis() - lastTry > 2000) {
    lastTry = millis();
    Serial.println("[wifi] Señal perdida, reconectando...");
    WiFi.reconnect();
  }
}