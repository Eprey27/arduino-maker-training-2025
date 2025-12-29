/*
 * Sound Sensor Test - KY-038 / LM393 (Motor Health Monitoring)
 *
 * PROJECT: Arduino Motor Control Learning
 * MODULE: Motor Health Monitoring System
 * TEST: Acoustic Noise Level Measurement
 *
 * OBJECTIVE:
 * Test sound sensor module to measure motor acoustic noise levels.
 * Sound monitoring can detect:
 * - Worn brushes (clicking, crackling)
 * - Bearing damage (grinding, rumbling)
 * - Gear wear (if motor has gearbox)
 * - Overall motor condition changes over time
 * - Establish acoustic baseline for health monitoring
 *
 * HARDWARE SETUP:
 * KY-038 Sound Sensor Module:
 * - VCC -> Arduino 5V
 * - GND -> Arduino GND
 * - A0 (Analog Out) -> Arduino A0
 * - D0 (Digital Out) -> Arduino D2 (optional, for threshold detection)
 *
 * Motor Control:
 * - PWM Pin D9 -> Motor control
 *
 * SENSOR POSITIONING (CRITICAL!):
 * - Fixed distance from motor: 10cm recommended
 * - Same height as motor axis
 * - Mark position for repeatable measurements
 * - Away from other noise sources (fans, AC, etc.)
 * - Consistent environment (same room, time of day)
 *
 * HOW IT WORKS:
 * 1. Microphone converts sound pressure to voltage
 * 2. Amplifier boosts signal (adjustable with pot)
 * 3. Arduino ADC reads analog voltage (0-1023)
 * 4. Higher values = louder sound
 * 5. Baseline subtraction isolates motor noise
 *
 * LIMITATIONS:
 * - Not calibrated to dB SPL (Sound Pressure Level)
 * - Relative measurements only (compare to baseline)
 * - Sensitive to ambient noise
 * - Frequency response typically 50Hz-10kHz
 * - Best for detecting CHANGES in noise, not absolute levels
 *
 * CALIBRATION:
 * - Adjust sensitivity pot while monitoring analog value
 * - Too sensitive: always at max (1023)
 * - Too low: no response to motor
 * - Target: motor noise gives 200-800 range
 *
 * EXPECTED BEHAVIOR:
 * - Quiet room: 50-200 ADC value
 * - Motor running low speed: +50-100 ADC
 * - Motor running high speed: +100-300 ADC
 * - Unusual sounds (clicks, grinding): sporadic high peaks
 *
 * VERSION: 1.0
 * DATE: 2024-12-29
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

// Pin definitions
const int SOUND_ANALOG_PIN = A0;
const int SOUND_DIGITAL_PIN = 2;  // Optional threshold detection
const int MOTOR_PWM_PIN = 9;

// Motor control
int motorPWM = 0;

// Sound measurement
int soundLevel = 0;            // Current ADC value
int soundMin = 1023;           // Minimum in sample window
int soundMax = 0;              // Maximum in sample window
int soundPeak = 0;             // Peak-to-peak amplitude
int soundAvg = 0;              // Running average

// Baseline (ambient noise, motor off)
int baselineNoise = 0;
bool baselineSet = false;

// Statistics
long soundSum = 0;
unsigned long sampleCount = 0;
int maxPeakSeen = 0;
int avgPeakSeen = 0;

// Sampling
const int SAMPLE_WINDOW = 50;  // Sample for 50ms to get peak-to-peak
unsigned long lastWindow = 0;

// Display timing
unsigned long lastPrint = 0;
const unsigned int PRINT_INTERVAL = 500;  // Print every 500ms

// Auto test
bool autoTest = false;

void setup() {
  Serial.begin(115200);

  pinMode(SOUND_ANALOG_PIN, INPUT);
  pinMode(SOUND_DIGITAL_PIN, INPUT);
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  analogWrite(MOTOR_PWM_PIN, 0);

  Serial.println(F("========================================"));
  Serial.println(F("Sound Sensor Test - KY-038"));
  Serial.println(F("========================================"));
  Serial.println();

  Serial.println(F("SENSOR CALIBRATION:"));
  Serial.println(F("1. Observe raw ADC values below"));
  Serial.println(F("2. Adjust sensitivity pot on KY-038:"));
  Serial.println(F("   - Quiet: 50-200 ADC"));
  Serial.println(F("   - Motor running: 200-800 ADC"));
  Serial.println(F("   - Avoid saturation (1023 = too high)"));
  Serial.println(F("3. Send 'c' to capture baseline"));
  Serial.println();

  delay(1000);

  // Show raw values for 5 seconds
  Serial.println(F("Raw ADC values (for calibration):"));
  unsigned long calStart = millis();
  while (millis() - calStart < 5000) {
    int raw = analogRead(SOUND_ANALOG_PIN);
    Serial.print(raw);
    Serial.print(F(" "));
    if ((millis() - calStart) % 500 < 50) Serial.println();
    delay(50);
  }
  Serial.println();

  Serial.println(F("\nCOMMANDS:"));
  Serial.println(F("  c - Capture baseline (motor OFF)"));
  Serial.println(F("  s - Start motor (PWM=100)"));
  Serial.println(F("  a - Auto PWM sweep"));
  Serial.println(F("  + - Increase PWM"));
  Serial.println(F("  - - Decrease PWM"));
  Serial.println(F("  x - Stop motor"));
  Serial.println(F("  r - Reset statistics"));
  Serial.println(F("  ? - Show help"));
  Serial.println();

  lastWindow = millis();
  lastPrint = millis();
}

void loop() {
  // Check for commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }

  // Sample sound level
  unsigned long currentTime = millis();
  sampleSound();

  // Process sample window
  if (currentTime - lastWindow >= SAMPLE_WINDOW) {
    calculatePeakToPeak();
    lastWindow = currentTime;
  }

  // Print at interval
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
    }
  }
}

// Sample sound continuously
void sampleSound() {
  soundLevel = analogRead(SOUND_ANALOG_PIN);

  // Track min/max for peak-to-peak calculation
  if (soundLevel < soundMin) soundMin = soundLevel;
  if (soundLevel > soundMax) soundMax = soundLevel;

  // Running sum for average
  soundSum += soundLevel;
  sampleCount++;
}

// Calculate peak-to-peak amplitude
void calculatePeakToPeak() {
  soundPeak = soundMax - soundMin;

  // Calculate average
  if (sampleCount > 0) {
    soundAvg = soundSum / sampleCount;
  }

  // Update statistics
  if (soundPeak > maxPeakSeen) {
    maxPeakSeen = soundPeak;
  }

  // Running average of peaks
  static int peakCount = 0;
  static long peakSum = 0;
  peakSum += soundPeak;
  peakCount++;
  avgPeakSeen = peakSum / peakCount;

  // Reset for next window
  soundMin = 1023;
  soundMax = 0;
  soundSum = 0;
  sampleCount = 0;
}

// Capture baseline ambient noise
void captureBaseline() {
  Serial.println(F("\nCapturing baseline (3 seconds)..."));
  Serial.println(F("Keep motor OFF and environment quiet."));

  long sum = 0;
  int samples = 0;
  unsigned long startTime = millis();

  while (millis() - startTime < 3000) {
    int reading = analogRead(SOUND_ANALOG_PIN);
    sum += reading;
    samples++;
    delay(10);
  }

  baselineNoise = sum / samples;
  baselineSet = true;

  Serial.print(F("Baseline captured: "));
  Serial.print(baselineNoise);
  Serial.println(F(" ADC"));
  Serial.println();
}

void displayData() {
  static int headerCount = 0;
  if (headerCount % 20 == 0) {
    Serial.println();
    if (baselineSet) {
      Serial.println(F("Time\tPWM\tAvg\tPeak\tΔBase\tMaxPk\tAvgPk\tStatus"));
      Serial.println(F("----\t---\t---\t----\t-----\t-----\t-----\t------"));
    } else {
      Serial.println(F("Time\tPWM\tAvg\tPeak\tMaxPk\tAvgPk\tStatus"));
      Serial.println(F("----\t---\t---\t----\t-----\t-----\t------"));
    }
  }
  headerCount++;

  Serial.print(millis() / 1000);
  Serial.print(F("\t"));
  Serial.print(motorPWM);
  Serial.print(F("\t"));
  Serial.print(soundAvg);
  Serial.print(F("\t"));
  Serial.print(soundPeak);
  Serial.print(F("\t"));

  if (baselineSet) {
    int delta = soundAvg - baselineNoise;
    Serial.print(delta);
    Serial.print(F("\t"));
  }

  Serial.print(maxPeakSeen);
  Serial.print(F("\t"));
  Serial.print(avgPeakSeen);
  Serial.print(F("\t"));

  // Status based on deviation from baseline
  if (baselineSet) {
    int delta = soundAvg - baselineNoise;
    if (delta < 20) {
      Serial.print(F("Quiet"));
    } else if (delta < 50) {
      Serial.print(F("Low"));
    } else if (delta < 100) {
      Serial.print(F("Normal"));
    } else if (delta < 200) {
      Serial.print(F("Loud"));
    } else {
      Serial.print(F("VERY LOUD"));
    }
  } else {
    Serial.print(F("No baseline"));
  }

  // Warn if saturating
  if (soundAvg > 900) {
    Serial.print(F(" [SATURATED!]"));
  }

  Serial.println();
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'c':
    case 'C':
      if (motorPWM == 0) {
        captureBaseline();
      } else {
        Serial.println(F("\n>>> ERROR: Stop motor first (x)"));
      }
      break;

    case 's':
    case 'S':
      motorPWM = 100;
      analogWrite(MOTOR_PWM_PIN, motorPWM);
      autoTest = false;
      Serial.println(F("\n>>> Motor STARTED (PWM=100)"));
      delay(500);  // Settling
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
      printSummary();
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
  maxPeakSeen = 0;
  avgPeakSeen = 0;
}

void printSummary() {
  if (!baselineSet) {
    Serial.println(F("\n>>> Capture baseline first (c)"));
    return;
  }

  Serial.println(F("\n========== SOUND SUMMARY =========="));
  Serial.print(F("Baseline:      ")); Serial.print(baselineNoise); Serial.println(F(" ADC"));
  Serial.print(F("Max peak:      ")); Serial.print(maxPeakSeen); Serial.println(F(" ADC"));
  Serial.print(F("Avg peak:      ")); Serial.print(avgPeakSeen); Serial.println(F(" ADC"));

  int increase = avgPeakSeen - baselineNoise;
  Serial.print(F("Increase:      ")); Serial.print(increase); Serial.println(F(" ADC"));

  Serial.print(F("Assessment:    "));
  if (increase < 30) {
    Serial.println(F("Very quiet motor"));
  } else if (increase < 80) {
    Serial.println(F("Normal noise level"));
  } else if (increase < 150) {
    Serial.println(F("Moderately loud"));
  } else {
    Serial.println(F("Loud - check for issues"));
  }
  Serial.println(F("===================================\n"));
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("c - Capture baseline"));
  Serial.println(F("s - Start motor (PWM=100)"));
  Serial.println(F("a - Auto PWM sweep"));
  Serial.println(F("+ - Increase PWM (+25)"));
  Serial.println(F("- - Decrease PWM (-25)"));
  Serial.println(F("x - Stop motor & summary"));
  Serial.println(F("r - Reset statistics"));
  Serial.println(F("? - Show help"));
  Serial.println(F("==============================\n"));
}

/*
 * VERIFICATION CHECKLIST:
 *
 * [ ] Sensor responds to sound (clap, talk)
 * [ ] Sensitivity pot adjusted (not saturating)
 * [ ] Baseline captured in quiet environment
 * [ ] Noise level increases with motor PWM
 * [ ] Sensor position is fixed and marked
 * [ ] Distance from motor is consistent (10cm)
 * [ ] No other noise sources interfering
 *
 * EXPECTED VALUES (to fill after testing):
 * Baseline (ambient):      _____ ADC
 * PWM=50:                  _____ ADC (Δ = _____)
 * PWM=100:                 _____ ADC (Δ = _____)
 * PWM=150:                 _____ ADC (Δ = _____)
 * PWM=200:                 _____ ADC (Δ = _____)
 * PWM=255:                 _____ ADC (Δ = _____)
 *
 * Unusual sounds detected: [ ] None [ ] Clicking [ ] Grinding [ ] Buzzing
 *
 * TROUBLESHOOTING:
 *
 * Sensor not responding:
 * - Check wiring (VCC, GND, A0)
 * - Verify with loud sound (clap)
 * - Adjust sensitivity pot
 *
 * Always at 0 or 1023:
 * - Sensitivity too low (turn pot clockwise)
 * - Or too high (turn counterclockwise)
 * - Recalibrate after adjustment
 *
 * No difference between motor on/off:
 * - Ambient noise too high
 * - Move to quieter environment
 * - Position sensor closer (5cm)
 * - Increase sensitivity
 *
 * Erratic readings:
 * - Normal for sound! (AC signal)
 * - Use peak-to-peak and average
 * - Increase sample window
 *
 * Can't establish consistent baseline:
 * - Ensure environment is stable
 * - Turn off fans, AC, other motors
 * - Test at consistent time of day
 * - Consider soundproofing enclosure
 *
 * NOTES:
 * - This is RELATIVE measurement, not calibrated dB
 * - For dB SPL, need calibrated mic like SPL meter
 * - Peak-to-peak better indicator than instantaneous
 * - Frequency analysis (FFT) can identify specific issues:
 *   - Bearing damage: Low frequency rumble
 *   - Brush wear: High frequency crackling
 *   - Imbalance: Frequency matches rotation speed
 * - Log noise profile over motor lifetime for predictive maintenance
 * - Correlation with vibration data increases diagnostic accuracy
 */
