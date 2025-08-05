#include <Stepper.h>

#include <IRremote.h>

#include <Servo.h>


const int pingPin = 7;
int pos = 0;
Servo servo_4;


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  servo_4.attach(4, 500, 2500);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  //^ is pointless
  long duration, inches, cm;
  // i don't know what function does but gives me 
  pinMode(pingPin, OUTPUT);
  //pingpin is sensor and is outputing
  digitalWrite(pingPin, LOW);
  //ping output no signal
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  //lines, 35-47 are neccassary for timing
  delay(100);
  //delay for ~10 operations per second
  if(cm < 250 && pos < 90) {
    digitalWrite(2, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    pos = pos + 1;
    servo_4.write(pos);
    delay(20);
    //when bridge closed and boat close to bridge
    //road LED's = green, marine LED's = red
  } if ( cm < 250 && pos == 90) {
    digitalWrite(2, LOW);
    digitalWrite(8, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(11, LOW);
    delay(20);
    //when bridge open and boat close
    //road = red, water = green
  } if(cm > 250 && pos > 0) {
    digitalWrite(2, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(11, LOW);
    pos = pos - 1;
    servo_4.write(pos);
    delay(20);
    //when no boat and bridge closing
    //water = red, road = red & motor closing bridge
  } if(cm > 250 && pos < 1) {
    digitalWrite(2, HIGH);
    digitalWrite(8, LOW);
    digitalWrite(12, LOW);
    digitalWrite(11, HIGH);
    delay(20);
    //when no boat and bridge closed
    //road = green, water = red
  }
}
//i dont understand the below functions but they are 100% necassary for code to function
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}