/*
 * Motor Direct Test - Phase 0, Test 0.1
 * 
 * PROJECT: Arduino Motor Control Learning
 * PHASE: 0 - Component Verification
 * TEST: 0.1 - Motor Direct Test
 * 
 * OBJECTIVE:
 * Verify that the DC motor works correctly when connected directly to power.
 * This is the most basic test - just motor + power, no control circuitry.
 * 
 * HARDWARE SETUP:
 * - Motor (+) connected to 5V
 * - Motor (-) connected to GND
 * - Multimeter in series to measure current (optional, recommended)
 * 
 * EXPECTED RESULTS:
 * - Motor should spin smoothly
 * - Current draw: 60-90mA (based on previous measurements)
 * - No excessive heat after 30 seconds
 * - No unusual noise or vibration
 * 
 * ACTUAL RESULTS (to be filled after test):
 * - Voltage: _____ V
 * - Current: _____ mA
 * - Behavior: [ ] Smooth [ ] Erratic [ ] Doesn't start
 * - Temperature after 30s: [ ] Cold [ ] Warm [ ] Hot
 * - Noise: [ ] Silent [ ] Normal [ ] Loud
 * 
 * NOTES:
 * This sketch doesn't actually control anything - it's just documentation
 * of a hardware-only test. The Arduino is not needed for this test.
 * However, this file documents what we're testing and why.
 * 
 * NEXT STEPS:
 * 1. Measure minimum voltage required for motor to start
 * 2. Characterize voltage vs speed relationship
 * 3. Document findings in docs/test-results.md
 * 
 * VERSION: 1.0
 * DATE: 2024-12-28
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

void setup() {
  // Initialize serial for documentation purposes
  Serial.begin(9600);
  
  Serial.println("=================================");
  Serial.println("Motor Direct Test - Phase 0.1");
  Serial.println("=================================");
  Serial.println();
  Serial.println("TEST PROCEDURE:");
  Serial.println("1. Disconnect Arduino from circuit");
  Serial.println("2. Connect motor (+) to 5V power source");
  Serial.println("3. Connect motor (-) to GND");
  Serial.println("4. Optionally: Insert multimeter in series to measure current");
  Serial.println("5. Power on and observe motor behavior");
  Serial.println();
  Serial.println("MEASUREMENTS TO TAKE:");
  Serial.println("- Voltage across motor terminals");
  Serial.println("- Current consumption");
  Serial.println("- Motor temperature after 30 seconds");
  Serial.println("- Any unusual behavior (noise, vibration, etc.)");
  Serial.println();
  Serial.println("MINIMUM VOLTAGE TEST:");
  Serial.println("1. Start with 5V");
  Serial.println("2. Gradually reduce voltage (if variable PSU available)");
  Serial.println("3. Note voltage where motor barely spins");
  Serial.println("4. Note voltage where motor completely stops");
  Serial.println();
  Serial.println("After testing, record results in:");
  Serial.println("docs/test-results.md - Test 0.1 section");
  Serial.println();
  Serial.println("=================================");
}

void loop() {
  // Nothing to do - this is a documentation sketch only
  // The actual test is done with hardware connections only
  delay(1000);
}

/*
 * TROUBLESHOOTING:
 * 
 * Motor doesn't spin:
 * - Check connections (loose wires?)
 * - Verify power supply is on and providing 5V
 * - Try reversing motor polarity (if not marked)
 * - Test motor with another power source
 * 
 * Motor spins erratically:
 * - Check for loose connections
 * - Verify power supply can deliver enough current
 * - Check motor brushes (if visible)
 * 
 * Motor gets very hot:
 * - Reduce test time
 * - Verify voltage is not exceeding motor rating
 * - Check if motor is mechanically blocked
 * 
 * Current higher than expected (>100mA):
 * - This may exceed your 100mA power supply limit
 * - Consider using external power source for motor
 * - Document actual consumption for phase planning
 */
