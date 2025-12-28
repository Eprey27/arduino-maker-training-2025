/*
 * MOSFET Switching Test - Phase 0, Test 0.3
 * 
 * PROJECT: Arduino Motor Control Learning
 * PHASE: 0 - Component Verification
 * TEST: 0.3 - MOSFET Switching Test
 * 
 * OBJECTIVE:
 * Verify that the IRF520 MOSFET can properly switch the motor on and off
 * when controlled by Arduino. Tests both digital switching (full on/off)
 * and determines the gate voltage characteristics.
 * 
 * HARDWARE SETUP:
 * - Motor (+) -> 5V (from external power supply, NOT Arduino)
 * - Motor (-) -> MOSFET Drain (pin 2, center pin)
 * - MOSFET Source (pin 3, right pin) -> GND (common with Arduino)
 * - MOSFET Gate (pin 1, left pin) -> Arduino Pin 9 (PWM capable)
 * - 10K resistor from Gate to GND (pull-down)
 * 
 * WARNING: Motor should be powered from external 5V supply, not Arduino 5V pin!
 * Arduino 5V pin cannot provide enough current for motor (60-90mA).
 * Common GND between Arduino and motor supply is REQUIRED.
 * 
 * WHAT THIS TESTS:
 * - Motor OFF when gate = LOW (0V)
 * - Motor ON when gate = HIGH (5V)
 * - Smooth switching behavior
 * - MOSFET temperature rise
 * - Threshold voltage (Vgs) where motor starts
 * 
 * EXPECTED RESULTS:
 * - Motor completely stopped when gate LOW
 * - Motor running when gate HIGH
 * - Clean switching with no erratic behavior
 * - MOSFET should not get very hot with this low current
 * 
 * ACTUAL RESULTS (to be filled after test):
 * - Motor behavior when gate LOW: [ ] OFF [ ] ON [ ] Partial
 * - Motor behavior when gate HIGH: [ ] OFF [ ] ON [ ] Partial
 * - Switching response: [ ] Immediate [ ] Delayed [ ] Erratic
 * - MOSFET temperature: [ ] Cool [ ] Warm [ ] Hot
 * - Threshold voltage (Vgs): _____ V
 * 
 * VERSION: 1.0
 * DATE: 2024-12-28
 * AUTHOR: Emilio
 * STATUS: Ready for test
 */

// Pin definitions
const int MOSFET_GATE_PIN = 9;    // PWM pin controlling MOSFET gate
const int LED_PIN = 13;            // Built-in LED mirrors motor state

// Test parameters
const int ON_TIME = 3000;          // Motor ON for 3 seconds
const int OFF_TIME = 2000;         // Motor OFF for 2 seconds

