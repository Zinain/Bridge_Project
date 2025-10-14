WiFiServer server(80);

const char* ssid = "BridgeControl";
const char* password = "12345678";

void setupWiFiAP() {
  Serial.println("Starting WiFi Access Point...");
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

// Runs on Core 0
void webTask(void* parameter) {
  for (;;) {
    WiFiClient client = server.available();
    if (!client) {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      continue;
    }

    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/open") != -1) {
      BridgeOpen();
    } else if (request.indexOf("/close") != -1) {
      BridgeClose();
    } else if (request.indexOf("/override") != -1) {
      overrided = !overrided;
    }

    // Basic web page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html\r\n");
    client.println("<!DOCTYPE html><html><body>");
    client.println("<h2>ESP32 Bridge Controller</h2>");
    client.println("<p>Bridge: " + Brstat + "</p>");
    client.println("<p>Traffic Red: " + outputRED + "</p>");
    client.println("<p>Traffic Green: " + outputGREEN + "</p>");
    client.println("<a href=\"/open\">Open Bridge</a><br>");
    client.println("<a href=\"/close\">Close Bridge</a><br>");
    client.println("<a href=\"/override\">Toggle Override</a>");
    client.println("</body></html>");
    client.stop();
  }
}
