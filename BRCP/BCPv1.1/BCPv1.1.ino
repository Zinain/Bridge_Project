
// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "Systems10";
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
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
long duration1;
long duration2;
float distanceCm1;
float distanceCm2;




// Assign output variables to GPIO pins
//Red LED pins
const int BoatRED = 25;
const int TrafficRED = 32;
//Green LED pins
const int BoatGREEN = 26;
const int TrafficGREEN = 33;
//Motor pins
const int motor1pin1 = 4;
const int motor1pin2 = 2;
const int ENA = 13; //speed control
//Ultrasonic sensor pins
const int echo1 = 5;
const int echo2 = 18;
const int trigg1 = 19;
const int trigg2 = 21;


void BoatStopSignal(){
  //Red light on for 2 seconds
  digitalWrite(BoatGREEN, LOW);
  digitalWrite(BoatRED, HIGH);
  delay(2000);
  digitalWrite(BoatRED, LOW);
}

void BoatGoSignal(){
  //Green light on 
  digitalWrite(BoatGREEN, HIGH);
}

void TrafficStopSignal(){
  //Red and green blinking
  for (int i = 0; i < 5; i++) {
    digitalWrite(TrafficRED, HIGH);
    digitalWrite(TrafficGREEN, HIGH);
    delay(500);
    digitalWrite(TrafficRED, LOW);
    digitalWrite(TrafficGREEN, LOW);
    delay(500);
  }
}

void TrafficGoSignal(){
  //green on
  digitalWrite(TrafficRED, LOW);
  digitalWrite(TrafficGREEN, HIGH);
}

void BridgeOpen(){
  //Boat signal green and red lights blinking while openning
  //Motor moves forward
  //motor stops
  //Green lights for boats are on
  //Red lights for traffic
  TrafficStopSignal();
  for (int i = 0; i < 6; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    motorForward(180);
    delay(500);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(500);
  }
  stopMotor();

  // Green lights for boats, red lights for traffic
  digitalWrite(BoatGREEN, HIGH);
  digitalWrite(BoatRED, LOW);
  digitalWrite(TrafficRED, HIGH);
  digitalWrite(TrafficGREEN, LOW);
}

void BridgeClose(){
  //similarly to the above but the bridge closes
  for (int i = 0; i < 6; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    motorBackward(180);
    delay(500);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(500);
  }
  stopMotor();

  BoatStopSignal();
  TrafficGoSignal();
  digitalWrite(BoatGREEN, LOW);
  digitalWrite(BoatRED, HIGH);
}

void stopMotor(){
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, 0);
}

void motorForward(int speed) { // opening
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, speed);
}

void motorBackward(int speed) { // closing
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  analogWrite(ENA, speed);
}

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  //RED pins
  pinMode(BoatRED, OUTPUT);
  pinMode(TrafficRED, OUTPUT);
  //GREEn pins
  pinMode(BoatGREEN, OUTPUT);
  pinMode(TrafficGREEN, OUTPUT);
  //Motor pins
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(ENA, OUTPUT);
  //Ultrasonic pins
  pinMode(trigg1, OUTPUT);
  pinMode(trigg2, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);
  // Set outputs to LOW
  digitalWrite(BoatRED, LOW);
  digitalWrite(TrafficRED, LOW);

  digitalWrite(BoatGREEN, LOW);
  digitalWrite(TrafficGREEN, LOW);


  digitalWrite(trigg1, LOW);
  digitalWrite(trigg2, LOW);
  
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

  /*digitalWrite(trigg1, HIGH);
  digitalWrite(trigg2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg1, LOW);
  digitalWrite(trigg2, LOW);

  duration1 = pulseIn(echo1, HIGH);
  duration2 = pulseIn(echo2, HIGH);

  // Calculate the distance
  distanceCm1 = duration1 * SOUND_SPEED/2;
  distanceCm2 = duration2 * SOUND_SPEED/2;
  Serial.print("Distance sensor 1 (cm): ");
  Serial.println(distanceCm1);
  Serial.print("Distance sensor 2 (cm): ");
  Serial.println(distanceCm2);



  if(distanceCm1 < 2 || distanceCm2 < 2){
      Brstat = "open";
      outputRED = "on";
      outputGREEN = "off";
      BridgeOpen();
      delay(10);
      Brstat = "closed";
      outputRED = "off";
      outputGREEN = "on";
      BridgeClose();
  }*/


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
            } else if (header.indexOf("GET /sensoroverride/off" ) >= 0){
              Brstat = "open";
              outputRED = "on";
              outputGREEN = "off";
              overrided = true;
              client.println("<p><a href=\"/sensoroverride/on\"><button class=\"button\">Sensor Override On</button></a></p>");
              BridgeOpen();
              delay(10);
              Brstat = "closed";
              outputRED = "off";
              outputGREEN = "on";
              BridgeClose();
              overrided = false;
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

            if(!overrided){
              client.println("<p><a href=\"/sensoroverride/off\"><button class=\"button button2\">Sensor Override Off</button></a></p>");
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