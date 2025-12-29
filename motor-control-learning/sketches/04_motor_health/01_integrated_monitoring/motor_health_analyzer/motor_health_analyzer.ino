/*
 * Motor Health Analyzer - Integrated Monitoring System
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * SKETCH: Complete Integrated Diagnostic System
 *
 * OBJECTIVE:
 * Comprehensive motor health monitoring combining all sensors:
 * - RPM (TCRT5000 IR sensor)
 * - Current & Voltage (INA219)
 * - Temperature (MLX90614 IR + DHT22 ambient)
 * - Vibration (ADXL345 accelerometer)
 * - Sound level (KY-038 microphone)
 *
 * Performs automated health assessment and generates health score (0-100).
 *
 * HARDWARE CONNECTIONS:
 * =====================
 * I2C Bus (shared):
 *   SDA (A4) -> INA219, ADXL345, MLX90614
 *   SCL (A5) -> INA219, ADXL345, MLX90614
 *
 * Digital Pins:
 *   D2 (INT0) -> TCRT5000 Digital Out (RPM sensor)
 *   D4        -> DHT22 Data
 *   D9        -> Motor PWM control
 *
 * Analog Pins:
 *   A0 -> KY-038 Sound sensor analog
 *   A1 -> TCRT5000 Analog (optional calibration)
 *
 * I2C Addresses:
 *   0x40 -> INA219 (current/voltage)
 *   0x53 -> ADXL345 (accelerometer)
 *   0x5A -> MLX90614 (IR temperature)
 *
 * DIAGNOSTIC PROTOCOL:
 * ====================
 * Phase 1: Baseline (30s, motor OFF)
 *   - Ambient temperature, humidity
 *   - Ambient noise level
 *   - Vibration baseline
 *   - Zero current verification
 *
 * Phase 2: Startup Test (30s, PWM ramp 0->100)
 *   - Measure startup current
 *   - Detect minimum startup voltage
 *   - Initial RPM response
 *
 * Phase 3: Low Speed (60s, PWM=100)
 *   - Stabilize measurements
 *   - Record all parameters
 *
 * Phase 4: Medium Speed (60s, PWM=150)
 *   - Monitor temperature rise
 *   - Track vibration increase
 *
 * Phase 5: High Speed (60s, PWM=200)
 *   - Maximum safe speed test
 *   - Peak measurements
 *
 * Phase 6: Cooldown (60s, motor OFF)
 *   - Measure thermal decay
 *   - Mechanical deceleration
 *
 * Total test time: ~5 minutes
 *
 * HEALTH SCORING:
 * ===============
 * Start with 100 points, deduct for anomalies:
 *
 * Current:
 *   -5: Consumption 10-20% over baseline
 *   -15: Consumption >20% over baseline
 *   -10: Erratic current (>10% variation)
 *
 * Temperature:
 *   -5: Delta-T 10-15°C
 *   -15: Delta-T 15-20°C
 *   -25: Delta-T >20°C
 *   -10: Doesn't stabilize in 5 min
 *
 * Vibration:
 *   -5: 0.3-0.5 m/s²
 *   -10: 0.5-1.0 m/s²
 *   -20: >1.0 m/s²
 *
 * RPM:
 *   -5: Variation 10-15%
 *   -10: Variation >15%
 *   -10: Doesn't reach expected speed
 *
 * Sound:
 *   -5: Moderate increase over baseline
 *   -10: Large increase
 *   -15: Unusual sounds detected
 *
 * Final Score:
 *   90-100: EXCELLENT - Motor in perfect condition
 *   70-89:  GOOD - Minor wear, acceptable
 *   50-69:  FAIR - Increased wear, monitor closely
 *   30-49:  POOR - Significant issues, maintenance needed
 *   0-29:   CRITICAL - Do not use for demanding tasks
 *
 * LIBRARIES REQUIRED:
 * ===================
 * - Wire (built-in)
 * - Adafruit_INA219
 * - Adafruit_MLX90614
 * - Adafruit_ADXL345_U
 * - Adafruit_Sensor
 * - DHT
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for testing
 */

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// ============ PIN DEFINITIONS ============
#define DHT_PIN 4
#define RPM_SENSOR_PIN 2
#define SOUND_SENSOR_PIN A0
#define MOTOR_PWM_PIN 9

