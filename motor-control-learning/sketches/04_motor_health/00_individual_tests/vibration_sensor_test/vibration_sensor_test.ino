/*
 * Vibration Sensor Test - ADXL345 (Motor Health Monitoring)
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * TEST: Vibration Analysis with ADXL345 Accelerometer
 *
 * OBJECTIVE:
 * Test ADXL345 3-axis accelerometer to detect motor vibrations.
 * Vibration analysis can reveal:
 * - Unbalanced motor shaft
 * - Worn bearings
 * - Loose mounting
 * - Mechanical resonance
 * - General motor condition
 *
 * HARDWARE SETUP:
 * ADXL345 Module (I2C):
 * - VCC -> Arduino 5V (or 3.3V)
 * - GND -> Arduino GND
 * - SDA -> Arduino A4
 * - SCL -> Arduino A5
 * - I2C Address: 0x53 (default)
 *
 * Motor Control:
 * - PWM Pin D9 -> Motor control
 *
 * SENSOR MOUNTING (CRITICAL!):
 * - Mount ADXL345 RIGIDLY to a stable surface 2-5cm from motor
 * - Use double-sided foam tape or screws
 * - Breadboard is NOT rigid enough - use wooden/plastic board
 * - Sensor should not move independently from motor vibrations
 * - Mark sensor position and orientation for repeatable tests
 * - Axis orientation:
 *   X-axis: Perpendicular to motor shaft
 *   Y-axis: Parallel to motor shaft
 *   Z-axis: Vertical (gravity ~1g when flat)
 *
 * HOW IT WORKS:
 * 1. ADXL345 measures acceleration in 3 axes
 * 2. Gravity provides 1g baseline on Z-axis when level
 * 3. Motor vibrations add AC component to DC gravity signal
 * 4. Calculate vibration magnitude: sqrt(x² + y² + z²) - 1g
 * 5. Analyze frequency and amplitude to detect issues
 *
 * MEASUREMENT RANGE:
 * - Sensitivity: ±2g, ±4g, ±8g, ±16g (selectable)
 * - Resolution: 13-bit (8192 levels for ±16g)
 * - Sample rate: Up to 3200 Hz
 * - For motor: ±2g range is sufficient
 *
 * EXPECTED RESULTS:
 * - Motor OFF: Vibration ≈ 0.01-0.05g (noise floor)
 * - Motor running low speed: 0.1-0.3g
 * - Motor running high speed: 0.3-0.8g
 * - Values >1g indicate serious vibration issues
 *
 * LIBRARY REQUIRED:
 * Adafruit ADXL345 Library
 * Install via: Tools > Manage Libraries > Search "ADXL345"
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// Create ADXL345 object with unique ID
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Motor control
const int MOTOR_PWM_PIN = 9;
int motorPWM = 0;

// Vibration variables
float vibration_X = 0.0;
float vibration_Y = 0.0;
float vibration_Z = 0.0;
float vibration_magnitude = 0.0;
float vibration_RMS = 0.0;

// Baseline (motor off)
float baseline_X = 0.0;
float baseline_Y = 0.0;
float baseline_Z = 0.0;
bool baselineSet = false;

// Statistics
float maxVibration = 0.0;
float minVibration = 9999.0;
float avgVibration = 0.0;
unsigned long sampleCount = 0;

// Peak detection
float peakVibration_X = 0.0;
float peakVibration_Y = 0.0;
float peakVibration_Z = 0.0;

// Timing
unsigned long lastPrint = 0;
const unsigned int PRINT_INTERVAL = 250;  // Print every 250ms

// Auto test
bool autoTest = false;

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_PWM_PIN, OUTPUT);
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("Vibration Sensor Test - ADXL345"));
  Serial.println(F("========================================"));
  Serial.println();

  // Initialize ADXL345
  Serial.print(F("Initializing ADXL345... "));
  if (!accel.begin()) {
    Serial.println(F("FAILED!"));
    Serial.println(F("ERROR: ADXL345 not found on I2C bus"));
    Serial.println(F("Check:"));
    Serial.println(F("  - I2C connections (SDA->A4, SCL->A5)"));
    Serial.println(F("  - Power (VCC, GND)"));
    Serial.println(F("  - I2C address (default 0x53)"));
    while (1) { delay(10); }
  }
  Serial.println(F("SUCCESS!"));

  // Display sensor details
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println(F("\nSensor Info:"));
  Serial.print(F("  Name:       ")); Serial.println(sensor.name);
  Serial.print(F("  Version:    ")); Serial.println(sensor.version);
  Serial.print(F("  Sensor ID:  ")); Serial.println(sensor.sensor_id);
  Serial.print(F("  Max Value:  ")); Serial.print(sensor.max_value); Serial.println(F(" m/s²"));
  Serial.print(F("  Min Value:  ")); Serial.print(sensor.min_value); Serial.println(F(" m/s²"));
  Serial.print(F("  Resolution: ")); Serial.print(sensor.resolution); Serial.println(F(" m/s²"));

  // Set range to ±2g (most sensitive for small motors)
  accel.setRange(ADXL345_RANGE_2_G);
  Serial.println(F("\nConfiguration:"));
  Serial.println(F("  Range: ±2g"));
  Serial.println(F("  Data rate: 100 Hz"));

  Serial.println(F("\nMOUNTING VERIFICATION:"));
  Serial.println(F("  - Sensor must be RIGIDLY mounted"));
  Serial.println(F("  - Position 2-5cm from motor"));
  Serial.println(F("  - Mark orientation for repeatability"));
  Serial.println(F("  - Z-axis should show ~1g (9.8 m/s²) when level"));

  delay(500);

  // Measure baseline (motor off)
  Serial.println(F("\nCalibrating baseline (motor OFF)..."));
  calibrateBaseline();

  Serial.println(F("\nCOMMANDS:"));
  Serial.println(F("  s - Start motor (PWM=100)"));
  Serial.println(F("  a - Auto PWM sweep"));
  Serial.println(F("  + - Increase PWM"));
  Serial.println(F("  - - Decrease PWM"));
  Serial.println(F("  x - Stop motor"));
  Serial.println(F("  c - Recalibrate baseline"));
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

  // Read sensor continuously
  readVibration();
  updateStatistics();

  // Print at interval
  unsigned long currentTime = millis();
  if (currentTime - lastPrint >= PRINT_INTERVAL) {
    displayData();
    lastPrint = currentTime;
  }

  // Auto test mode
  if (autoTest) {
    static unsigned long lastChange = 0;
    if (currentTime - lastChange >= 4000) {  // Every 4 seconds
      motorPWM += 25;
      if (motorPWM > 255) motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      lastChange = currentTime;
      Serial.print(F("\n>>> Auto PWM -> "));
      Serial.println(motorPWM);
      delay(1000);  // Settle time
    }
  }

  delay(10);  // Small delay for stability
}

// Calibrate baseline vibration (motor off)
void calibrateBaseline() {
  const int samples = 100;
  float sumX = 0, sumY = 0, sumZ = 0;

  for (int i = 0; i < samples; i++) {
    sensors_event_t event;
    accel.getEvent(&event);
    sumX += event.acceleration.x;
    sumY += event.acceleration.y;
    sumZ += event.acceleration.z;
    delay(10);
  }

  baseline_X = sumX / samples;
  baseline_Y = sumY / samples;
  baseline_Z = sumZ / samples;
  baselineSet = true;

  Serial.println(F("Baseline established:"));
  Serial.print(F("  X: ")); Serial.print(baseline_X, 3); Serial.println(F(" m/s²"));
  Serial.print(F("  Y: ")); Serial.print(baseline_Y, 3); Serial.println(F(" m/s²"));
  Serial.print(F("  Z: ")); Serial.print(baseline_Z, 3); Serial.println(F(" m/s² (should be ~9.8)"));

  // Check if sensor is level
  if (abs(baseline_Z - 9.8) > 2.0) {
    Serial.println(F("  WARNING: Z-axis far from 9.8 m/s² - sensor may be tilted"));
  }
}

// Read vibration from sensor
void readVibration() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Subtract baseline to get vibration component
  vibration_X = event.acceleration.x - baseline_X;
  vibration_Y = event.acceleration.y - baseline_Y;
  vibration_Z = event.acceleration.z - baseline_Z;

  // Calculate magnitude (Euclidean norm)
  vibration_magnitude = sqrt(vibration_X * vibration_X +
                             vibration_Y * vibration_Y +
                             vibration_Z * vibration_Z);

  // Track peaks
  if (abs(vibration_X) > abs(peakVibration_X)) peakVibration_X = vibration_X;
  if (abs(vibration_Y) > abs(peakVibration_Y)) peakVibration_Y = vibration_Y;
  if (abs(vibration_Z) > abs(peakVibration_Z)) peakVibration_Z = vibration_Z;
}

void updateStatistics() {
  sampleCount++;

  if (vibration_magnitude > maxVibration) {
    maxVibration = vibration_magnitude;
  }
  if (vibration_magnitude < minVibration) {
    minVibration = vibration_magnitude;
  }

  // Running average
  avgVibration = avgVibration + (vibration_magnitude - avgVibration) / sampleCount;
}

void displayData() {
  static int headerCount = 0;
  if (headerCount % 20 == 0) {
    Serial.println();
    Serial.println(F("Time\tPWM\tX(m/s²)\tY(m/s²)\tZ(m/s²)\tMag\tAvg\tMax\tStatus"));
    Serial.println(F("----\t---\t-------\t-------\t-------\t---\t---\t---\t------"));
  }
  headerCount++;

  Serial.print(millis() / 1000);
  Serial.print(F("\t"));
  Serial.print(motorPWM);
  Serial.print(F("\t"));
  Serial.print(vibration_X, 3);
  Serial.print(F("\t"));
  Serial.print(vibration_Y, 3);
  Serial.print(F("\t"));
  Serial.print(vibration_Z, 3);
  Serial.print(F("\t"));
  Serial.print(vibration_magnitude, 3);
  Serial.print(F("\t"));
  Serial.print(avgVibration, 3);
  Serial.print(F("\t"));
  Serial.print(maxVibration, 3);
  Serial.print(F("\t"));

  // Status
  if (vibration_magnitude < 0.1) {
    Serial.print(F("Low"));
  } else if (vibration_magnitude < 0.5) {
    Serial.print(F("Normal"));
  } else if (vibration_magnitude < 1.0) {
    Serial.print(F("High"));
  } else {
    Serial.print(F("CRITICAL"));
  }

  Serial.println();
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 's':
    case 'S':
      motorPWM = 100;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      resetStatistics();
      Serial.println(F("\n>>> Motor STARTED (PWM=100)"));
      delay(500);  // Settling time
      break;

    case 'a':
    case 'A':
      autoTest = !autoTest;
      Serial.print(F("\n>>> Auto test: "));
      Serial.println(autoTest ? F("ON") : F("OFF"));
      if (autoTest) {
        motorPWM = 50;
        resetStatistics();
      }
      break;

    case '+':
      motorPWM = constrain(motorPWM + 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      delay(500);
      break;

    case '-':
      motorPWM = constrain(motorPWM - 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      delay(500);
      break;

    case 'x':
    case 'X':
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      autoTest = false;
      Serial.println(F("\n>>> Motor STOPPED"));
      printSummary();
      break;

    case 'c':
    case 'C':
      if (motorPWM == 0) {
        Serial.println(F("\n>>> Recalibrating baseline..."));
        calibrateBaseline();
      } else {
        Serial.println(F("\n>>> ERROR: Stop motor first (x)"));
      }
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
  maxVibration = 0.0;
  minVibration = 9999.0;
  avgVibration = 0.0;
  sampleCount = 0;
  peakVibration_X = 0.0;
  peakVibration_Y = 0.0;
  peakVibration_Z = 0.0;
}

void printSummary() {
  Serial.println(F("\n========== VIBRATION SUMMARY =========="));
  Serial.print(F("Average:     ")); Serial.print(avgVibration, 3); Serial.println(F(" m/s²"));
  Serial.print(F("Maximum:     ")); Serial.print(maxVibration, 3); Serial.println(F(" m/s²"));
  Serial.print(F("Peak X:      ")); Serial.print(peakVibration_X, 3); Serial.println(F(" m/s²"));
  Serial.print(F("Peak Y:      ")); Serial.print(peakVibration_Y, 3); Serial.println(F(" m/s²"));
  Serial.print(F("Peak Z:      ")); Serial.print(peakVibration_Z, 3); Serial.println(F(" m/s²"));
  Serial.print(F("Assessment:  "));

  if (maxVibration < 0.2) {
    Serial.println(F("EXCELLENT - Very smooth"));
  } else if (maxVibration < 0.5) {
    Serial.println(F("GOOD - Normal vibration"));
  } else if (maxVibration < 1.0) {
    Serial.println(F("WARNING - High vibration"));
  } else {
    Serial.println(F("CRITICAL - Excessive vibration!"));
  }

  Serial.println(F("=======================================\n"));
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("s - Start motor (PWM=100)"));
  Serial.println(F("a - Auto PWM sweep"));
  Serial.println(F("+ - Increase PWM (+25)"));
  Serial.println(F("- - Decrease PWM (-25)"));
  Serial.println(F("x - Stop motor & summary"));
  Serial.println(F("c - Calibrate baseline"));
  Serial.println(F("r - Reset statistics"));
  Serial.println(F("? - Show help"));
  Serial.println(F("==============================\n"));
}

/*
 * VERIFICATION CHECKLIST:
 *
 * [ ] ADXL345 detected on I2C (0x53)
 * [ ] Z-axis reads ~9.8 m/s² when sensor is level
 * [ ] Sensor is rigidly mounted (no independent movement)
 * [ ] Baseline calibration shows low values (<0.05 m/s²)
 * [ ] Vibration increases when motor speed increases
 * [ ] Vibration returns to baseline when motor stops
 * [ ] Sensor position is marked for repeatability
 *
 * EXPECTED VIBRATION LEVELS (to fill after testing):
 * Motor OFF:       _____ m/s²
 * PWM=50:          _____ m/s²
 * PWM=100:         _____ m/s²
 * PWM=150:         _____ m/s²
 * PWM=200:         _____ m/s²
 * PWM=255:         _____ m/s²
 *
 * Dominant axis:   [ ] X [ ] Y [ ] Z [ ] Balanced
 *
 * TROUBLESHOOTING:
 *
 * ADXL345 not found:
 * - Check I2C wiring
 * - Run I2C scanner
 * - Some modules need 3.3V only
 *
 * Z-axis not near 9.8 m/s²:
 * - Sensor is tilted
 * - Level the sensor or recalculate baseline
 *
 * No vibration detected:
 * - Sensor may not be mounted rigidly enough
 * - Try closer distance to motor (2cm)
 * - Check if motor is actually running
 *
 * Vibration too high even when motor off:
 * - Sensor mounted on unstable surface
 * - External vibrations (fans, AC, etc.)
 * - Recalibrate baseline
 *
 * One axis dominates:
 * - May indicate shaft misalignment
 * - Or unbalanced load on shaft
 * - Normal for some motor orientations
 *
 * Vibration increases over time:
 * - Bearings wearing out
 * - Motor heating causing expansion
 * - Document for health monitoring
 *
 * NOTES:
 * - For frequency analysis, implement FFT (arduinoFFT library)
 * - Vibration patterns can reveal specific mechanical issues
 * - Baseline should be recalibrated if sensor is moved
 * - Log data for trend analysis (early fault detection)
 */
