/*
 * Experimento 3: Medición de Velocidad y Precisión del Motor Paso a Paso
 *
 * OBJETIVO:
 * - Caracterizar el rendimiento del motor 28BYJ-48
 * - Determinar la velocidad máxima confiable
 * - Medir la precisión de posicionamiento
 * - Documentar consumo de tiempo real vs teórico
 *
 * HARDWARE:
 * - Motor 28BYJ-48 (5V)
 * - Driver ULN2003APG
 * - Arduino UNO o ESP32
 *
 * CONEXIONES:
 * Arduino Pin 8  -> Driver IN1
 * Arduino Pin 9  -> Driver IN2
 * Arduino Pin 10 -> Driver IN3
 * Arduino Pin 11 -> Driver IN4
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Constantes del motor
const int STEPS_PER_REVOLUTION = 4096;  // Half-step mode
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION;  // ~0.088 grados por paso

// Secuencia Half-Step
const int halfStepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// Variables de control
int currentStep = 0;
unsigned long totalSteps = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("========================================"));
  Serial.println(F("Motor Paso a Paso 28BYJ-48"));
  Serial.println(F("Exp 3: Medicion y Caracterizacion"));
  Serial.println(F("========================================"));
  Serial.println();

  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();
  delay(2000);

  // Menú de pruebas
  Serial.println(F("PRUEBAS AUTOMATICAS"));
  Serial.println(F("-------------------"));
  Serial.println();

  delay(1000);

  // Test 1: Velocidades diferentes
  testDifferentSpeeds();

  delay(3000);

  // Test 2: Precisión angular
  testAngularPrecision();

  delay(3000);

  // Test 3: Reversibilidad
  testReversibility();

  delay(3000);

  // Test 4: Múltiples revoluciones
  testMultipleRevolutions();

  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("TODAS LAS PRUEBAS COMPLETADAS"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Revisa resultados en documentacion."));
  Serial.println(F("Motor en modo espera."));
}

void loop() {
  // Mantener el motor apagado después de las pruebas
  stopMotor();
  delay(1000);
}

// ============================================
// TEST 1: Velocidades diferentes
// ============================================
void testDifferentSpeeds() {
  Serial.println(F("========================================"));
  Serial.println(F("  TEST 1: VELOCIDADES DIFERENTES"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Probando delays entre pasos..."));
  Serial.println();

  int delayTimes[] = {10, 5, 3, 2, 1};  // Milisegundos entre pasos
  int numTests = 5;

  for (int i = 0; i < numTests; i++) {
    int delayMs = delayTimes[i];

    Serial.print("Test ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numTests);
    Serial.print(" - Delay: ");
    Serial.print(delayMs);
    Serial.println(" ms");

    // Calcular velocidad teórica en RPM
    float stepsPerSecond = 1000.0 / delayMs;
    float rpm = (stepsPerSecond / STEPS_PER_REVOLUTION) * 60.0;

    Serial.print(F("  Vel teorica: "));
    Serial.print(rpm, 2);
    Serial.println(F(" RPM"));

    // Ejecutar una revolución completa
    unsigned long startTime = millis();
    rotateSteps(STEPS_PER_REVOLUTION, delayMs);
    unsigned long endTime = millis();

    float actualTimeSeconds = (endTime - startTime) / 1000.0;
    float actualRpm = 60.0 / actualTimeSeconds;

    Serial.print(F("  Tiempo real: "));
    Serial.print(actualTimeSeconds, 2);
    Serial.println(F(" seg"));

    Serial.print(F("  Vel real: "));
    Serial.print(actualRpm, 2);
    Serial.println(F(" RPM"));

    Serial.println();

    delay(2000);  // Pausa entre pruebas
  }

  stopMotor();
}

// ============================================
// TEST 2: Precisión angular
// ============================================
void testAngularPrecision() {
  Serial.println(F("========================================"));
  Serial.println(F("  TEST 2: PRECISION ANGULAR"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Prueba de angulos especificos..."));
  Serial.println(F("Marca posicion inicial del eje."));
  Serial.println();

  delay(3000);

  // Probar diferentes ángulos
  int angles[] = {45, 90, 180, 360};
  int numAngles = 4;

  for (int i = 0; i < numAngles; i++) {
    int targetAngle = angles[i];
    long steps = ((long)targetAngle * STEPS_PER_REVOLUTION) / 360;

    Serial.print(F("Rotando "));
    Serial.print(targetAngle);
    Serial.println(F(" grados..."));

    Serial.print(F("  Pasos: "));
    Serial.println(steps);

    rotateSteps((int)steps, 2);

    Serial.println(F("  Verifica angulo."));
    Serial.println();

    delay(3000);
  }

  // Regresar a la posición inicial
  Serial.println(F("Regresando a posicion inicial..."));
  long totalAngle = -(45L + 90L + 180L + 360L);
  rotateSteps((int)(totalAngle * STEPS_PER_REVOLUTION / 360), 2);
  Serial.println(F("Regreso a posicion original?"));
  Serial.println();

  stopMotor();
}

// ============================================
// TEST 3: Reversibilidad
// ============================================
void testReversibility() {
  Serial.println(F("========================================"));
  Serial.println(F("  TEST 3: REVERSIBILIDAD"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Probando adelante-atras..."));
  Serial.println();

  int cycles = 5;

  for (int i = 0; i < cycles; i++) {
    Serial.print("Ciclo ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.println(cycles);

    Serial.println(F("  > Adelante 180°"));
    rotateSteps(STEPS_PER_REVOLUTION / 2, 2);
    delay(500);

    Serial.println(F("  < Atras 180°"));
    rotateSteps(-STEPS_PER_REVOLUTION / 2, 2);
    delay(500);

    Serial.println();
  }

  Serial.println(F("Regreso a posicion inicial?"));
  Serial.println();

  stopMotor();
}

// ============================================
// TEST 4: Múltiples revoluciones continuas
// ============================================
void testMultipleRevolutions() {
  Serial.println(F("========================================"));
  Serial.println(F("  TEST 4: MULTIPLES REVOLUCIONES"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Ejecutando 5 revoluciones continuas..."));
  Serial.println(F("Marca posicion inicial."));
  Serial.println();

  delay(3000);

  int revolutions = 5;
  unsigned long startTime = millis();

  for (int i = 0; i < revolutions; i++) {
    Serial.print("Revolución ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.println(revolutions);

    rotateSteps(STEPS_PER_REVOLUTION, 2);
  }

  unsigned long endTime = millis();
  float totalTime = (endTime - startTime) / 1000.0;

  Serial.println();
  Serial.print(F("Tiempo total: "));
  Serial.print(totalTime, 2);
  Serial.println(F(" seg"));

  Serial.print(F("Tiempo promedio: "));
  Serial.print(totalTime / revolutions, 2);
  Serial.println(F(" seg"));

  Serial.println();
  Serial.println(F("Completo 5 revoluciones exactas?"));
  Serial.println(F("Regreso a posicion inicial?"));
  Serial.println();

  stopMotor();
}

// ============================================
// Funciones auxiliares
// ============================================

void rotateSteps(int steps, int delayMs) {
  int direction = (steps >= 0) ? 1 : -1;
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    executeStep(currentStep);

    currentStep += direction;

    if (currentStep >= 8) currentStep = 0;
    if (currentStep < 0) currentStep = 7;

    totalSteps++;

    delay(delayMs);
  }
}

void executeStep(int step) {
  digitalWrite(IN1, halfStepSequence[step][0]);
  digitalWrite(IN2, halfStepSequence[step][1]);
  digitalWrite(IN3, halfStepSequence[step][2]);
  digitalWrite(IN4, halfStepSequence[step][3]);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/*
 * RESULTADOS ESPERADOS:
 *
 * Test 1 - Velocidades:
 * - A 10ms delay: ~3 RPM
 * - A 5ms delay: ~6 RPM
 * - A 2ms delay: ~15 RPM
 * - A 1ms delay: El motor puede empezar a perder pasos (depende de la carga)
 *
 * Test 2 - Precisión Angular:
 * - El motor debería alcanzar los ángulos con precisión de ±2 grados
 * - Después de múltiples movimientos, debería regresar cerca de la posición inicial
 *
 * Test 3 - Reversibilidad:
 * - El motor debería poder invertir dirección sin problemas
 * - Después de 5 ciclos, debería estar en la posición inicial
 *
 * Test 4 - Múltiples Revoluciones:
 * - Cada revolución debería tomar aproximadamente el mismo tiempo
 * - Después de 5 revoluciones completas, debería estar en la posición inicial
 *
 * ANOTA TUS OBSERVACIONES:
 * - ¿A qué velocidad (delay) el motor funciona mejor?
 * - ¿Cuándo empieza a perder pasos?
 * - ¿Qué tan preciso es el posicionamiento?
 * - ¿Hay diferencia entre CW y CCW?
 */
