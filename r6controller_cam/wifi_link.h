// =====================================================================
//  wifi_link.h  —  Conexión a la red WiFi del carro
// =====================================================================
#pragma once

// Se conecta a la red del carro con IP fija 192.168.4.2 (espera hasta lograrlo).
void wifiConnect();

// Reconecta si se perdió la señal. Llamar en loop().
void wifiKeepAlive();
