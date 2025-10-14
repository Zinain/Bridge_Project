// Shared states
String outputRED = "off";
String outputGREEN = "on";
String Brstat = "closed";
bool overrided = false;

// Motor control pins
#define motor1pin1 4
#define motor1pin2 2
#define ENA 13

// LEDs
#define BoatRED 25
#define BoatGREEN 26
#define TrafficRED 32
#define TrafficGREEN 33

// Motor functions
void stopMotor() {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, 0);
}

void motorForward(int speed) {
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  analogWrite(ENA, speed);
}

void motorBackward(int speed) {
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  analogWrite(ENA, speed);
}

// Signal functions
void TrafficGoSignal() {
  digitalWrite(TrafficRED, LOW);
  digitalWrite(TrafficGREEN, HIGH);
}

void TrafficStopSignal() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(TrafficRED, HIGH);
    digitalWrite(TrafficGREEN, LOW);
    delay(200);
    digitalWrite(TrafficRED, LOW);
    digitalWrite(TrafficGREEN, HIGH);
    delay(200);
  }
  digitalWrite(TrafficRED, HIGH);
  digitalWrite(TrafficGREEN, LOW);
}

void BoatStopSignal() {
  digitalWrite(BoatGREEN, LOW);
  digitalWrite(BoatRED, HIGH);
  delay(1000);
}

void BoatGoSignal() {
  digitalWrite(BoatRED, LOW);
  digitalWrite(BoatGREEN, HIGH);
}

void BridgeOpen() {
  TrafficStopSignal();
  for (int i = 0; i < 4; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    motorForward(200);
    delay(300);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(300);
  }
  stopMotor();
  BoatGoSignal();
  digitalWrite(TrafficRED, HIGH);
  digitalWrite(TrafficGREEN, LOW);
}

void BridgeClose() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    motorBackward(200);
    delay(300);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(300);
  }
  stopMotor();
  BoatStopSignal();
  TrafficGoSignal();
}

void setupBridgePins() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(BoatRED, OUTPUT);
  pinMode(BoatGREEN, OUTPUT);
  pinMode(TrafficRED, OUTPUT);
  pinMode(TrafficGREEN, OUTPUT);

  pinMode(ENA, OUTPUT);
  stopMotor();
}
