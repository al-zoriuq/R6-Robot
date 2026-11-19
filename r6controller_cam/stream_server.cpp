#include "stream_server.h"
#include "config.h"
#include "esp_http_server.h"

// MJPEG = muchas fotos JPEG enviadas seguidas por la misma conexión,
// separadas por una "frontera" (boundary).
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static httpd_handle_t server = NULL;

static esp_err_t streamHandler(httpd_req_t* req) {
  char partHeader[64];
  esp_err_t res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    camera_fb_t* fb = esp_camera_fb_get();   // toma una foto
    if (!fb) { res = ESP_FAIL; break; }

    res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
    if (res == ESP_OK) {
      size_t len = snprintf(partHeader, sizeof(partHeader), STREAM_PART, fb->len);
      res = httpd_resp_send_chunk(req, partHeader, len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    }

    esp_camera_fb_return(fb);   // libera la memoria de la foto
    if (res != ESP_OK) break;   // la app se desconectó
  }
  return res;
}

void streamServerStart() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = STREAM_PORT;
  config.ctrl_port   = 32769;

  httpd_uri_t streamUri = {};
  streamUri.uri      = "/stream";
  streamUri.method   = HTTP_GET;
  streamUri.handler  = streamHandler;
  streamUri.user_ctx = NULL;

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &streamUri);
    Serial.printf("[stream] Video en puerto %u, ruta /stream\n", STREAM_PORT);
  } else {
    Serial.println("[stream] No se pudo iniciar el servidor");
  }
}
