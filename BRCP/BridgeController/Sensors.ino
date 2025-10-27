#define trigg1 19
#define echo1 5
#define trigg2 21
#define echo2 18
#define SOUND_SPEED 0.034

long duration1, duration2;
float distanceCm1, distanceCm2;

extern bool emergencyStopActive;


float readDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH) * SOUND_SPEED / 2;
}

// Runs on Core 1
void bridgeTask(void* parameter) {
  pinMode(trigg1, OUTPUT);
  pinMode(trigg2, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(echo2, INPUT);

  unsigned long lastCheck = 0;

  for (;;) {
    if (emergencyStopActive) {
      vTaskDelay(200 / portTICK_PERIOD_MS);
      continue;
    }
    
    if ( millis() - lastCheck > 2000) {
      lastCheck = millis();

      distanceCm1 = readDistance(trigg1, echo1);
      distanceCm2 = readDistance(trigg2, echo2);

      Serial.printf("Sensor1: %.2f cm, Sensor2: %.2f cm\n", distanceCm1, distanceCm2);

      if (distanceCm1 < 5 || distanceCm2 < 5) {
        BridgeOpen();

        vTaskDelay(5000 / portTICK_PERIOD_MS);

        BridgeClose();
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
