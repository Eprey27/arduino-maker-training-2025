/*
 * Current Sensor Test - INA219 (Motor Health Monitoring)
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * TEST: Current and Voltage Measurement with INA219
 *
 * OBJECTIVE:
 * Test the INA219 current/voltage sensor to measure motor power consumption.
 * This sensor provides high-precision measurements via I2C and can measure
 * both current (shunt resistor) and bus voltage simultaneously.
 *
 * HARDWARE SETUP:
 * INA219 Module:
 * - VCC -> Arduino 5V
 * - GND -> Arduino GND
 * - SDA -> Arduino A4 (SDA)
 * - SCL -> Arduino A5 (SCL)
 *
 * Power Measurement (Motor in series):
 * - Power Supply (+) -> INA219 Vin+
 * - INA219 Vin- -> Motor (+)
 * - Motor (-) -> GND
 * - Motor (-) -> MOSFET Drain (if using MOSFET control)
 *
 * HOW IT WORKS:
 * 1. INA219 has internal 0.1Ω shunt resistor
 * 2. Measures voltage drop across shunt (V = I × R)
 * 3. Calculates current: I = V_shunt / 0.1Ω
 * 4. Also measures bus voltage (supply voltage)
 * 5. Calculates power: P = V × I
 *
 * MEASUREMENT RANGE:
 * - Voltage: 0-26V (bus voltage)
 * - Current: ±3.2A (with 0.1Ω shunt, default)
 * - Resolution: 0.8mA (current), 4mV (voltage)
 * - Accuracy: ±0.5% typical
 *
 * I2C ADDRESS:
 * - Default: 0x40
 * - Can be changed to 0x41, 0x44, 0x45 by soldering jumpers
 *
 * EXPECTED RESULTS:
 * - Motor @ 5V, no load: 60-90mA (from previous tests)
 * - Voltage drop across INA219: <0.01V (negligible with 0.1Ω shunt)
 * - Stable readings when motor speed is constant
 * - Current increases when motor is mechanically loaded
 *
 * LIBRARY REQUIRED:
 * Adafruit INA219 Library
 * Install via: Tools > Manage Libraries > Search "INA219"
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

#include <Wire.h>
#include <Adafruit_INA219.h>

// Create INA219 object
Adafruit_INA219 ina219;

// Motor control
const int MOTOR_PWM_PIN = 9;
int motorPWM = 0;

// Measurement variables
float shuntVoltage_mV = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;
float loadVoltage_V = 0.0;  // Bus voltage - shunt voltage

// Statistics
float minCurrent = 9999.0;
float maxCurrent = 0.0;
float avgCurrent = 0.0;
unsigned long sampleCount = 0;

// Timing
unsigned long lastPrint = 0;
const unsigned int PRINT_INTERVAL = 500;  // Print every 500ms

// Auto test
bool autoTest = false;

void setup() {
  Serial.begin(115200);

  // Configure motor pin
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("INA219 Current/Voltage Sensor Test"));
  Serial.println(F("========================================"));
  Serial.println();

  // Initialize INA219
  Serial.print(F("Initializing INA219... "));
  if (!ina219.begin()) {
    Serial.println(F("FAILED!"));
    Serial.println(F("ERROR: INA219 not found!"));
    Serial.println(F("Check:"));
    Serial.println(F("  - I2C connections (SDA->A4, SCL->A5)"));
    Serial.println(F("  - Power supply (VCC, GND)"));
    Serial.println(F("  - I2C address (default 0x40)"));
    while (1) { delay(10); }  // Halt
  }
  Serial.println(F("SUCCESS!"));

  // Configure INA219 calibration
  // Default: 32V, 2A range (sufficient for our motor)
  // For higher precision with low current, could use 32V, 1A range
  ina219.setCalibration_32V_2A();

  Serial.println(F("Configuration: 32V, 2A range"));
  Serial.println(F("Shunt resistor: 0.1 ohm"));
  Serial.println(F("Current resolution: ~0.8mA"));
  Serial.println();

  // Test initial reading
  Serial.println(F("Testing initial reading (motor OFF)..."));
  readSensor();
  Serial.print(F("  Bus Voltage:   "));
  Serial.print(busVoltage_V, 3);
  Serial.println(F(" V"));
  Serial.print(F("  Shunt Voltage: "));
  Serial.print(shuntVoltage_mV, 2);
  Serial.println(F(" mV"));
  Serial.print(F("  Current:       "));
  Serial.print(current_mA, 1);
  Serial.println(F(" mA"));
  Serial.println();

  Serial.println(F("COMMANDS:"));
  Serial.println(F("  s - Start motor (PWM=100)"));
  Serial.println(F("  a - Auto PWM sweep"));
  Serial.println(F("  + - Increase PWM"));
  Serial.println(F("  - - Decrease PWM"));
  Serial.println(F("  x - Stop motor"));
  Serial.println(F("  r - Reset statistics"));
  Serial.println(F("  ? - Show help"));
  Serial.println();

  lastPrint = millis();
}

void loop() {
  // Check for commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }

  // Read sensor continuously for statistics
  readSensor();
  updateStatistics();

  // Print at regular intervals
  unsigned long currentTime = millis();
  if (currentTime - lastPrint >= PRINT_INTERVAL) {
    displayData();
    lastPrint = currentTime;
  }

  // Auto test mode
  if (autoTest) {
    static unsigned long lastChange = 0;
    if (currentTime - lastChange >= 5000) {  // Every 5 seconds
      motorPWM += 25;
      if (motorPWM > 255) motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      lastChange = currentTime;
      Serial.print(F("\n>>> Auto PWM -> "));
      Serial.println(motorPWM);
    }
  }

  delay(50);  // Small delay to avoid overwhelming I2C bus
}

// Read all values from INA219
void readSensor() {
  shuntVoltage_mV = ina219.getShuntVoltage_mV();
  busVoltage_V = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadVoltage_V = busVoltage_V + (shuntVoltage_mV / 1000.0);
}

// Update running statistics
void updateStatistics() {
  if (current_mA < minCurrent) minCurrent = current_mA;
  if (current_mA > maxCurrent) maxCurrent = current_mA;

  // Running average
  sampleCount++;
  avgCurrent = avgCurrent + (current_mA - avgCurrent) / sampleCount;
}

// Display current measurements
void displayData() {
  static int headerCount = 0;
  if (headerCount % 15 == 0) {
    Serial.println();
    Serial.println(F("Time\tPWM\tBus(V)\tLoad(V)\tCurr(mA)\tPower(mW)\tMin\tMax\tAvg"));
    Serial.println(F("----\t---\t------\t-------\t--------\t---------\t---\t---\t---"));
  }
  headerCount++;

  Serial.print(millis() / 1000);
  Serial.print(F("\t"));
  Serial.print(motorPWM);
  Serial.print(F("\t"));
  Serial.print(busVoltage_V, 2);
  Serial.print(F("\t"));
  Serial.print(loadVoltage_V, 2);
  Serial.print(F("\t"));
  Serial.print(current_mA, 1);
  Serial.print(F("\t\t"));
  Serial.print(power_mW, 1);
  Serial.print(F("\t\t"));
  Serial.print(minCurrent, 0);
  Serial.print(F("\t"));
  Serial.print(maxCurrent, 0);
  Serial.print(F("\t"));
  Serial.print(avgCurrent, 0);
  Serial.println();
}

// Handle serial commands
void handleCommand(char cmd) {
  switch (cmd) {
    case 's':
    case 'S':
      motorPWM = 100;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.println(F("\n>>> Motor started (PWM=100)"));
      break;

    case 'a':
    case 'A':
      autoTest = !autoTest;
      Serial.print(F("\n>>> Auto test: "));
      Serial.println(autoTest ? F("ON") : F("OFF"));
      if (autoTest) motorPWM = 50;
      break;

    case '+':
      motorPWM = constrain(motorPWM + 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      break;

    case '-':
      motorPWM = constrain(motorPWM - 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      break;

    case 'x':
    case 'X':
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      autoTest = false;
      Serial.println(F("\n>>> Motor STOPPED"));
      break;

    case 'r':
    case 'R':
      resetStatistics();
      Serial.println(F("\n>>> Statistics RESET"));
      break;

    case '?':
      printHelp();
      break;
  }
}

void resetStatistics() {
  minCurrent = 9999.0;
  maxCurrent = 0.0;
  avgCurrent = 0.0;
  sampleCount = 0;
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("s - Start motor (PWM=100)"));
  Serial.println(F("a - Auto PWM sweep"));
  Serial.println(F("+ - Increase PWM (+25)"));
  Serial.println(F("- - Decrease PWM (-25)"));
  Serial.println(F("x - Stop motor"));
  Serial.println(F("r - Reset statistics"));
  Serial.println(F("? - Show help"));
  Serial.println(F("==============================\n"));
}

/*
 * VERIFICATION CHECKLIST:
 *
 * [ ] INA219 detected on I2C bus (check with I2C scanner if issues)
 * [ ] Bus voltage matches power supply (~5V)
 * [ ] Current reading is ~0mA when motor is off
 * [ ] Current increases when motor PWM increases
 * [ ] Current matches multimeter measurement (±5%)
 * [ ] Power calculation is reasonable (P = V × I)
 * [ ] No significant voltage drop across INA219 (<0.01V)
 *
 * EXPECTED VALUES (to be filled after testing):
 * Motor OFF:       _____ mA
 * PWM=50:          _____ mA
 * PWM=100:         _____ mA
 * PWM=150:         _____ mA
 * PWM=200:         _____ mA
 * PWM=255:         _____ mA
 *
 * Startup current spike: _____ mA (transient)
 *
 * TROUBLESHOOTING:
 *
 * "INA219 not found" error:
 * - Check I2C wiring (SDA, SCL, VCC, GND)
 * - Verify I2C address with scanner sketch
 * - Try different I2C address (0x41, 0x44, 0x45)
 * - Check for I2C pull-up resistors (usually on module)
 *
 * Current reads 0 or negative:
 * - Check power flow direction (Vin+ should be supply, Vin- to motor)
 * - Swap Vin+ and Vin- if reading is negative
 * - Verify motor is actually running
 *
 * Erratic readings:
 * - Add 100µF capacitor across motor terminals
 * - Increase delay between readings
 * - Check for loose connections
 * - Motor noise may affect I2C (use shielded cables if necessary)
 *
 * Current seems too high/low:
 * - Compare with multimeter in series
 * - Try different calibration (32V_1A for lower current)
 * - Check for short circuits or additional loads
 *
 * Bus voltage drops when motor runs:
 * - Power supply may be insufficient (check rating)
 * - Use larger gauge wires
 * - Add bulk capacitor (1000µF) near motor
 *
 * NOTES:
 * - INA219 can handle up to 26V on bus (safe for 5V-12V motors)
 * - Current measurement is bidirectional (detects reverse current)
 * - For currents >3.2A, use external shunt resistor
 * - Consider logging data to SD card for long-term monitoring
 */
