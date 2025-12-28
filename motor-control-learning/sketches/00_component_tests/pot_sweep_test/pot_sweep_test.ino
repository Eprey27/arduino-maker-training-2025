/*
 * Potentiometer Sweep Test - Phase 0, Test 0.2
 * 
 * PROJECT: Arduino Motor Control Learning
 * PHASE: 0 - Component Verification  
 * TEST: 0.2 - Potentiometer Sweep Test
 * 
 * OBJECTIVE:
 * Verify that the 10K potentiometer works correctly and has smooth
 * variation from 0 to 10K ohms. This test reads the pot value through
 * Arduino's analog input to visualize the sweep.
 * 
 * HARDWARE SETUP:
 * - Potentiometer Pin 1 (one end) -> 5V
 * - Potentiometer Pin 2 (center/wiper) -> Arduino A0
 * - Potentiometer Pin 3 (other end) -> GND
 * 
 * WHAT THIS TESTS:
 * - Smooth variation without dead zones
 * - Full range (0-1023 in ADC, representing 0-10K)
 * - Direction of rotation (which way increases resistance)
 * - Any scratchy/noisy behavior
 * 
 * EXPECTED RESULTS:
 * - Pot at one extreme: ADC reads ~0
 * - Pot at other extreme: ADC reads ~1023  
 * - Smooth transition when rotating
 * - No sudden jumps or dead zones
 * 
 * ACTUAL RESULTS (to be filled after test):
 * - Minimum ADC value: _____
 * - Maximum ADC value: _____
 * - Behavior: [ ] Smooth [ ] Jumpy [ ] Dead zones
 * - Rotation direction: [ ] CW increases [ ] CCW increases
 * 
 * NOTES:
 * Arduino ADC is 10-bit: 0-1023 values
 * 0 = 0V, 1023 = 5V
 * Each count = ~4.88mV
 * 
 * The pot acts as a voltage divider:
 * Vout = Vin * (R_wiper_to_gnd / R_total)
 * 
 * VERSION: 1.0
 * DATE: 2024-12-28
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

// Pin definitions
const int POT_PIN = A0;           // Potentiometer wiper connected here

// Sampling parameters
const int SAMPLE_DELAY = 100;     // Read every 100ms
const int SAMPLES_FOR_AVG = 5;    // Average 5 readings to reduce noise

// Variables
int potValue = 0;                 // Raw ADC value (0-1023)
float voltage = 0.0;              // Calculated voltage (0-5V)
int minValueSeen = 1023;          // Track minimum value
int maxValueSeen = 0;             // Track maximum value

void setup() {
  Serial.begin(9600);
  
  // Configure analog input
  pinMode(POT_PIN, INPUT);
  
  Serial.println("=================================");
  Serial.println("Potentiometer Sweep Test - Phase 0.2");
  Serial.println("=================================");
  Serial.println();
  Serial.println("INSTRUCTIONS:");
  Serial.println("1. Slowly rotate potentiometer from one extreme to the other");
  Serial.println("2. Watch the ADC values and voltage readings");
  Serial.println("3. Note which direction increases the value");
  Serial.println("4. Watch for any jumps, dead zones, or erratic behavior");
  Serial.println();
  Serial.println("ADC Range: 0-1023 (0 = 0V, 1023 = 5V)");
  Serial.println();
  Serial.println("Starting readings...");
  Serial.println("ADC\tVoltage\tMin\tMax");
  Serial.println("---\t-------\t---\t---");
  
  delay(1000);
}

void loop() {
  // Take multiple samples and average them to reduce noise
  long sum = 0;
  for (int i = 0; i < SAMPLES_FOR_AVG; i++) {
    sum += analogRead(POT_PIN);
    delay(10);
  }
  potValue = sum / SAMPLES_FOR_AVG;
  
  // Calculate voltage from ADC value
  voltage = (potValue / 1023.0) * 5.0;
  
  // Track min and max values seen
  if (potValue < minValueSeen) minValueSeen = potValue;
  if (potValue > maxValueSeen) maxValueSeen = potValue;
  
  // Print current reading with min/max tracking
  Serial.print(potValue);
  Serial.print("\t");
  Serial.print(voltage, 2);
  Serial.print("V\t");
  Serial.print(minValueSeen);
  Serial.print("\t");
  Serial.println(maxValueSeen);
  
  delay(SAMPLE_DELAY);
}

/*
 * VERIFICATION CHECKLIST:
 * 
 * After running this test, verify:
 * [ ] Minimum value reaches close to 0 (within 0-10)
 * [ ] Maximum value reaches close to 1023 (within 1013-1023)
 * [ ] Values change smoothly when rotating (no jumps > 50 ADC units)
 * [ ] No dead zones (areas where rotation doesn't change value)
 * [ ] Direction of rotation is consistent and predictable
 * 
 * ADDITIONAL TESTS WITH MULTIMETER:
 * After Arduino test, disconnect and measure with multimeter in resistance mode:
 * 
 * 1. Measure Pin 1 to Pin 3 (total resistance):
 *    Should be ~10K ohms, doesn't change when rotating
 * 
 * 2. Measure Pin 2 to Pin 1 while rotating:
 *    Should vary smoothly from ~0 to ~10K
 * 
 * 3. Measure Pin 2 to Pin 3 while rotating:
 *    Should vary in opposite direction (10K to 0)
 * 
 * TROUBLESHOOTING:
 * 
 * Readings don't reach 0 or 1023:
 * - Check wiring (Pin 1 to 5V, Pin 3 to GND)
 * - Verify pot is rotated to full extremes
 * - May need mechanical adjustment if pot has built-in stops
 * 
 * Erratic readings / sudden jumps:
 * - Pot may be worn or dirty
 * - Try rotating back and forth several times
 * - May need contact cleaner or replacement
 * 
 * Values stuck or don't change:
 * - Check wiper connection (Pin 2)
 * - Verify pot isn't damaged
 * - Try different pot if available
 * 
 * Noisy readings (constantly jumping by small amounts):
 * - This is normal for cheap pots
 * - Averaging helps (already implemented)
 * - Can add small capacitor (0.1uF) across pot if needed
 */