// Test mode selection
// Set to true to run different test modes
bool runOnOffTest = true;          // Basic on/off switching
bool runThresholdTest = false;     // Find Vgs threshold (run separately)

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(MOSFET_GATE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Start with motor OFF
  digitalWrite(MOSFET_GATE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("=================================");
  Serial.println("MOSFET Switching Test - Phase 0.3");
  Serial.println("=================================");
  Serial.println();
  Serial.println("HARDWARE CHECK:");
  Serial.println("[ ] Motor (+) connected to external 5V supply");
  Serial.println("[ ] Motor (-) connected to MOSFET Drain (center pin)");
  Serial.println("[ ] MOSFET Source (right pin) connected to GND");
  Serial.println("[ ] MOSFET Gate (left pin) connected to Arduino Pin 9");
  Serial.println("[ ] 10K resistor from Gate to GND");
  Serial.println("[ ] Common GND between Arduino and motor supply");
  Serial.println();
  
  if (runOnOffTest) {
    Serial.println("TEST MODE: ON/OFF Switching");
    Serial.println("Motor will cycle ON (3s) / OFF (2s)");
    Serial.println("Observe: Clean switching, no erratic behavior");
    Serial.println("Monitor: MOSFET temperature (should stay cool)");
  }
  
  if (runThresholdTest) {
    Serial.println("TEST MODE: Vgs Threshold Detection");
    Serial.println("Gate voltage will ramp from 0-5V slowly");
    Serial.println("Note the voltage where motor starts to spin");
  }
  
  Serial.println();
  Serial.println("Built-in LED (pin 13) mirrors motor state");
  Serial.println();
  
  delay(3000);  // Give time to read instructions
  
  Serial.println("Starting test...");
  Serial.println();
}

void loop() {
  if (runOnOffTest) {
    onOffSwitchingTest();
  }
  
  if (runThresholdTest) {
    thresholdTest();
    // Run threshold test only once
    while(true) delay(1000);
  }
}

// Basic ON/OFF switching test
void onOffSwitchingTest() {
  // Turn motor ON
  Serial.println("Motor ON (Gate = HIGH, 5V)");
  digitalWrite(MOSFET_GATE_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  
  // Monitor for ON_TIME
  unsigned long startTime = millis();
  while (millis() - startTime < ON_TIME) {
    // Could add current/voltage monitoring here if sensors available
    delay(100);
  }
  
  // Turn motor OFF
  Serial.println("Motor OFF (Gate = LOW, 0V)");
  digitalWrite(MOSFET_GATE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("Check: Motor should have stopped completely");
  Serial.println();
  
  delay(OFF_TIME);
}

// Threshold voltage test - finds minimum Vgs for motor operation
void thresholdTest() {
  Serial.println("=== Vgs THRESHOLD TEST ===");
  Serial.println("PWM\tVgs(approx)\tObservation");
  Serial.println("---\t----------\t-----------");
  
  // Ramp PWM from 0 to 255 slowly
  for (int pwm = 0; pwm <= 255; pwm += 5) {
    analogWrite(MOSFET_GATE_PIN, pwm);
    
    // Calculate approximate gate voltage
    float vgs = (pwm / 255.0) * 5.0;
    
    Serial.print(pwm);
    Serial.print("\t");
    Serial.print(vgs, 2);
    Serial.print("V");
    Serial.print("\t");
    
    // Guidance for what to observe
    if (pwm == 0) {
      Serial.println("Motor should be OFF");
    } else if (pwm < 50) {
      Serial.println("Motor likely still OFF");
    } else if (pwm < 100) {
      Serial.println("Watch for first signs of movement");
    } else if (pwm < 150) {
      Serial.println("Motor might be starting to spin");
    } else {
      Serial.println("Motor should be running");
    }
    
    delay(1000);  // Hold each PWM level for 1 second
  }
  
  Serial.println();
  Serial.println("Test complete. Review observations above.");
  Serial.println("Note the PWM value where motor first started moving.");
  
  // Return to OFF
  digitalWrite(MOSFET_GATE_PIN, LOW);
}

/*
 * ANALYSIS QUESTIONS:
 * 
 * After On/Off Test:
 * 1. Does motor stop COMPLETELY when gate is LOW?
 *    - If not, check for Gate-Source short or bad MOSFET
 * 
 * 2. Does motor run at full speed when gate is HIGH?
 *    - If not, IRF520 may not fully saturate at 5V (expected)
 *    - This is OK for 60-90mA motor, but note for future
 * 
 * 3. Is switching clean and immediate?
 *    - If delayed, check gate pull-down resistor
 *    - If erratic, check all connections
 * 
 * 4. Does MOSFET get hot?
 *    - Cool/warm: GOOD - MOSFET working efficiently
 *    - Hot: CONCERN - High Rds(on), may need better MOSFET
 * 
 * After Threshold Test:
 * 1. At what Vgs does motor first move?
 *    - This is the threshold voltage for your specific setup
 *    - IRF520 datasheet says 2-4V, but load affects this
 * 
 * 2. At what Vgs does motor reach good speed?
 *    - Compare this to maximum speed at 5V
 *    - This tells you the useful control range
 * 
 * TROUBLESHOOTING:
 * 
 * Motor always ON (even with gate LOW):
 * - MOSFET may be damaged (shorted Drain-Source)
 * - Check wiring: gate should have pull-down resistor
 * - Verify Source is connected to GND, not floating
 * 
 * Motor never turns ON (even with gate HIGH):
 * - Check motor power supply (is 5V present?)
 * - Verify MOSFET pinout (Gate/Drain/Source correct?)
 * - Test motor directly to verify it works
 * - MOSFET may be damaged (open circuit)
 * 
 * Motor partially ON when gate LOW:
 * - Gate floating (add/check pull-down resistor)
 * - MOSFET threshold voltage very low (unusual)
 * - Electrical noise coupling to gate
 * 
 * Erratic switching:
 * - Check all connections are solid
 * - Verify common GND between Arduino and motor supply
 * - Add capacitor (0.1uF) near MOSFET gate if noise suspected
 * 
 * MOSFET very hot:
 * - IRF520 not fully saturating at 5V (expected for this MOSFET)
 * - For this project (60-90mA) should be OK
 * - For higher currents, use IRLZ44N or IRL540N instead
 * - Consider adding small heatsink if continuous operation planned
 */
