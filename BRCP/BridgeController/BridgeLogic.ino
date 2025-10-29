#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Most I2C LCDs use address 0x27 or 0x3F


// Shared states
String Traffic = "RED";
String Boats = "RED";
String Brstat = "closed";
bool emergencyStopActive = false;

// Encoder pins
const byte encoderA = 23; 
const byte encoderB = 22;
volatile long encoderCount = 0;
long targetPosition = 0; // closed position

// Motor control pins
#define motor1pin1 4
#define motor1pin2 2
#define ENA 13

// LEDs
#define BoatRED 25
#define BoatGREEN 26
#define TrafficRED 32
#define TrafficGREEN 33

// buzzer
#define Buzzer 27

// LCD



// Encoder
void IRAM_ATTR onEncoderChange() {
  int b = digitalRead(encoderB);
  if (b == HIGH) encoderCount++;
  else encoderCount--;
}

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

void TrafficToggle(){
  if(Traffic == "RED"){
    digitalWrite(TrafficRED, LOW);
    digitalWrite(TrafficGREEN, HIGH);
    Traffic = "GREEN";
  } else {
    digitalWrite(TrafficRED, HIGH);
    digitalWrite(TrafficGREEN, LOW);
    Traffic = "RED";
  }
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

void BoatToggle(){
  if(Boats == "RED"){
    BoatGoSignal();
  } else {
    BoatStopSignal();
  }
}

void BridgeOpen() {
  if (emergencyStopActive) {
    Serial.println("Cannot open bridge: Emergency stop active!");
    return;
  }
  Brstat = "opening";
  TrafficStopSignal();
  for (int i = 0; i < 4 && !emergencyStopActive; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bridge opening");
    tone(Buzzer, 2000);
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    Boats = "GREEN";
    motorForward(200);
    delay(300);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(300);
    noTone(Buzzer);
    if (emergencyStopActive) {
      return;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bridge open");
  stopMotor();
  BoatGoSignal();
  digitalWrite(TrafficRED, HIGH);
  digitalWrite(TrafficGREEN, LOW);
  Traffic = "RED";
  Brstat = "open";
}

void ManualOpen() {
    Brstat = "opening";
    for (int i = 0; i < 4 ; i++) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Manual opening");
      tone(Buzzer, 2000);
      motorForward(200);
      delay(300);
      delay(300);
      noTone(Buzzer);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bridge open");
    stopMotor();
    Brstat = "open";
}

void BridgeClose() {
  if (emergencyStopActive) {
    Serial.println("Cannot close bridge: Emergency stop active!");
    return;
  }
  Brstat = "closing";
  for (int i = 0; i < 4 && !emergencyStopActive; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bridge closing");
    tone(Buzzer, 2000);
    digitalWrite(BoatRED, HIGH);
    digitalWrite(BoatGREEN, HIGH);
    Boats = "RED";
    motorBackward(200);
    delay(300);
    digitalWrite(BoatRED, LOW);
    digitalWrite(BoatGREEN, LOW);
    delay(300);
    noTone(Buzzer);
    if (emergencyStopActive) {
      return;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bridge closed");
  stopMotor();
  BoatStopSignal();
  TrafficGoSignal();
  Brstat = "closed";
}

void ManualClose() {
    Brstat = "closing";
    for (int i = 0; i < 4 ; i++) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Manual closing");
      tone(Buzzer, 2000);
      motorBackward(200);
      delay(300);
      delay(300);
      noTone(Buzzer);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bridge closed");
    stopMotor();
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
    Traffic = "RED";
    Boats = "RED";
  } else {
    Serial.println("Emergency stop deactivated. Normal operation restored.");
  }
}


// reState() — return to original position
void reState() {
  if(!emergencyStopActive){
    return;
  }
  Serial.println("Restoring bridge to safe closed position...");

  BoatStopSignal();
  TrafficStopSignal();

  // Suppose encoderCount > 0 means partially open
  while (encoderCount > targetPosition) {
    motorBackward(180);
    delay(10);
  }

  stopMotor();
  encoderCount = targetPosition;

  BoatStopSignal();
  TrafficGoSignal();

  Brstat = "closed";

  Serial.println("Bridge restored to closed position.");
}

void setupBridgePins() {
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(BoatRED, OUTPUT);
  pinMode(BoatGREEN, OUTPUT);
  pinMode(TrafficRED, OUTPUT);
  pinMode(TrafficGREEN, OUTPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(Buzzer, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(encoderA), onEncoderChange, CHANGE);

  pinMode(ENA, OUTPUT);
  TrafficGoSignal();
  BoatStopSignal();
  stopMotor();
  // LCD setup
  Wire.begin(16, 17);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BridgeController");
  lcd.setCursor(0, 1);
  lcd.print("Status: Ready");
}
