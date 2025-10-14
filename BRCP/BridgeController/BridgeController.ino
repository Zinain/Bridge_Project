#include <WiFi.h>

// Shared states
extern String outputRED;
extern String outputGREEN;
extern String Brstat;
extern bool overrided;

// Web server reference
extern WiFiServer server;

// Function declarations
void setupBridgePins();
void setupWiFiAP();
void bridgeTask(void* parameter);
void webTask(void* parameter);

void setup() {
  Serial.begin(115200);
  setupBridgePins();
  setupWiFiAP();

  // Start tasks on two cores
  xTaskCreatePinnedToCore(
    bridgeTask, "BridgeTask", 8000, NULL, 1, NULL, 1);  // core 1
  xTaskCreatePinnedToCore(
    webTask, "WebTask", 10000, NULL, 1, NULL, 0);        // core 0
}

void loop() {
  // Nothing needed here — tasks handle everything concurrently
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
