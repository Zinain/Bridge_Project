/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
//Red LED Variables current state
String outputRED = "off";
//Green LED Variables current state
String outputGREEN = "off";
//Motor pins current state
String Brstat = "closed";
//Override on/off state
String overrided = "off";


// Assign output variables to GPIO pins
//Red LED pins
const int outputRED1 = 2;
const int outputRED2 = 4;
const int outputRED4 = 5;
const int outputRED3 = 6;
//Green LED pins
const int outputGREEN1 = 7;
const int outputGREEN2 = 15;
const int outputGREEN3 = 16;
const int outputGREEN4 = 17;
//Motor pins
const int motor1pin1 = 18;
const int motor1pin2 = 8;

void GlightsOn(){

}

void RlightsOn(){

}

void GlightsOff(){

}

void RlightsOff(){

}

void motorF(){

}

void motorB(){

}

void motorS(){

}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  //RED pins
  pinMode(outputRED1, OUTPUT);
  pinMode(outputRED2, OUTPUT);
  pinMode(outputRED3, OUTPUT);
  pinMode(outputRED4, OUTPUT);
  //GREEn pins
  pinMode(outputGREEN1, OUTPUT);
  pinMode(outputGREEN2, OUTPUT);
  pinMode(outputGREEN3, OUTPUT);
  pinMode(outputGREEN4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(outputRED1, LOW);
  digitalWrite(outputRED2, LOW);
  digitalWrite(outputRED3, LOW);
  digitalWrite(outputRED4, LOW);

  digitalWrite(outputGREEN1, LOW);
  digitalWrite(outputGREEN2, LOW);
  digitalWrite(outputGREEN3, LOW);
  digitalWrite(outputGREEN4, LOW);
  

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /red/on") >= 0) {
              Serial.println("RED on");
              outputRED = "on";
              RlightsOn();
            } else if (header.indexOf("GET /red/off") >= 0) {
              Serial.println("RED off");
              outputRED = "off";
              RlightsOff();
            } else if (header.indexOf("GET /green/on") >= 0) {
              Serial.println("GREEN on");
              outputGREEN = "on";
              GlightsOn();
            } else if (header.indexOf("GET /green/off") >= 0) {
              Serial.println("GREEN off");
              outputGREEN = "off";
              GlightsOff();
            } else if (header.indexOf("GET /all/on" ) >= 0) {
              outputGREEN = "on";
              outputRED = "on";
              GlightsOn();
              RlightsOn();
            } else if (header.indexOf("GET /all/off" ) >= 0) {
              outputGREEN = "off";
              outputRED = "off";
              GlightsOff();
              RlightsOff();
            } else if (header.indexOf("GET /open/off" ) >= 0) {
              Brstat = "open";
              outputRED = "on";
              outputGREEN = "off";
              GlightsOff();
              RlightsOn();
              motorF();
              delay(1);
              motorS();
            } else if (header.indexOf("GET /close/off" ) >= 0) {
              Brstat = "closed";
              outputRED = "off";
              outputGREEN = "on";
              GlightsOn();
              RlightsOff();
              motorB();
              delay(1);
              motorS();
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>RED - State " + outputRED + "</p>");
            // If the output26State is off, it displays the ON button       
            if (outputRED=="off") {
              client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/red/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GREEN - State " + outputGREEN + "</p>");
            // If the output27State is off, it displays the ON button       
            if (outputGREEN=="off") {
              client.println("<p><a href=\"/green/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/green/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            if(outputRED == "off" && outputGREEN == "off"){
              client.println("<p><a href=\"/all/on\"><button class=\"button\">ALL ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/all/off\"><button class=\"button button2\">ALL OFF</button></a></p>");
            }

            if(Brstat == "closed"){
              client.println("<p><a href=\"/open/on\"><button class=\"button\">OPEN BR</button></a></p>");
            } else {
              client.println("<p><a href=\"/close/off\"><button class=\"button button2\">CLOSE BR</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
      delay(1);
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}