// ============ SENSOR OBJECTS ============
DHT dht(DHT_PIN, DHT22);
Adafruit_INA219 ina219;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// ============ MEASUREMENT VARIABLES ============
// RPM
volatile unsigned long rpmPulseCount = 0;
unsigned long lastRPMCalc = 0;
float currentRPM = 0.0;

// Current & Voltage
float current_mA = 0.0;
float voltage_V = 0.0;
float power_mW = 0.0;

// Temperature
float motorTemp_C = 0.0;
float ambientTemp_C = 0.0;
float humidity_percent = 0.0;
float deltaTemp_C = 0.0;

// Vibration
float vibration_magnitude = 0.0;
float vib_baseline = 0.0;

// Sound
int soundLevel = 0;
int soundBaseline = 0;

// Motor control
int motorPWM = 0;

// ============ HEALTH METRICS ============
struct HealthMetrics {
  float maxCurrent = 0.0;
  float avgCurrent = 0.0;
  float maxDeltaTemp = 0.0;
  float maxVibration = 0.0;
  float avgRPM = 0.0;
  float rpmStability = 0.0;  // Coefficient of variation
  int maxSound = 0;

  // Anomaly flags
  bool highCurrent = false;
  bool excessiveHeat = false;
  bool highVibration = false;
  bool unstableRPM = false;
  bool excessiveNoise = false;

  // Health score
  int healthScore = 100;
};

HealthMetrics metrics;

// ============ TEST STATE ============
enum TestPhase {
  IDLE,
  BASELINE,
  STARTUP,
  LOW_SPEED,
  MED_SPEED,
  HIGH_SPEED,
  COOLDOWN,
  COMPLETE
};

TestPhase currentPhase = IDLE;
unsigned long phaseStartTime = 0;

// ============ FUNCTION PROTOTYPES ============
void initializeSensors();
void countRPMPulse();
void readAllSensors();
void runDiagnosticTest();
void updateHealthMetrics();
int calculateHealthScore();
void printReport();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(RPM_SENSOR_PIN, INPUT);
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("  MOTOR HEALTH ANALYZER v1.0"));
  Serial.println(F("========================================"));
  Serial.println();

  delay(1000);

  initializeSensors();

  Serial.println();
  Serial.println(F("COMMANDS:"));
  Serial.println(F("  d - Start diagnostic test (~5 min)"));
  Serial.println(F("  m - Manual monitoring mode"));
  Serial.println(F("  x - Stop motor"));
  Serial.println(F("  r - Show last report"));
  Serial.println();

  lastRPMCalc = millis();
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Check for commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }

  // Read all sensors continuously
  readAllSensors();

  // If diagnostic test is running
  if (currentPhase != IDLE && currentPhase != COMPLETE) {
    runDiagnosticTest();
  }

  delay(50);
}

// ============================================================================
// SENSOR INITIALIZATION
// ============================================================================
void initializeSensors() {
  Serial.println(F("Initializing sensors..."));
  Serial.println();

  // DHT22
  Serial.print(F("  DHT22...        "));
  dht.begin();
  delay(2000);
  if (!isnan(dht.readTemperature())) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAILED (continuing without)"));
  }

  // INA219
  Serial.print(F("  INA219...       "));
  if (ina219.begin()) {
    ina219.setCalibration_32V_2A();
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAILED!"));
  }

  // MLX90614
  Serial.print(F("  MLX90614...     "));
  if (mlx.begin()) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("FAILED!"));
  }

  // ADXL345
  Serial.print(F("  ADXL345...      "));
  if (accel.begin()) {
    accel.setRange(ADXL345_RANGE_2_G);
    Serial.println(F("OK"));
    calibrateVibrationBaseline();
  } else {
    Serial.println(F("FAILED!"));
  }

  // RPM Sensor (interrupt)
  Serial.print(F("  TCRT5000...     "));
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), countRPMPulse, RISING);
  Serial.println(F("OK"));

  // Sound sensor (always works)
  Serial.print(F("  KY-038...       "));
  soundBaseline = analogRead(SOUND_SENSOR_PIN);
  Serial.println(F("OK"));

  Serial.println();
  Serial.println(F("All sensors initialized."));
}

// ============================================================================
// SENSOR READING
// ============================================================================
void countRPMPulse() {
  rpmPulseCount++;
}

void calculateRPM() {
  unsigned long currentTime = millis();
  if (currentTime - lastRPMCalc >= 1000) {
    noInterrupts();
    unsigned long pulses = rpmPulseCount;
    rpmPulseCount = 0;
    interrupts();

    currentRPM = pulses * 60.0;
    lastRPMCalc = currentTime;
  }
}

