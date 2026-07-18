#include "web_control.h"

#include <WiFi.h>

namespace {
const char* CONTROL_HEADER = "X-Control-Token";
int clampMotor(const String& value) { return constrain(value.toInt(), -255, 255); }
}

WebControl::WebControl(const char* ssid, const char* password, const char* token,
                       uint16_t deadmanMs)
    : ssid_(ssid), password_(password), token_(token), deadmanMs_(deadmanMs) {}

void WebControl::begin() {
  if (strlen(ssid_) == 0 || strlen(token_) < 12) {
    Serial.println("Wi-Fi control disabled: configure include/secrets.h");
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_, password_);
  Serial.print("Connecting to Wi-Fi");
  const uint32_t started = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - started < 15000) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi unavailable; autonomous mode remains active");
    return;
  }

  const char* headers[] = {CONTROL_HEADER};
  server_.collectHeaders(headers, 1);
  server_.on("/api/status", HTTP_GET, [this] { handleStatus(); });
  server_.on("/api/mode", HTTP_POST, [this] { handleMode(); });
  server_.on("/api/control", HTTP_POST, [this] { handleControl(); });
  server_.on("/api/status", HTTP_OPTIONS, [this] { handleOptions(); });
  server_.on("/api/mode", HTTP_OPTIONS, [this] { handleOptions(); });
  server_.on("/api/control", HTTP_OPTIONS, [this] { handleOptions(); });
  server_.onNotFound([this] { sendJson(404, "{\"error\":\"not found\"}"); });
  server_.begin();
  Serial.printf("Tank control: http://%s\n", WiFi.localIP().toString().c_str());
}

void WebControl::update() {
  if (WiFi.status() == WL_CONNECTED) server_.handleClient();
}

bool WebControl::commandFresh(uint32_t nowMs) const {
  return hasCommand_ && nowMs - lastCommandMs_ <= deadmanMs_;
}

const char* WebControl::ipAddress() const {
  ipCache_ = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "offline";
  return ipCache_.c_str();
}

bool WebControl::authorized() {
  return strlen(token_) >= 12 && server_.header(CONTROL_HEADER) == token_;
}

void WebControl::addCors() {
  server_.sendHeader("Access-Control-Allow-Origin", "*");
  server_.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server_.sendHeader("Access-Control-Allow-Headers", CONTROL_HEADER);
  server_.sendHeader("Cache-Control", "no-store");
}

void WebControl::sendJson(int status, const String& body) {
  addCors();
  server_.send(status, "application/json", body);
}

void WebControl::handleOptions() {
  addCors();
  server_.send(204);
}

void WebControl::handleMode() {
  if (!authorized()) return sendJson(401, "{\"error\":\"unauthorized\"}");
  const String requested = server_.arg("value");
  if (requested == "auto") mode_ = ControlMode::Auto;
  else if (requested == "manual") mode_ = ControlMode::Manual;
  else return sendJson(400, "{\"error\":\"mode must be auto or manual\"}");
  manualSpeeds_ = {0, 0};
  hasCommand_ = false;
  sendJson(200, String("{\"mode\":\"") + (mode_ == ControlMode::Auto ? "auto" : "manual") + "\"}");
}

void WebControl::handleControl() {
  if (!authorized()) return sendJson(401, "{\"error\":\"unauthorized\"}");
  if (mode_ != ControlMode::Manual)
    return sendJson(409, "{\"error\":\"manual mode required\"}");
  if (!server_.hasArg("left") || !server_.hasArg("right") || !server_.hasArg("seq"))
    return sendJson(400, "{\"error\":\"left, right and seq required\"}");
  const uint32_t sequence = strtoul(server_.arg("seq").c_str(), nullptr, 10);
  if (hasCommand_ && sequence <= lastSequence_)
    return sendJson(409, "{\"error\":\"stale sequence\"}");
  manualSpeeds_ = {clampMotor(server_.arg("left")), clampMotor(server_.arg("right"))};
  lastSequence_ = sequence;
  lastCommandMs_ = millis();
  hasCommand_ = true;
  sendJson(200, "{\"ok\":true}");
}

void WebControl::handleStatus() {
  if (!authorized()) return sendJson(401, "{\"error\":\"unauthorized\"}");
  String body = "{\"mode\":\"";
  body += mode_ == ControlMode::Auto ? "auto" : "manual";
  body += "\",\"distanceCm\":";
  body += telemetry_.rangeValid ? String(telemetry_.distanceCm, 1) : "null";
  body += ",\"rangeValid\":";
  body += telemetry_.rangeValid ? "true" : "false";
  body += ",\"motion\":\"" + String(motionName(telemetry_.motion)) + "\"";
  body += ",\"phase\":\"" + String(telemetry_.phase) + "\"";
  body += ",\"commandFresh\":";
  body += commandFresh(millis()) ? "true" : "false";
  body += ",\"lastSequence\":" + String(lastSequence_);
  body += ",\"ip\":\"" + String(ipAddress()) + "\"}";
  sendJson(200, body);
}
