#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address, change to 0x3F if needed

#define VOLTAGE_SENSOR_PIN A0

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Battery Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(VOLTAGE_SENSOR_PIN);
  float analogVoltage = sensorValue * (5.0 / 1023.0);  // ADC to voltage
  float coilVoltage = analogVoltage * 5.7;             // Sensor scale factor

  int batteryPercent = map(coilVoltage * 100, 1100, 1260, 0, 100);
  batteryPercent = constrain(batteryPercent, 0, 100);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Coil V: ");
  lcd.print(coilVoltage, 2);
  lcd.print("V   ");

  lcd.setCursor(0, 1);
  lcd.print("Charge: ");
  lcd.print(batteryPercent);
  lcd.print("%    ");

  // Serial Output
  Serial.print("Coil Voltage: ");
  Serial.print(coilVoltage, 2);
  Serial.println(" V");

  Serial.print("Battery Charge: ");
  Serial.print(batteryPercent);
  Serial.println(" %");

  delay(2000);
}