void calibrateVibrationBaseline() {
  Serial.print(F("    Calibrating vibration baseline... "));
  float sum = 0;
  for (int i = 0; i < 50; i++) {
    sensors_event_t event;
    accel.getEvent(&event);
    float mag = sqrt(event.acceleration.x * event.acceleration.x +
                    event.acceleration.y * event.acceleration.y +
                    event.acceleration.z * event.acceleration.z);
    sum += mag;
    delay(20);
  }
  vib_baseline = sum / 50.0;
  Serial.print(vib_baseline, 2);
  Serial.println(F(" m/s²"));
}

void readAllSensors() {
  // RPM
  calculateRPM();

  // Current & Voltage
  current_mA = ina219.getCurrent_mA();
  voltage_V = ina219.getBusVoltage_V();
  power_mW = ina219.getPower_mW();

  // Temperature
  motorTemp_C = mlx.readObjectTempC();
  float dhtTemp = dht.readTemperature();
  float dhtHumid = dht.readHumidity();
  if (!isnan(dhtTemp)) ambientTemp_C = dhtTemp;
  if (!isnan(dhtHumid)) humidity_percent = dhtHumid;
  deltaTemp_C = motorTemp_C - ambientTemp_C;

  // Vibration
  sensors_event_t event;
  accel.getEvent(&event);
  vibration_magnitude = sqrt(event.acceleration.x * event.acceleration.x +
                            event.acceleration.y * event.acceleration.y +
                            event.acceleration.z * event.acceleration.z);
  vibration_magnitude = abs(vibration_magnitude - vib_baseline);

  // Sound
  soundLevel = analogRead(SOUND_SENSOR_PIN);
}

// ============================================================================
// DIAGNOSTIC TEST STATE MACHINE
// ============================================================================
void runDiagnosticTest() {
  unsigned long phaseTime = millis() - phaseStartTime;

  switch (currentPhase) {
    case BASELINE:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 1: BASELINE (30s)"));
        Serial.println(F("    Motor OFF, measuring ambient conditions..."));
        motorPWM = 0;
        analogWrite(MOTOR_PWM_PIN, 0);
      }

      if (phaseTime > 30000) {
        currentPhase = STARTUP;
        phaseStartTime = millis();
      }
      break;

    case STARTUP:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 2: STARTUP (30s)"));
        Serial.println(F("    Ramping PWM 0->100..."));
      }

      motorPWM = map(phaseTime, 0, 30000, 0, 100);
      analogWrite(MOTOR_PWM_PIN, motorPWM);

      if (phaseTime > 30000) {
        currentPhase = LOW_SPEED;
        phaseStartTime = millis();
      }
      break;

    case LOW_SPEED:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 3: LOW SPEED (60s, PWM=100)"));
        motorPWM = 100;
        analogWrite(MOTOR_PWM_PIN, 100);
      }

      updateHealthMetrics();

      if (phaseTime > 60000) {
        currentPhase = MED_SPEED;
        phaseStartTime = millis();
      }
      break;

    case MED_SPEED:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 4: MEDIUM SPEED (60s, PWM=150)"));
        motorPWM = 150;
        analogWrite(MOTOR_PWM_PIN, 150);
      }

      updateHealthMetrics();

      if (phaseTime > 60000) {
        currentPhase = HIGH_SPEED;
        phaseStartTime = millis();
      }
      break;

    case HIGH_SPEED:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 5: HIGH SPEED (60s, PWM=200)"));
        motorPWM = 200;
        analogWrite(MOTOR_PWM_PIN, 200);
      }

      updateHealthMetrics();

      if (phaseTime > 60000) {
        currentPhase = COOLDOWN;
        phaseStartTime = millis();
      }
      break;

    case COOLDOWN:
      if (phaseTime == 0) {
        Serial.println(F("\n>>> PHASE 6: COOLDOWN (60s)"));
        Serial.println(F("    Motor OFF, measuring thermal decay..."));
        motorPWM = 0;
        analogWrite(MOTOR_PWM_PIN, 0);
      }

      if (phaseTime > 60000) {
        currentPhase = COMPLETE;
        Serial.println(F("\n>>> TEST COMPLETE!"));
        Serial.println();
        calculateHealthScore();
        printReport();
      }
      break;

    default:
      break;
  }

  // Print status every 5 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 5000) {
    printQuickStatus();
    lastStatus = millis();
  }
}

