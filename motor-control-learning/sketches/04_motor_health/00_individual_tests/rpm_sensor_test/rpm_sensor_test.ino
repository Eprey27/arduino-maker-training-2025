/*
 * RPM Sensor Test - Motor Health Monitoring
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * TEST: RPM Measurement with TCRT5000 IR Sensor
 *
 * OBJECTIVE:
 * Test and calibrate an infrared reflective sensor (TCRT5000) to measure
 * motor RPM using interrupts for accurate counting. This establishes the
 * foundation for motor speed monitoring.
 *
 * HARDWARE SETUP:
 * TCRT5000 Module:
 * - VCC -> Arduino 5V
 * - GND -> Arduino GND
 * - D0 (Digital Out) -> Arduino D2 (INT0)
 * - A0 (Analog Out) -> Arduino A0 (optional, for calibration)
 *
 * Motor Setup:
 * - Add reflective marker on motor shaft (white tape or sticker)
 * - Position TCRT5000 2-5mm from shaft
 * - Ensure stable mounting (vibrations affect readings)
 *
 * HOW IT WORKS:
 * 1. TCRT5000 emits IR light and detects reflection
 * 2. Reflective mark on shaft reflects more light -> HIGH pulse
 * 3. Dark shaft reflects less -> LOW
 * 4. Each HIGH pulse = 1 revolution
 * 5. Count pulses over time window to calculate RPM
 *
 * INTERRUPT METHOD:
 * - Using hardware interrupt on pin D2
 * - RISING edge triggers counter increment
 * - Calculate RPM every 1 second
 * - RPM = (pulses_counted * 60) / time_window_seconds
 *
 * EXPECTED RESULTS:
 * - Stable RPM readings when motor speed is constant
 * - RPM increases proportionally with PWM
 * - Minimal jitter (<5% variation)
 *
 * CALIBRATION STEPS:
 * 1. Read analog output (A0) to find threshold
 * 2. Adjust sensitivity potentiometer on TCRT5000
 * 3. Verify single pulse per revolution (check Serial output)
 * 4. Compare calculated RPM with known reference if available
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

// Pin definitions
const int RPM_SENSOR_DIGITAL = 2;    // D2 (INT0) - Digital output from TCRT5000
const int RPM_SENSOR_ANALOG = A0;    // A0 - Analog output (for calibration)
const int MOTOR_PWM_PIN = 9;         // PWM pin to control motor speed

// RPM calculation variables
volatile unsigned long pulseCount = 0;    // Counts interrupts (volatile for ISR)
unsigned long lastCalcTime = 0;           // Last time we calculated RPM
const unsigned int CALC_INTERVAL = 1000;  // Calculate RPM every 1 second
float currentRPM = 0.0;                   // Current motor RPM

// Motor control
int motorPWM = 0;                         // Current PWM value (0-255)
bool autoTest = false;                    // Auto-increment PWM for testing

// Calibration
bool showAnalogValue = true;              // Show analog sensor value for calibration

void setup() {
  Serial.begin(115200);  // Higher baud rate for real-time monitoring

  // Configure pins
  pinMode(RPM_SENSOR_DIGITAL, INPUT);
  pinMode(RPM_SENSOR_ANALOG, INPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);

  // Attach interrupt to count pulses
  // RISING: trigger when signal goes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_DIGITAL), countPulse, RISING);

  // Initialize motor off
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("RPM Sensor Test - TCRT5000"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("SENSOR CALIBRATION MODE"));
  Serial.println(F("1. Check analog value (A0) below"));
  Serial.println(F("2. Adjust TCRT5000 pot until:"));
  Serial.println(F("   - Reflective mark: >512 (HIGH)"));
  Serial.println(F("   - Dark shaft: <200 (LOW)"));
  Serial.println(F("3. Send 's' to start motor test"));
  Serial.println(F("4. Send 'a' to start auto PWM sweep"));
  Serial.println(F("5. Send '+'/'-' to adjust PWM manually"));
  Serial.println();

  lastCalcTime = millis();
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }

  // Calculate RPM every CALC_INTERVAL milliseconds
  unsigned long currentTime = millis();
  if (currentTime - lastCalcTime >= CALC_INTERVAL) {
    calculateRPM();
    displayData();
    lastCalcTime = currentTime;
  }

  // Auto-test mode: slowly increase PWM
  if (autoTest) {
    static unsigned long lastPWMChange = 0;
    if (currentTime - lastPWMChange >= 3000) {  // Change every 3 seconds
      motorPWM += 25;
      if (motorPWM > 255) motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      lastPWMChange = currentTime;
    }
  }
}

// Interrupt Service Routine - called on each pulse
void countPulse() {
  pulseCount++;
}

// Calculate RPM based on pulse count
void calculateRPM() {
  // Disable interrupts briefly to safely read volatile variable
  noInterrupts();
  unsigned long pulses = pulseCount;
  pulseCount = 0;  // Reset counter for next interval
  interrupts();

  // RPM = (pulses per interval) * (60 seconds / interval_seconds)
  // For 1 second interval: RPM = pulses * 60
  float intervalSeconds = CALC_INTERVAL / 1000.0;
  currentRPM = (pulses * 60.0) / intervalSeconds;
}

// Display current readings
void displayData() {
  // Header (print occasionally)
  static int lineCount = 0;
  if (lineCount % 20 == 0) {
    Serial.println();
    Serial.println(F("Time(s)\tPWM\tRPM\tAnalog\tStatus"));
    Serial.println(F("-------\t---\t---\t------\t------"));
  }
  lineCount++;

  // Current time
  Serial.print(millis() / 1000.0, 1);
  Serial.print(F("\t"));

  // PWM value
  Serial.print(motorPWM);
  Serial.print(F("\t"));

  // RPM
  Serial.print(currentRPM, 0);
  Serial.print(F("\t"));

  // Analog sensor value (for calibration)
  if (showAnalogValue) {
    int analogValue = analogRead(RPM_SENSOR_ANALOG);
    Serial.print(analogValue);
    Serial.print(F("\t"));

    // Status indicator
    if (analogValue > 512) {
      Serial.print(F("HIGH"));
    } else if (analogValue < 200) {
      Serial.print(F("LOW"));
    } else {
      Serial.print(F("MID"));
    }
  }

  Serial.println();
}

// Handle serial commands
void handleCommand(char cmd) {
  switch (cmd) {
    case 's':
    case 'S':
      // Start motor at low speed
      motorPWM = 100;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.println(F("\n>>> Motor started at PWM=100"));
      break;

    case 'a':
    case 'A':
      // Start auto PWM sweep
      autoTest = !autoTest;
      if (autoTest) {
        Serial.println(F("\n>>> Auto PWM sweep STARTED"));
        motorPWM = 50;
      } else {
        Serial.println(F("\n>>> Auto PWM sweep STOPPED"));
      }
      break;

    case '+':
      // Increase PWM
      motorPWM = constrain(motorPWM + 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM increased to "));
      Serial.println(motorPWM);
      break;

    case '-':
      // Decrease PWM
      motorPWM = constrain(motorPWM - 25, 0, 255);
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.print(F("\n>>> PWM decreased to "));
      Serial.println(motorPWM);
      break;

    case 'x':
    case 'X':
      // Stop motor
      motorPWM = 0;
      analogWrite(MOTOR_PWM_PIN, 0);
      autoTest = false;
      Serial.println(F("\n>>> Motor STOPPED"));
      break;

    case 'h':
    case 'H':
      // Toggle analog display
      showAnalogValue = !showAnalogValue;
      Serial.print(F("\n>>> Analog display: "));
      Serial.println(showAnalogValue ? F("ON") : F("OFF"));
      break;

    case '?':
      // Help
      printHelp();
      break;
  }
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("s - Start motor (PWM=100)"));
  Serial.println(F("a - Auto PWM sweep test"));
  Serial.println(F("+ - Increase PWM by 25"));
  Serial.println(F("- - Decrease PWM by 25"));
  Serial.println(F("x - Stop motor"));
  Serial.println(F("h - Toggle analog display"));
  Serial.println(F("? - Show this help"));
  Serial.println(F("==============================\n"));
}

/*
 * VERIFICATION CHECKLIST:
 *
 * [ ] TCRT5000 positioned 2-5mm from shaft
 * [ ] Reflective marker clearly visible on shaft
 * [ ] Analog value shows clear HIGH/LOW distinction
 * [ ] Digital output pulses once per revolution (not multiple)
 * [ ] RPM readings are stable (variation <5%)
 * [ ] RPM increases when PWM increases
 * [ ] RPM decreases when PWM decreases
 * [ ] No missed pulses (check by marking shaft and counting manually)
 *
 * EXPECTED RPM RANGES (to be filled after testing):
 * PWM=50:   _____ RPM
 * PWM=100:  _____ RPM
 * PWM=150:  _____ RPM
 * PWM=200:  _____ RPM
 * PWM=255:  _____ RPM
 *
 * TROUBLESHOOTING:
 *
 * No pulses detected:
 * - Check wiring (VCC, GND, D0->D2)
 * - Verify reflective marker is present
 * - Adjust TCRT5000 sensitivity pot
 * - Check analog value: should toggle between HIGH/LOW
 *
 * Multiple pulses per revolution:
 * - Marker may be too large or uneven
 * - Sensor too sensitive (adjust pot counterclockwise)
 * - Check for reflections from surroundings
 *
 * Unstable readings:
 * - Sensor mount may be vibrating
 * - Increase CALC_INTERVAL to 2000ms
 * - Add debouncing (software or hardware)
 * - Shield sensor from ambient IR light
 *
 * RPM seems too high or too low:
 * - Verify calculation: RPM = pulses * 60 (for 1s window)
 * - Check if motor has gearbox (multiply/divide accordingly)
 * - Manually count rotations with stopwatch to verify
 *
 * NOTES:
 * - For accurate low RPM (<100), increase CALC_INTERVAL to 5000ms
 * - For high RPM (>5000), reduce CALC_INTERVAL to 500ms
 * - Maximum measurable RPM depends on Arduino interrupt capability (~8000 Hz)
 * - Consider adding averaging filter for smoother readings
 */
