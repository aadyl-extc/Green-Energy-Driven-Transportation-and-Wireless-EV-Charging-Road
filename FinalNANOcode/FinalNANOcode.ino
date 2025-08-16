#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // Set the LCD address and dimensions

// Define analog pins for your sensors
const int solarVoltagePin = A0;
const int solarCurrentPin = A1;
const int windVoltagePin = A2;
const int windCurrentPin = A3;

void setup() {
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  // Clear the LCD
  lcd.clear();
}


id loop() {
  // Read voltage and current values for solar and wind
  float solarVoltage = (analogRead(solarVoltagePin) * 1.35 / 12.3); // Assuming a 5V reference
  float solarCurrent = (analogRead(solarCurrentPin) * 2.5 / 10023.0); // Assuming a 5V reference

  float windVoltage = (analogRead(windVoltagePin) * 1.35 / 12.3); // Assuming a 5V reference
  float windCurrent = (analogRead(windCurrentPin) * 2.5 / 10023.0); // Assuming a 5V reference

  // Display the values on the LCD with units
  lcd.setCursor(0, 0);
  lcd.print("Solar Voltage: ");
  lcd.print(solarVoltage, 1);
  lcd.print("V");

  lcd.setCursor(0, 1);
  lcd.print("Solar Current: ");
  lcd.print(solarCurrent, 2);
  lcd.print("A");

  lcd.setCursor(0, 2);
  lcd.print("Wind  Voltage: ");
  lcd.print(windVoltage, 1);
  lcd.print("V");

  lcd.setCursor(0, 3);
  lcd.print("Wind  Current: ");
  lcd.print(windCurrent, 2);
  lcd.print("A");

  delay(1000); // Update the display every second (adjust as needed)
}
