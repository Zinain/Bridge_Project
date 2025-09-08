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
String outputGREEN = "on";
//Motor pins current state
String Brstat = "closed";
//Override on/off state
bool overrided = false;


// Assign output variables to GPIO pins
//Red LED pins
const int outputRED1 = 34;
const int outputRED2 = 32;
const int outputRED3 = 25;
const int outputRED4 = 27;
//Green LED pins
const int outputGREEN1 = 35;
const int outputGREEN2 = 33;
const int outputGREEN3 = 26;
const int outputGREEN4 = 14;
//Motor pins
const int motor1pin1 = 4;
const int motor1pin2 = 2;

void BoatStopSignal(){
  //Red light on for 2 seconds
  digitalWrite(outputGREEN1, LOW);
  digitalWrite(outputGREEN2, LOW);
  digitalWrite(outputRED1, HIGH);
  digitalWrite(outputRED2, HIGH);
  delay(2000);
  digitalWrite(outputRED1, LOW);
  digitalWrite(outputRED2, LOW);
}

void BoatGoSignal(){
  //Green light on 
  digitalWrite(outputGREEN1, HIGH);
  digitalWrite(outputGREEN2, HIGH);
}

void TrafficStopSignal(){
  //Red and green blinking
  for (int i = 0; i < 5; i++) {
    digitalWrite(outputRED3, HIGH);
    digitalWrite(outputRED4, HIGH);
    digitalWrite(outputGREEN3, HIGH);
    digitalWrite(outputGREEN4, HIGH);
    delay(500);
    digitalWrite(outputRED3, LOW);
    digitalWrite(outputRED4, LOW);
    digitalWrite(outputGREEN3, LOW);
    digitalWrite(outputGREEN4, LOW);
    delay(500);
  }
}

void TrafficGoSignal(){
  //green on
  digitalWrite(outputRED3, LOW);
  digitalWrite(outputRED4, LOW);
  digitalWrite(outputGREEN3, HIGH);
  digitalWrite(outputGREEN4, HIGH);
}

void BridgeOpen(){
  //Boat signal green and red lights blinking while openning
  //Motor moves forward
  //motor stops
  //Green lights for boats are on
  //Red lights for traffic
  for (int i = 0; i < 6; i++) {
    digitalWrite(outputRED1, HIGH);
    digitalWrite(outputRED2, HIGH);
    digitalWrite(outputGREEN1, HIGH);
    digitalWrite(outputGREEN2, HIGH);
    motorForward();
    delay(500);
    digitalWrite(outputRED1, LOW);
    digitalWrite(outputRED2, LOW);
    digitalWrite(outputGREEN1, LOW);
    digitalWrite(outputGREEN2, LOW);
    delay(500);
  }
  stopMotor();

  // Green lights for boats, red lights for traffic
  BoatGoSignal();
  digitalWrite(outputRED3, HIGH);
  digitalWrite(outputRED4, HIGH);
  digitalWrite(outputGREEN3, LOW);
  digitalWrite(outputGREEN4, LOW);
}

void BridgeClose(){
  //similarly to the above but the bridge closes
  for (int i = 0; i < 6; i++) {
    digitalWrite(outputRED1, HIGH);
    digitalWrite(outputRED2, HIGH);
    digitalWrite(outputGREEN1, HIGH);
    digitalWrite(outputGREEN2, HIGH);
    motorBackward();
    delay(500);
    digitalWrite(outputRED1, LOW);
    digitalWrite(outputRED2, LOW);
    digitalWrite(outputGREEN1, LOW);
    digitalWrite(outputGREEN2, LOW);
    delay(500);
  }
  stopMotor();

  BoatStopSignal();
  TrafficGoSignal();
}

void stopMotor(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
}

void motorForward() { // opening
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
}

void motorBackward() { // closing
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
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
  //Motor pins
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  // Set outputs to LOW
  digitalWrite(outputRED1, LOW);
  digitalWrite(outputRED2, LOW);
  digitalWrite(outputRED3, LOW);
  digitalWrite(outputRED4, LOW);

  digitalWrite(outputGREEN1, LOW);
  digitalWrite(outputGREEN2, LOW);
  digitalWrite(outputGREEN3, LOW);
  digitalWrite(outputGREEN4, LOW);
  
  stopMotor();

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
  /*TrafficGoSignal();   // cars go
  delay(5000);

  TrafficStopSignal(); // stop cars
  BoatSignal();        // warn boats
  BridgeOpen();        // open bridge
  delay(5000);         // keep open

  BridgeClose();       // close bridge
  delay(5000);*///testing purposes

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
            
            if (header.indexOf("GET /closed/" ) >= 0) {
              Brstat = "open";
              outputRED = "on";
              outputGREEN = "off";
              BridgeOpen();
            } else if (header.indexOf("GET /open/" ) >= 0) {
              Brstat = "closed";
              outputRED = "off";
              outputGREEN = "on";
              BridgeClose();
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
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GREEN - State " + outputGREEN + "</p>");

            if(Brstat == "closed"){
              client.println("<p><a href=\"/closed/\"><button class=\"button button2\">Bridge Closed</button></a></p>");
            } else {
              client.println("<p><a href=\"/open/\"><button class=\"button\">Bridge Opened</button></a></p>");
            }
            
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