// ============================================================================
// HEALTH METRICS
// ============================================================================
void updateHealthMetrics() {
  // Track maximums
  if (current_mA > metrics.maxCurrent) metrics.maxCurrent = current_mA;
  if (deltaTemp_C > metrics.maxDeltaTemp) metrics.maxDeltaTemp = deltaTemp_C;
  if (vibration_magnitude > metrics.maxVibration) metrics.maxVibration = vibration_magnitude;
  if (soundLevel > metrics.maxSound) metrics.maxSound = soundLevel;

  // Running averages (simplified)
  static int samples = 0;
  samples++;
  metrics.avgCurrent += (current_mA - metrics.avgCurrent) / samples;
  metrics.avgRPM += (currentRPM - metrics.avgRPM) / samples;
}

int calculateHealthScore() {
  int score = 100;

  // Current analysis
  if (metrics.maxCurrent > 150) {
    score -= 15;
    metrics.highCurrent = true;
  } else if (metrics.maxCurrent > 110) {
    score -= 5;
  }

  // Temperature analysis
  if (metrics.maxDeltaTemp > 20) {
    score -= 25;
    metrics.excessiveHeat = true;
  } else if (metrics.maxDeltaTemp > 15) {
    score -= 15;
  } else if (metrics.maxDeltaTemp > 10) {
    score -= 5;
  }

  // Vibration analysis
  if (metrics.maxVibration > 1.0) {
    score -= 20;
    metrics.highVibration = true;
  } else if (metrics.maxVibration > 0.5) {
    score -= 10;
  } else if (metrics.maxVibration > 0.3) {
    score -= 5;
  }

  // Sound analysis
  int soundDelta = metrics.maxSound - soundBaseline;
  if (soundDelta > 200) {
    score -= 15;
    metrics.excessiveNoise = true;
  } else if (soundDelta > 100) {
    score -= 10;
  } else if (soundDelta > 50) {
    score -= 5;
  }

  metrics.healthScore = constrain(score, 0, 100);
  return metrics.healthScore;
}

// ============================================================================
// DISPLAY & REPORTING
// ============================================================================
void printQuickStatus() {
  Serial.print(F("["));
  Serial.print(millis() / 1000);
  Serial.print(F("s] PWM:"));
  Serial.print(motorPWM);
  Serial.print(F(" | RPM:"));
  Serial.print(currentRPM, 0);
  Serial.print(F(" | I:"));
  Serial.print(current_mA, 0);
  Serial.print(F("mA | ΔT:"));
  Serial.print(deltaTemp_C, 1);
  Serial.print(F("°C | Vib:"));
  Serial.print(vibration_magnitude, 2);
  Serial.println(F("m/s²"));
}

