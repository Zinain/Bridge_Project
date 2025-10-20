// Shared states
String Traffic = "RED";
String Boats = "RED";
String Brstat = "closed";
bool emergencyStopActive = false;

// Motor control pins
#define motor1pin1 4
#define motor1pin2 2
#define ENA 13

// LEDs
#define BoatRED 25
#define BoatGREEN 26
#define TrafficRED 32
#define TrafficGREEN 33

// Emergency Stop button
#define EMERGENCY_BTN 5

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
  Traffic = "GREEN";
}

void TrafficStopSignal() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(TrafficRED, HIGH);
    digitalWrite(TrafficGREEN, LOW);
    Traffic = "RED";
    delay(200);
    digitalWrite(TrafficRED, LOW);
    digitalWrite(TrafficGREEN, HIGH);
    Traffic = "GREEN";
    delay(200);
  }
  digitalWrite(TrafficRED, HIGH);
  digitalWrite(TrafficGREEN, LOW);
  Traffic = "RED";
}

void BoatStopSignal() {
  digitalWrite(BoatGREEN, LOW);
  digitalWrite(BoatRED, HIGH);
  Boats = "RED";
}

void BoatGoSignal() {
  digitalWrite(BoatRED, LOW);
  digitalWrite(BoatGREEN, HIGH);
  Boats = "GREEN";
}

void BridgeOpen() {
  if (emergencyStopActive) {
    Serial.println("Cannot open bridge: Emergency stop active!");
    return;
  }
  Brstat = "opening";
  TrafficStopSignal();
  for (int i = 0; i < 4 && !emergencyStopActive; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    Boats = "GREEN";
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
  Traffic = "RED";
  Brstat = "open";
}

void BridgeClose() {
  if (emergencyStopActive) {
    Serial.println("Cannot close bridge: Emergency stop active!");
    return;
  }
  Brstat = "closing";
  for (int i = 0; i < 4 && !emergencyStopActive; i++) {
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    Boats = "RED";
    motorBackward(200);
    delay(300);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(300);
  }
  stopMotor();
  BoatStopSignal();
  TrafficGoSignal();
  Brstat = "closed";
}

void EmergencyStopToggle() {
  emergencyStopActive = !emergencyStopActive;
  if (emergencyStopActive) {
    Serial.println("EMERGENCY STOP ACTIVATED");
    stopMotor();
    digitalWrite(TrafficRED, HIGH);
    digitalWrite(TrafficGREEN, LOW);
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, LOW);
  } else {
    Serial.println("Emergency stop deactivated. Normal operation restored.");
  }
}

void CheckEmergencyButton() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(EMERGENCY_BTN);
  if (lastState == HIGH && currentState == LOW) {
    EmergencyStopToggle();
    delay(300);  // debounce
  }
  lastState = currentState;
}

void setupBridgePins() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(BoatRED, OUTPUT);
  pinMode(BoatGREEN, OUTPUT);
  pinMode(TrafficRED, OUTPUT);
  pinMode(TrafficGREEN, OUTPUT);
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  pinMode(ENA, OUTPUT);
  TrafficGoSignal();
  BoatStopSignal();
  stopMotor();
}
