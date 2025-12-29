/*
 * Temperature Sensor Test - MLX90614 IR + DHT22 (Motor Health Monitoring)
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * TEST: Temperature Measurement (Motor + Ambient)
 *
 * OBJECTIVE:
 * Test dual temperature measurement system:
 * 1. MLX90614 - Non-contact IR temperature sensor for motor surface
 * 2. DHT22 - Ambient temperature and humidity sensor
 * This allows calculating motor temperature rise (Delta-T) which indicates
 * mechanical friction, bearing wear, or excessive load.
 *
 * HARDWARE SETUP:
 * MLX90614 IR Sensor:
 * - VCC -> Arduino 5V (3.3V also works)
 * - GND -> Arduino GND
 * - SDA -> Arduino A4 (with 4.7K pull-up to VCC)
 * - SCL -> Arduino A5 (with 4.7K pull-up to VCC)
 * - I2C Address: 0x5A (default)
 *
 * DHT22 Sensor:
 * - VCC -> Arduino 5V
 * - GND -> Arduino GND
 * - DATA -> Arduino D4 (with 10K pull-up to VCC, often on module)
 *
 * Motor Control:
 * - PWM Pin D9 -> Motor control (via MOSFET)
 *
 * SENSOR POSITIONING:
 * MLX90614:
 * - Point at motor body (not shaft)
 * - Distance: 5-10cm for best accuracy
 * - Field of view: ~90° cone
 * - Avoid reflective surfaces (they read ambient temp)
 * - Mark position for repeatable measurements
 *
 * DHT22:
 * - Position away from motor heat (20-30cm)
 * - Good air circulation
 * - Not in direct sunlight or near heat sources
 *
 * HOW IT WORKS:
 * MLX90614:
 * - Measures infrared radiation emitted by motor
 * - Provides object temperature (motor) and ambient temp
 * - No contact needed - ideal for rotating equipment
 * - Resolution: 0.02°C, Accuracy: ±0.5°C
 *
 * DHT22:
 * - Measures air temperature and humidity
 * - Provides baseline ambient conditions
 * - Resolution: 0.1°C, Accuracy: ±0.5°C
 * - Also measures humidity (useful for correlation)
 *
 * EXPECTED RESULTS:
 * - Motor off: object temp ≈ ambient temp
 * - Motor running 5 min @ 5V: Delta-T = 5-10°C (healthy motor)
 * - Delta-T > 15°C may indicate problems
 * - Temperature should stabilize within 5 minutes
 *
 * LIBRARIES REQUIRED:
 * - Adafruit MLX90614 Library
 * - DHT sensor library by Adafruit
 * Install via: Tools > Manage Libraries
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <DHT.h>

// DHT22 configuration
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// MLX90614 configuration
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Motor control
const int MOTOR_PWM_PIN = 9;
int motorPWM = 0;

// Temperature variables
float motorTemp_C = 0.0;       // Motor surface temperature (IR)
float ambientTemp_MLX_C = 0.0; // Ambient temp from MLX90614
float ambientTemp_DHT_C = 0.0; // Ambient temp from DHT22
float humidity_percent = 0.0;   // Relative humidity
float deltaTemp_C = 0.0;        // Motor temp - Ambient temp

// Statistics
float maxMotorTemp = 0.0;
float maxDeltaTemp = 0.0;
unsigned long motorStartTime = 0;
bool motorRunning = false;

// Timing
unsigned long lastRead = 0;
const unsigned int READ_INTERVAL = 2000;  // Read every 2 seconds (DHT22 limitation)

// Auto test
bool autoTest = false;

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_PWM_PIN, OUTPUT);
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("Temperature Sensor Test"));
  Serial.println(F("MLX90614 (IR) + DHT22 (Ambient)"));
  Serial.println(F("========================================"));
  Serial.println();

  // Initialize MLX90614
  Serial.print(F("Initializing MLX90614 IR sensor... "));
  if (!mlx.begin()) {
    Serial.println(F("FAILED!"));
    Serial.println(F("ERROR: MLX90614 not found on I2C bus"));
    Serial.println(F("Check:"));
    Serial.println(F("  - I2C connections (SDA->A4, SCL->A5)"));
    Serial.println(F("  - Pull-up resistors (4.7K on SDA/SCL)"));
    Serial.println(F("  - Power (VCC, GND)"));
    Serial.println(F("  - I2C address (default 0x5A)"));
    while (1) { delay(10); }
  }
  Serial.println(F("SUCCESS!"));

  // Initialize DHT22
  Serial.print(F("Initializing DHT22 sensor... "));
  dht.begin();
  // DHT doesn't have a proper begin() check, do first read
  delay(2000);  // DHT needs 2s to stabilize
  float testRead = dht.readTemperature();
  if (isnan(testRead)) {
    Serial.println(F("FAILED!"));
    Serial.println(F("WARNING: DHT22 not responding"));
    Serial.println(F("Check:"));
    Serial.println(F("  - Connection to D4"));
    Serial.println(F("  - Pull-up resistor (10K)"));
    Serial.println(F("  - Power (VCC, GND)"));
    Serial.println(F("Continuing with MLX90614 only..."));
  } else {
    Serial.println(F("SUCCESS!"));
  }

  Serial.println();
  Serial.println(F("Sensor Configuration:"));
  Serial.print(F("  MLX90614 Emissivity: "));
  Serial.println(mlx.readEmissivity());
  Serial.println(F("  DHT22 Type: Temperature + Humidity"));
  Serial.println();

  // Initial readings
  Serial.println(F("Initial readings (motor OFF):"));
  readSensors();
  displayInitialReadings();

  Serial.println();
  Serial.println(F("COMMANDS:"));
  Serial.println(F("  s - Start motor (PWM=150)"));
  Serial.println(F("  a - Auto test (5 min run)"));
  Serial.println(F("  + - Increase PWM"));
  Serial.println(F("  - - Decrease PWM"));
  Serial.println(F("  x - Stop motor"));
  Serial.println(F("  r - Reset statistics"));
  Serial.println(F("  ? - Show help"));
  Serial.println();

  lastRead = millis();
}

void loop() {
  // Check for commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }

  // Read sensors at interval
  unsigned long currentTime = millis();
  if (currentTime - lastRead >= READ_INTERVAL) {
    readSensors();
    updateStatistics();
    displayData();
    lastRead = currentTime;
  }

  // Auto test mode
  if (autoTest) {
    static unsigned long testStartTime = 0;
    if (testStartTime == 0) {
      testStartTime = millis();
      motorPWM = 200;  // High speed for thermal test
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      motorStartTime = millis();
      motorRunning = true;
      Serial.println(F("\n>>> AUTO TEST STARTED (5 minutes @ PWM=200)"));
      Serial.println(F(">>> Monitoring temperature rise..."));
    }

    // Stop after 5 minutes
    if (millis() - testStartTime >= 300000) {  // 5 min
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      motorRunning = false;
      autoTest = false;
      testStartTime = 0;
      Serial.println(F("\n>>> AUTO TEST COMPLETE"));
      printTestSummary();
    }
  }
}

void readSensors() {
  // Read MLX90614 (fast, no delay needed)
  motorTemp_C = mlx.readObjectTempC();
  ambientTemp_MLX_C = mlx.readAmbientTempC();

  // Read DHT22 (slow, can fail)
  float dhtTemp = dht.readTemperature();
  float dhtHumid = dht.readHumidity();

  if (!isnan(dhtTemp)) {
    ambientTemp_DHT_C = dhtTemp;
  }
  if (!isnan(dhtHumid)) {
    humidity_percent = dhtHumid;
  }

  // Calculate delta temperature
  // Use DHT22 ambient if available (more accurate), otherwise MLX ambient
  float ambientRef = (!isnan(ambientTemp_DHT_C)) ? ambientTemp_DHT_C : ambientTemp_MLX_C;
  deltaTemp_C = motorTemp_C - ambientRef;
}

void updateStatistics() {
  if (motorRunning) {
    if (motorTemp_C > maxMotorTemp) {
      maxMotorTemp = motorTemp_C;
    }
    if (deltaTemp_C > maxDeltaTemp) {
      maxDeltaTemp = deltaTemp_C;
    }
  }
}

void displayData() {
  static int headerCount = 0;
  if (headerCount % 15 == 0) {
    Serial.println();
    Serial.println(F("Time\tPWM\tMotor°C\tAmb°C\tΔT°C\tHumid%\tMax°C\tMaxΔT"));
    Serial.println(F("----\t---\t-------\t-----\t----\t------\t-----\t-----"));
  }
  headerCount++;

  // Runtime
  Serial.print(millis() / 1000);
  Serial.print(F("\t"));

  // PWM
  Serial.print(motorPWM);
  Serial.print(F("\t"));

  // Motor temperature
  Serial.print(motorTemp_C, 1);
  Serial.print(F("\t"));

  // Ambient (prefer DHT22)
  float ambDisplay = (!isnan(ambientTemp_DHT_C)) ? ambientTemp_DHT_C : ambientTemp_MLX_C;
  Serial.print(ambDisplay, 1);
  Serial.print(F("\t"));

  // Delta T
  Serial.print(deltaTemp_C, 1);
  Serial.print(F("\t"));

  // Humidity
  if (!isnan(humidity_percent)) {
    Serial.print(humidity_percent, 0);
  } else {
    Serial.print(F("--"));
  }
  Serial.print(F("\t"));

  // Statistics
  if (motorRunning) {
    Serial.print(maxMotorTemp, 1);
    Serial.print(F("\t"));
    Serial.print(maxDeltaTemp, 1);

    // Runtime indicator
    unsigned long runtime = (millis() - motorStartTime) / 1000;
    Serial.print(F("\t["));
    Serial.print(runtime);
    Serial.print(F("s]"));
  }

  Serial.println();
}

void displayInitialReadings() {
  Serial.print(F("  Motor surface: "));
  Serial.print(motorTemp_C, 2);
  Serial.println(F(" °C"));

  Serial.print(F("  Ambient (MLX): "));
  Serial.print(ambientTemp_MLX_C, 2);
  Serial.println(F(" °C"));

  if (!isnan(ambientTemp_DHT_C)) {
    Serial.print(F("  Ambient (DHT): "));
    Serial.print(ambientTemp_DHT_C, 2);
    Serial.println(F(" °C"));

    Serial.print(F("  Humidity:      "));
    Serial.print(humidity_percent, 1);
    Serial.println(F(" %"));
  }

  Serial.print(F("  Delta-T:       "));
  Serial.print(deltaTemp_C, 2);
  Serial.println(F(" °C"));
}

void printTestSummary() {
  Serial.println(F("\n========== TEST SUMMARY =========="));
  Serial.print(F("Max motor temp: "));
  Serial.print(maxMotorTemp, 1);
  Serial.println(F(" °C"));

  Serial.print(F("Max Delta-T:    "));
  Serial.print(maxDeltaTemp, 1);
  Serial.println(F(" °C"));

  Serial.print(F("Assessment:     "));
  if (maxDeltaTemp < 10) {
    Serial.println(F("EXCELLENT - Motor runs cool"));
  } else if (maxDeltaTemp < 15) {
    Serial.println(F("GOOD - Normal temperature rise"));
  } else if (maxDeltaTemp < 20) {
    Serial.println(F("WARNING - Higher than expected"));
  } else {
    Serial.println(F("CRITICAL - Excessive heating!"));
  }
  Serial.println(F("==================================\n"));
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 's':
    case 'S':
      motorPWM = 150;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      motorStartTime = millis();
      motorRunning = true;
      autoTest = false;
      Serial.println(F("\n>>> Motor STARTED (PWM=150)"));
      break;

    case 'a':
    case 'A':
      if (!autoTest) {
        autoTest = true;
        resetStatistics();
      }
      break;

    case '+':
      motorPWM = constrain(motorPWM + 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      if (!motorRunning && motorPWM > 0) {
        motorStartTime = millis();
        motorRunning = true;
      }
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      break;

    case '-':
      motorPWM = constrain(motorPWM - 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      if (motorPWM == 0) motorRunning = false;
      autoTest = false;
      Serial.print(F("\n>>> PWM -> "));
      Serial.println(motorPWM);
      break;

    case 'x':
    case 'X':
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      motorRunning = false;
      autoTest = false;
      Serial.println(F("\n>>> Motor STOPPED"));
      if (maxMotorTemp > 0) printTestSummary();
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
  maxMotorTemp = 0.0;
  maxDeltaTemp = 0.0;
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("s - Start motor (PWM=150)"));
  Serial.println(F("a - Auto test (5 min)"));
  Serial.println(F("+ - Increase PWM (+25)"));
  Serial.println(F("- - Decrease PWM (-25)"));
  Serial.println(F("x - Stop motor & show summary"));
  Serial.println(F("r - Reset statistics"));
  Serial.println(F("? - Show help"));
  Serial.println(F("==============================\n"));
}

/*
 * VERIFICATION CHECKLIST:
 *
 * [ ] MLX90614 detected on I2C (0x5A)
 * [ ] DHT22 responds with valid readings
 * [ ] Motor temp ≈ ambient when motor is off
 * [ ] Motor temp rises when motor runs
 * [ ] Delta-T increases over time, then stabilizes
 * [ ] Sensor position is marked and repeatable
 * [ ] MLX90614 pointing at motor body (not reflective parts)
 * [ ] DHT22 is away from motor heat source
 *
 * EXPECTED THERMAL PROFILE (to fill after testing):
 * Initial temp (motor off):     _____ °C
 * After 1 minute @ PWM=200:     _____ °C (ΔT = _____)
 * After 3 minutes:              _____ °C (ΔT = _____)
 * After 5 minutes (stabilized): _____ °C (ΔT = _____)
 * After cooldown (5 min):       _____ °C
 *
 * TROUBLESHOOTING:
 *
 * MLX90614 not found:
 * - Check I2C wiring and pull-ups
 * - Run I2C scanner to verify address
 * - Some modules are 3.3V only (check datasheet)
 *
 * DHT22 returns NaN:
 * - Check wiring to D4
 * - Verify pull-up resistor (10K)
 * - Wait 2 seconds between readings
 * - Try replacing sensor if consistently fails
 *
 * Motor temp doesn't rise:
 * - Check if MLX90614 is actually pointing at motor
 * - Verify motor is running (listen/observe)
 * - Motor might be very efficient (low friction)
 * - Increase runtime (some motors take >5 min)
 *
 * Motor temp = ambient always:
 * - MLX90614 may be pointing at reflective surface
 * - Reflective surfaces read ambient, not object temp
 * - Point at matte/dark area of motor
 * - Add matte tape to motor if needed
 *
 * Erratic temperature readings:
 * - Motor vibrations may move sensor
 * - Secure sensor in fixed mount
 * - Increase averaging (add software filter)
 *
 * Excessive heating (ΔT > 20°C):
 * - Check for mechanical binding
 * - Verify motor is not stalled
 * - Inspect bearings for wear
 * - Reduce test duration/speed
 *
 * NOTES:
 * - MLX90614 has ~90° field of view - position carefully
 * - Emissivity setting affects readings (default 0.95 for most surfaces)
 * - DHT22 has ±0.5°C accuracy but 2-5% humidity error
 * - Consider adding heat sink if motor runs very hot
 * - Log temperature profile to SD card for detailed analysis
 */
