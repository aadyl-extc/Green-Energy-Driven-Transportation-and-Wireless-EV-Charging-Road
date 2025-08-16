#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <math.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo setup
Servo servo1;
Servo servo2;
Servo servo3;

// Pin definitions
const int servoPin1 = 9;
const int servoPin2 = 10;
const int servoPin3 = 11;

const int switchUpPin = 2;
const int switchDownPin = 4;
const int signalPinFromUno = 3;

const int irSensorPin1 = A1;
const int irSensorPin2 = A2;

const int led1 = 12;
const int led2 = 13;

// Servo angles
float restAngle1 = 12;
float restAngle2 = -10;
float restAngle3 = 170;

float upAngle1 = 82;
float upAngle2 = 35;
float upAngle3 = 100;

bool isPantographUp = false;
bool autoLowered = false;

unsigned long suspensionStartTime = 0;
float currentAngle1 = upAngle1;

int lastUpState = HIGH;
int lastDownState = HIGH;
int lastSignalState = HIGH;

int lastIR1State = HIGH;
int lastIR2State = HIGH;

unsigned long lastMotionChangeTime = 0;
bool vehicleMoving = false;
bool lastVehicleMoving = false;

unsigned long lastBlinkTime = 0;
unsigned long blinkTimeout = 1000;

String lastPantographStatus = "Pantograph Ready";

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(lastPantographStatus);
  lcd.setCursor(0, 1);
  lcd.print("Vehicle Stopped ");

  pinMode(switchUpPin, INPUT_PULLUP);
  pinMode(switchDownPin, INPUT_PULLUP);
  pinMode(signalPinFromUno, INPUT);

  pinMode(irSensorPin1, INPUT);
  pinMode(irSensorPin2, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);

  servo1.write(restAngle1);
  servo2.write(restAngle2);
  servo3.write(restAngle3);
  delay(500);
}

void loop() {
  int upState = digitalRead(switchUpPin);
  int downState = digitalRead(switchDownPin);
  int signalFromUno = digitalRead(signalPinFromUno);

  int ir1 = digitalRead(irSensorPin1);
  int ir2 = digitalRead(irSensorPin2);

  Serial.print("IR1: ");
  Serial.print(ir1);
  Serial.print(" | IR2: ");
  Serial.println(ir2);

  // Detect motion based on IR blinking
  if (ir1 != lastIR1State || ir2 != lastIR2State) {
    lastBlinkTime = millis();
    vehicleMoving = true;
  } else {
    if (millis() - lastBlinkTime > blinkTimeout) {
      vehicleMoving = false;
    }
  }

  if (vehicleMoving != lastVehicleMoving) {
    if (vehicleMoving) {
      lcd.setCursor(0, 1);
      lcd.print("Vehicle Moving  ");
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Vehicle Stopped ");
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
    }

    lastVehicleMoving = vehicleMoving;
  }

  lastIR1State = ir1;
  lastIR2State = ir2;

  // Auto-lower on signal from Uno
  if (signalFromUno == LOW && lastSignalState == HIGH && isPantographUp) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Power Lost!");
    lcd.setCursor(0, 1);
    lcd.print("Auto Lowering... ");
    delay(1500);
    lowerPantograph();
    autoLowered = true;
  }

  // Manual raise
  if (upState == LOW && lastUpState == HIGH) {
    if (!isPantographUp) {
      raisePantograph();
      autoLowered = false;
    }
    delay(200);
  }

  // Manual lower
  if (downState == LOW && lastDownState == HIGH) {
    if (isPantographUp) {
      lowerPantograph();
    }
    delay(200);
  }

  lastUpState = upState;
  lastDownState = downState;
  lastSignalState = signalFromUno;

  // Suspension logic
  if (isPantographUp) {
    unsigned long timeNow = millis() - suspensionStartTime;
    float bounce = sin(timeNow * 0.005) * 2.5;
    float newAngle1 = upAngle1 + bounce;
    float newAngle3 = upAngle3 - bounce;

    servo1.write(newAngle1);
    servo2.write(upAngle2);
    servo3.write(newAngle3);
    delay(10);
  }
}

void raisePantograph() {
  Serial.println("🔼 Pantograph going up");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pantograph");
  lcd.setCursor(0, 1);
  lcd.print("Going UP...     ");

  float angle2 = restAngle2;
  float angle3 = restAngle3;

  for (float angle1 = restAngle1; angle1 <= upAngle1; angle1 += 1) {
    servo1.write(angle1);
    angle3 = restAngle3 - (angle1 - restAngle1);
    servo3.write(angle3);

    if (angle1 >= (restAngle1 + upAngle1) / 2 && angle2 < upAngle2) {
      angle2 += 1;
      servo2.write(angle2);
    }
    delay(15);
  }

  servo1.write(upAngle1);
  servo2.write(upAngle2);
  servo3.write(upAngle3);

  currentAngle1 = upAngle1;
  suspensionStartTime = millis();
  isPantographUp = true;

  lastPantographStatus = "Pantograph UP";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lastPantographStatus);
  lcd.setCursor(0, 1);
  lcd.print("Active Suspensn");
}

void lowerPantograph() {
  Serial.println("🔽 Pantograph going down");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pantograph");
  lcd.setCursor(0, 1);
  lcd.print("Going DOWN...   ");

  float angle1 = upAngle1;
  float angle3 = upAngle3;

  for (float angle2 = upAngle2; angle2 >= restAngle2; angle2 -= 1) {
    servo2.write(angle2);

    if (angle2 <= (restAngle2 + upAngle2) / 2 && angle1 > restAngle1) {
      angle1 -= 1;
      angle3 += 1;
      servo1.write(angle1);
      servo3.write(angle3);
    }

    delay(15);
  }

  while (angle1 > restAngle1) {
    angle1 -= 1;
    angle3 += 1;
    servo1.write(angle1);
    servo3.write(angle3);
    delay(15);
  }

  servo1.write(restAngle1);
  servo2.write(restAngle2);
  servo3.write(restAngle3);

  isPantographUp = false;

  lastPantographStatus = "Pantograph Down";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lastPantographStatus);
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear second line
}
