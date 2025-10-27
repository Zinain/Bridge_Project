WiFiServer server(80);

const char* ssid = "BridgeControl";
const char* password = "12345678";

extern bool emergencyStopActive;
extern float distanceCm1, distanceCm2;

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

    // Handle routes
    if (request.indexOf("GET /open") >= 0) {
      BridgeOpen();
    } else if (request.indexOf("GET /close") >= 0) {
      BridgeClose(); 
    } else if (request.indexOf("/stop") >= 0) {
      EmergencyStopToggle();
    } else if (request.indexOf("/restate") >= 0) {
      reState();
    } else if (request.indexOf("GET /status") >= 0) {
      // Return JSON status (for AJAX refresh)
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close\r\n");
      client.print("{\"bridge\":\"" + Brstat + 
                   "\",\"red\":\"" + Traffic + 
                   "\",\"green\":\"" + Boats +
                   "\",\"emergency\":\"" + (emergencyStopActive ? "ON" : "OFF") + "\"}");
      client.stop();
      continue;
    }
    
    

    // Serve main HTML page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html\r\n");
    client.println("<!DOCTYPE html>");
    client.println("<html lang='en'>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<meta charset='UTF-8'>");
    client.println("<title>ESP32 Bridge Controller</title>");
    client.println("<style>");
    client.println("body{font-family:Arial,sans-serif;margin:0;padding:0;background:linear-gradient(135deg,#1c1c1c,#2f2f2f);color:#fff;display:flex;flex-direction:column;align-items:center;min-height:100vh;box-sizing:border-box;}");
    client.println("h2{margin-top:30px;font-size:2em;text-align:center;}");
    client.println(".card{background:#2c2c2c;border-radius:15px;box-shadow:0 4px 20px rgba(0,0,0,0.3);padding:25px 20px;width:90%;max-width:420px;text-align:center;box-sizing:border-box;}");
    client.println(".btn{display:block;width:100%;margin:10px 0;padding:14px;border:none;border-radius:10px;font-size:1.1em;font-weight:bold;cursor:pointer;color:#fff;transition:all 0.3s ease;text-decoration:none;box-sizing:border-box;}");
    client.println(".open{background-color:#28a745;}.open:hover{background-color:#34d058;}");
    client.println(".close{background-color:#dc3545;}.close:hover{background-color:#ff4d5e;}");
    client.println(".stop{background-color:#ff9800;}.stop:hover{background-color:#ffb84d;}");
    client.println(".restate{background-color:#ff9800;}.stop:hover{background-color:#ffb84d;}");
    client.println(".footer{margin-top:auto;padding:15px 0;font-size:0.85em;opacity:0.7;text-align:center;}");
    client.println("@media(max-width:600px){h2{font-size:1.6em;}.btn{font-size:1em;padding:12px;}}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h2>🌉 ESP32 Bridge Controller</h2>");
    client.println("<div class='card'>");
    client.println("<div id='status'>Loading...</div>");
    client.println("<a class='btn open' href='/open'>🔓 Open Bridge</a><br>");
    client.println("<a class='btn close' href='/close'>🔒 Close Bridge</a><br>");
    client.println("<a class='btn stop' href='/stop'>🛑 Emergency Stop</a>");
    client.println("<a class='btn restate' href='/restate'>🔄 Restate</a>");
    client.println("</div>");
    client.println("<div class='footer'>ESP32 Controller &copy; 2025</div>");
    client.println("<script>");
    client.println("async function updateStatus(){");
    client.println(" try {");
    client.println("  let res = await fetch('/status');");
    client.println("  let data = await res.json();");
    client.println("  let html = `<p><strong>Bridge:</strong> ${data.bridge}</p>` +");
    client.println("             `<p>🚦 Traffic: ${data.red}</p>` +");
    client.println("             `<p>🚦 Boats: ${data.green}</p>` +");
    client.println("             `<p>🛑 Emergency Stop: <strong>${data.emergency}</p>`;");
    client.println("  document.getElementById('status').innerHTML = html;");
    client.println(" } catch(e){ console.log('Error fetching status', e); }");
    client.println("}");
    client.println("setInterval(updateStatus, 1000); updateStatus();");
    client.println("</script>");
    client.println("</body></html>");
    client.stop();
  }
}

