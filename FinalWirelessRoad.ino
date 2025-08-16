#include <Servo.h>

// Define the pins for the IR sensors, relays, and servo
const int irSensorPins[] = {2, 4, 6, 8, 10};   // IR sensor pins (e.g., Pins 2, 4, 6, 8, 10)
const int relayPins[] = {3, 5, 7, 9};          // Relay control pins (e.g., Pins 3, 5, 7, 9)
const int servoPin = 11;                       // Servo control pin (e.g., Pin 11)

Servo servo;  // Create a servo object

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(irSensorPins[i], INPUT);
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);  // Initialize all relays to be off
  }

  servo.attach(servoPin);  // Attach the servo to the specified pin
  servo.write(90);         // Initialize the servo position to 90 degrees
}

void loop() {
  for (int i = 0; i < 5; i++) {
    // Read the state of each IR sensor
    int irSensorState = digitalRead(irSensorPins[i]);

    if (irSensorState == LOW) {
      if (i == 4) {
        // Object detected by the 5th IR sensor, move the servo to 180 degrees
        servo.write(180);
        delay(3000); // Delay for one second (adjust as needed)
        servo.write(90); // Move the servo back to 90 degrees
      } else {
        // Object detected by other sensors, turn the corresponding relay on
        digitalWrite(relayPins[i], LOW);
      }
    } else {
      if (i != 4) {
        // No object detected by other sensors, turn the corresponding relay off
        digitalWrite(relayPins[i], HIGH);
      }
    }
  }

  delay(100);  // Adjust the delay as needed to prevent false triggers
}
