#include <Servo.h>

Servo solarServo;

int ldrPin1 = 2;  // LDR module 1 connected to digital pin 2
int ldrPin2 = 3;  // LDR module 2 connected to digital pin 3
int threshold = 100;  // Adjust this value as needed

void setup() {
  solarServo.attach(9);  // Attach the servo to digital pin 9
  Serial.begin(9600);  // Initialize serial communication for debugging

  // Set the initial position of the servo to 90 degrees
  solarServo.write(90);
  delay(1500);
}

void loop() {
  int ldrValue1 = digitalRead(ldrPin1);
  int ldrValue2 = digitalRead(ldrPin2);

  Serial.print("LDR 1 Value: ");
  Serial.print(ldrValue1);
  Serial.print(" | LDR 2 Value: ");
  Serial.print(ldrValue2);
  Serial.print(" | Current Servo Angle: ");
  Serial.println(solarServo.read());

  if (ldrValue1 == HIGH && ldrValue2 == LOW) {
    tiltSolarPanel(1);  // Tilt towards LDR 1
  } else if (ldrValue2 == HIGH && ldrValue1 == LOW) {
    tiltSolarPanel(-1);  // Tilt towards LDR 2
  } else {
    // No significant difference, keep the solar panel horizontal or at a default angle
    tiltSolarPanel(0);
  }
}

void tiltSolarPanel(int direction) {
  int currentPos = solarServo.read();
  int newPos = currentPos + (direction * 1); // Adjust the angle as needed

  // Constrain the new position to avoid damaging the servo
  newPos = constrain(newPos, 80, 110);
  solarServo.write(newPos);

  delay(100);  // Add a delay to prevent constant servo movement
}