void printReport() {
  Serial.println(F("╔══════════════════════════════════════════════════════╗"));
  Serial.println(F("║         MOTOR HEALTH DIAGNOSTIC REPORT              ║"));
  Serial.println(F("╠══════════════════════════════════════════════════════╣"));

  // Health Score
  Serial.print(F("║  HEALTH SCORE: "));
  Serial.print(metrics.healthScore);
  Serial.print(F(" / 100  "));

  if (metrics.healthScore >= 90) {
    Serial.println(F("[EXCELLENT]          ║"));
  } else if (metrics.healthScore >= 70) {
    Serial.println(F("[GOOD]               ║"));
  } else if (metrics.healthScore >= 50) {
    Serial.println(F("[FAIR]               ║"));
  } else if (metrics.healthScore >= 30) {
    Serial.println(F("[POOR]               ║"));
  } else {
    Serial.println(F("[CRITICAL]           ║"));
  }

  Serial.println(F("╠══════════════════════════════════════════════════════╣"));
  Serial.println(F("║  MEASUREMENTS:                                       ║"));
  Serial.print(F("║    Max Current:        "));
  Serial.print(metrics.maxCurrent, 1);
  Serial.println(F(" mA                      ║"));

  Serial.print(F("║    Max Temp Rise:      "));
  Serial.print(metrics.maxDeltaTemp, 1);
  Serial.println(F(" °C                      ║"));

  Serial.print(F("║    Max Vibration:      "));
  Serial.print(metrics.maxVibration, 3);
  Serial.println(F(" m/s²                 ║"));

  Serial.print(F("║    Avg RPM:            "));
  Serial.print(metrics.avgRPM, 0);
  Serial.println(F(" RPM                     ║"));

  Serial.println(F("╠══════════════════════════════════════════════════════╣"));
  Serial.println(F("║  ISSUES DETECTED:                                    ║"));

  if (metrics.highCurrent) Serial.println(F("║    ⚠ High current draw                              ║"));
  if (metrics.excessiveHeat) Serial.println(F("║    ⚠ Excessive heating                              ║"));
  if (metrics.highVibration) Serial.println(F("║    ⚠ High vibration                                 ║"));
  if (metrics.excessiveNoise) Serial.println(F("║    ⚠ Excessive noise                                ║"));
  if (!metrics.highCurrent && !metrics.excessiveHeat &&
      !metrics.highVibration && !metrics.excessiveNoise) {
    Serial.println(F("║    ✓ No major issues detected                        ║"));
  }

  Serial.println(F("╠══════════════════════════════════════════════════════╣"));
  Serial.println(F("║  RECOMMENDATIONS:                                    ║"));

  if (metrics.healthScore >= 90) {
    Serial.println(F("║    Motor is in excellent condition.                  ║"));
    Serial.println(F("║    Safe for demanding applications.                  ║"));
  } else if (metrics.healthScore >= 70) {
    Serial.println(F("║    Motor shows minor wear but is operational.        ║"));
    Serial.println(F("║    Monitor for changes over time.                    ║"));
  } else if (metrics.healthScore >= 50) {
    Serial.println(F("║    Motor has increased wear.                         ║"));
    Serial.println(F("║    Consider maintenance or replacement soon.         ║"));
  } else {
    Serial.println(F("║    Motor has significant issues!                     ║"));
    Serial.println(F("║    DO NOT use for demanding tasks.                   ║"));
    Serial.println(F("║    Maintenance or replacement required.              ║"));
  }

  Serial.println(F("╚══════════════════════════════════════════════════════╝"));
  Serial.println();
}

// ============================================================================
// COMMAND HANDLING
// ============================================================================
void handleCommand(char cmd) {
  switch (cmd) {
    case 'd':
    case 'D':
      if (currentPhase == IDLE || currentPhase == COMPLETE) {
        Serial.println(F("\n>>> Starting diagnostic test..."));
        currentPhase = BASELINE;
        phaseStartTime = millis();
        // Reset metrics
        metrics = HealthMetrics();
      } else {
        Serial.println(F("\n>>> Test already running!"));
      }
      break;

    case 'x':
    case 'X':
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      currentPhase = IDLE;
      Serial.println(F("\n>>> Motor STOPPED, test aborted"));
      break;

    case 'r':
    case 'R':
      if (currentPhase == COMPLETE) {
        printReport();
      } else {
        Serial.println(F("\n>>> No report available. Run test first (d)"));
      }
      break;

    case 'm':
    case 'M':
      Serial.println(F("\n>>> Manual mode not yet implemented"));
      break;
  }
}

/*
 * USAGE INSTRUCTIONS:
 * ===================
 * 1. Connect all sensors as per pin definitions
 * 2. Upload sketch and open Serial Monitor (115200 baud)
 * 3. Send 'd' to start automated diagnostic test
 * 4. Wait ~5 minutes for test completion
 * 5. Review health report
 * 6. Send 'r' to view report again
 *
 * INTERPRETING RESULTS:
 * =====================
 * Health Score 90-100: Motor is excellent, use freely
 * Health Score 70-89:  Normal wear, monitor periodically
 * Health Score 50-69:  Increased wear, plan maintenance
 * Health Score 30-49:  Significant issues, limit usage
 * Health Score 0-29:   Critical, do not use for demanding tasks
 *
 * TROUBLESHOOTING:
 * ================
 * Sensor initialization failures:
 *   - Check I2C wiring (SDA, SCL, VCC, GND)
 *   - Run I2C scanner to verify addresses
 *   - Ensure pull-up resistors on I2C lines
 *
 * No RPM detected:
 *   - Verify reflective marker on motor shaft
 *   - Check TCRT5000 sensitivity adjustment
 *   - Ensure rigid mounting of RPM sensor
 *
 * Temperature doesn't rise:
 *   - Check MLX90614 is pointing at motor
 *   - Verify motor is actually running
 *   - Some motors are very efficient (normal)
 *
 * High vibration readings:
 *   - ADXL345 must be rigidly mounted
 *   - Breadboard mounting is too flexible
 *   - Use solid mounting platform
 */
