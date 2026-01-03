/*
 * Experimento 11: Calibración Automática con Photo Interrupter
 *
 * OBJETIVO:
 * - Calibrar STEPS_PER_REVOLUTION automáticamente usando photo interrupter
 * - Medir error en revoluciones completas con precisión
 * - Análisis estadístico del error sistemático
 * - Ajuste iterativo hasta alcanzar precisión óptima
 *
 * METODOLOGÍA:
 * - El sensor IR detecta cuando completa EXACTAMENTE 1 revolución
 * - Ejecutamos N pasos y contamos cuántas revoluciones reales hace el motor
 * - Si pasos/revoluciones != STEPS_PER_REVOLUTION → hay error
 * - Ajustamos automáticamente hasta que coincidan
 *
 * HARDWARE REQUERIDO:
 * - Motor 28BYJ-48 + Driver ULN2003
 * - Arduino UNO o ESP32
 * - Photo Interrupter (IR Switch) del kit Keyestudio
 * - Disco codificador con 1 ranura montado en el eje
 *
 * CONEXIONES:
 * Photo Interrupter:
 *   VCC  -> Arduino 5V
 *   GND  -> Arduino GND
 *   OUT  -> Arduino Pin 2 (interrupción externa)
 *
 * Motor 28BYJ-48:
 *   IN1  -> Arduino Pin 8
 *   IN2  -> Arduino Pin 9
 *   IN3  -> Arduino Pin 10
 *   IN4  -> Arduino Pin 11
 *
 * PRINCIPIO DE CALIBRACION:
 *
 * Si ejecutamos X pasos y el sensor cuenta Y revoluciones:
 *   STEPS_PER_REVOLUTION_REAL = X / Y
 *
 * Ejemplo:
 *   - Configurado: 4097 pasos/rev
 *   - Ejecutamos: 40970 pasos (objetivo: 10 revoluciones)
 *   - Sensor detecta: 10.05 revoluciones
 *   - Real = 40970 / 10.05 = 4077 pasos/rev
 *   - Error: +20 pasos → Necesitamos REDUCIR a 4077
 */

// ============================================
// Configuración de Pines
// ============================================

#define IR_SENSOR_PIN 2  // Pin con interrupción (INT0)

#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// ============================================
// Configuración del Motor
// ============================================

// Valor inicial (de calibración manual previa)
long STEPS_PER_REVOLUTION = 4097;  // Valor a calibrar
const int DELAY_MS = 2;

// ============================================
// Configuración del Sensor
// ============================================

const int PULSES_PER_REVOLUTION = 36;  // Tu sensor genera 36 pulsos/vuelta
const float DEGREES_PER_PULSE = 360.0 / PULSES_PER_REVOLUTION;  // = 10 grados/pulso

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

int currentStep = 0;

// ============================================
// Variables de Calibración
// ============================================

// Contador de pulsos (actualizado por interrupción)
volatile long pulseCount = 0;
volatile bool pulseDetected = false;

// Configuración de pruebas - Con 36 pulsos/rev tenemos mejor precisión
const int TEST_REVOLUTIONS[] = {1, 2, 5, 10, 20};  // Revoluciones objetivo
const int NUM_TESTS = 5;

// Parámetros de calibración
const float TARGET_PRECISION = 0.01;  // Precisión objetivo: ±1% error
const int MAX_ITERATIONS = 5;
int currentIteration = 0;

// ============================================
// Estructuras de Datos
// ============================================

struct CalibrationTest {
  int targetRevolutions;        // Revoluciones objetivo
  long stepsExecuted;           // Pasos ejecutados
  long pulsesDetected;          // Pulsos detectados por el sensor
  float actualRevolutions;      // Revoluciones reales medidas (pulsos/36)
  float errorRevolutions;       // Error en revoluciones
  float errorPercent;           // Error porcentual
  float errorDegrees;           // Error en grados
  long calculatedSteps;         // Pasos/rev calculados
};

struct StatisticalAnalysis {
  float meanError;              // Error promedio (%)
  float stdDeviation;           // Desviación estándar
  float maxError;               // Error máximo
  float minError;               // Error mínimo
  bool isSystematic;            // ¿Error sistemático?
  long recommendedSteps;        // Pasos recomendados
  float confidence;             // Nivel de confianza
};

const int MAX_TESTS_STORAGE = 10;
CalibrationTest tests[MAX_TESTS_STORAGE];
int testCount = 0;

// ============================================
// Funciones de Interrupción
// ============================================

void onPulseDetected() {
  // Debounce: ignorar triggers muy rápidos (<2ms)
  // Con 36 pulsos/rev, el intervalo mínimo es ~100ms/36 = ~3ms a baja velocidad
  static unsigned long lastTrigger = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTrigger > 2) {
    pulseCount++;
    pulseDetected = true;
    lastTrigger = currentTime;
  }
}

// ============================================
// Funciones del Motor
// ============================================

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

void rotateSteps(long steps) {
  for (long i = 0; i < steps; i++) {
    executeStep(currentStep);
    currentStep++;
    if (currentStep >= 8) currentStep = 0;
    delay(DELAY_MS);
  }
}

// ============================================
// Funciones de Calibración
// ============================================

CalibrationTest performCalibrationTest(int targetRevs) {
  CalibrationTest test;
  test.targetRevolutions = targetRevs;
  test.stepsExecuted = (long)targetRevs * STEPS_PER_REVOLUTION;

  Serial.println(F("\n========================================"));
  Serial.print(F("PRUEBA: "));
  Serial.print(targetRevs);
  Serial.println(F(" revoluciones"));
  Serial.println(F("========================================\n"));

  // Esperar estabilización
  delay(500);

  // Resetear contador de pulsos
  noInterrupts();
  pulseCount = 0;
  pulseDetected = false;
  interrupts();

  Serial.print(F("Pasos a ejecutar: "));
  Serial.println(test.stepsExecuted);
  Serial.print(F("Revoluciones esperadas: "));
  Serial.println(targetRevs);
  Serial.print(F("Pulsos esperados: "));
  Serial.println(targetRevs * PULSES_PER_REVOLUTION);
  Serial.println(F("\nEjecutando..."));

  // Ejecutar movimiento
  unsigned long startTime = millis();
  rotateSteps(test.stepsExecuted);
  unsigned long endTime = millis();

  // Esperar última detección
  delay(200);

  // Leer pulsos detectados
  noInterrupts();
  long detectedPulses = pulseCount;
  interrupts();

  test.pulsesDetected = detectedPulses;

  // Calcular revoluciones reales (pulsos / 36)
  test.actualRevolutions = (float)detectedPulses / PULSES_PER_REVOLUTION;

  // Calcular error
  test.errorRevolutions = test.actualRevolutions - targetRevs;
  test.errorPercent = (test.errorRevolutions / targetRevs) * 100.0;

  // Calcular error en grados (con resolución de 10° gracias a 36 pulsos/rev)
  test.errorDegrees = test.errorRevolutions * 360.0;

  // Calcular pasos reales por revolución
  if (test.actualRevolutions > 0) {
    test.calculatedSteps = (long)((float)test.stepsExecuted / test.actualRevolutions);
  } else {
    test.calculatedSteps = STEPS_PER_REVOLUTION;
  }

  // Resultados
  Serial.println(F("\n--- RESULTADOS ---"));
  Serial.print(F("Pulsos detectados: "));
  Serial.print(test.pulsesDetected);
  Serial.print(F(" / "));
  Serial.println(targetRevs * PULSES_PER_REVOLUTION);

  Serial.print(F("Revoluciones medidas: "));
  Serial.println(test.actualRevolutions, 4);

  Serial.print(F("Error en revoluciones: "));
  if (test.errorRevolutions >= 0) Serial.print(F("+"));
  Serial.print(test.errorRevolutions, 4);
  Serial.println(F(" rev"));

  Serial.print(F("Error en grados: "));
  if (test.errorDegrees >= 0) Serial.print(F("+"));
  Serial.print(test.errorDegrees, 2);
  Serial.println(F("°"));

  Serial.print(F("Error porcentual: "));
  if (test.errorPercent >= 0) Serial.print(F("+"));
  Serial.print(test.errorPercent, 3);
  Serial.println(F("%"));

  Serial.print(F("Pasos/rev calculados: "));
  Serial.println(test.calculatedSteps);

  Serial.print(F("Tiempo: "));
  Serial.print((endTime - startTime) / 1000.0, 2);
  Serial.println(F(" segundos"));

  Serial.println();

  return test;
}

void runCalibrationBattery() {
  Serial.println(F("\n###########################################"));
  Serial.println(F("BATERIA DE PRUEBAS DE CALIBRACION"));
  Serial.println(F("###########################################\n"));

  Serial.print(F("Valor actual: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev\n"));

  testCount = 0;

  for (int i = 0; i < NUM_TESTS; i++) {
    tests[testCount] = performCalibrationTest(TEST_REVOLUTIONS[i]);
    testCount++;

    // Pequeña pausa entre pruebas
    delay(1000);
  }

  Serial.println(F("\n###########################################"));
  Serial.println(F("BATERIA COMPLETADA"));
  Serial.println(F("###########################################\n"));
}

StatisticalAnalysis analyzeResults() {
  StatisticalAnalysis analysis;

  if (testCount == 0) {
    Serial.println(F("ERROR: No hay datos"));
    return analysis;
  }

  // Calcular estadísticas
  float sumError = 0;
  float sumSteps = 0;
  float maxErr = tests[0].errorPercent;
  float minErr = tests[0].errorPercent;

  for (int i = 0; i < testCount; i++) {
    sumError += tests[i].errorPercent;
    sumSteps += tests[i].calculatedSteps;

    if (tests[i].errorPercent > maxErr) maxErr = tests[i].errorPercent;
    if (tests[i].errorPercent < minErr) minErr = tests[i].errorPercent;
  }

  analysis.meanError = sumError / testCount;
  analysis.maxError = maxErr;
  analysis.minError = minErr;

  // Desviación estándar
  float sumSquaredDiff = 0;
  for (int i = 0; i < testCount; i++) {
    float diff = tests[i].errorPercent - analysis.meanError;
    sumSquaredDiff += diff * diff;
  }
  analysis.stdDeviation = sqrt(sumSquaredDiff / testCount);

  // Determinar si es sistemático
  float cv = analysis.stdDeviation / (abs(analysis.meanError) + 0.01);  // Coef. variación
  analysis.isSystematic = (cv < 0.5);  // Si CV < 0.5, es sistemático

  // Calcular pasos recomendados (promedio ponderado)
  analysis.recommendedSteps = (long)(sumSteps / testCount);

  // Nivel de confianza
  analysis.confidence = 1.0 - (analysis.stdDeviation / 100.0);
  if (analysis.confidence > 1.0) analysis.confidence = 1.0;
  if (analysis.confidence < 0.0) analysis.confidence = 0.0;

  return analysis;
}

// ============================================
// Funciones de Visualización
// ============================================

void printTestSummary() {
  Serial.println(F("\n========================================"));
  Serial.println(F("RESUMEN DE PRUEBAS"));
  Serial.println(F("========================================\n"));

  Serial.println(F("Rev\tPulsos\tDetect\tRev Real\tError°\t\tError%\t\tPasos/Rev"));
  Serial.println(F("---\t------\t------\t--------\t------\t\t------\t\t---------"));

  for (int i = 0; i < testCount; i++) {
    Serial.print(tests[i].targetRevolutions);
    Serial.print(F("\t"));

    int expectedPulses = tests[i].targetRevolutions * PULSES_PER_REVOLUTION;
    Serial.print(expectedPulses);
    Serial.print(F("\t"));

    Serial.print(tests[i].pulsesDetected);
    Serial.print(F("\t"));

    Serial.print(tests[i].actualRevolutions, 3);
    Serial.print(F("\t\t"));

    if (tests[i].errorDegrees >= 0) Serial.print(F("+"));
    Serial.print(tests[i].errorDegrees, 1);
    Serial.print(F("°\t\t"));

    if (tests[i].errorPercent >= 0) Serial.print(F("+"));
    Serial.print(tests[i].errorPercent, 2);
    Serial.print(F("%\t\t"));

    Serial.println(tests[i].calculatedSteps);
  }

  Serial.println();
}

void printStatisticalAnalysis(StatisticalAnalysis analysis) {
  Serial.println(F("\n========================================"));
  Serial.println(F("ANALISIS ESTADISTICO"));
  Serial.println(F("========================================\n"));

  Serial.print(F("Error promedio: "));
  if (analysis.meanError >= 0) Serial.print(F("+"));
  Serial.print(analysis.meanError, 3);
  Serial.println(F("%"));

  Serial.print(F("Desviacion estandar: "));
  Serial.print(analysis.stdDeviation, 3);
  Serial.println(F("%"));

  Serial.print(F("Error maximo: "));
  if (analysis.maxError >= 0) Serial.print(F("+"));
  Serial.print(analysis.maxError, 3);
  Serial.println(F("%"));

  Serial.print(F("Error minimo: "));
  if (analysis.minError >= 0) Serial.print(F("+"));
  Serial.print(analysis.minError, 3);
  Serial.println(F("%"));

  Serial.print(F("Nivel de confianza: "));
  Serial.print(analysis.confidence * 100.0, 1);
  Serial.println(F("%"));

  Serial.println();

  // Tipo de error
  Serial.println(F("TIPO DE ERROR:"));
  if (analysis.isSystematic) {
    if (analysis.meanError > 0) {
      Serial.println(F("- ERROR SISTEMATICO POSITIVO"));
      Serial.println(F("  Motor ejecuta MAS revoluciones de lo esperado"));
      Serial.println(F("  STEPS_PER_REVOLUTION es muy BAJO"));
    } else {
      Serial.println(F("- ERROR SISTEMATICO NEGATIVO"));
      Serial.println(F("  Motor ejecuta MENOS revoluciones de lo esperado"));
      Serial.println(F("  STEPS_PER_REVOLUTION es muy ALTO"));
    }
    Serial.print(F("  Recomendacion: ajustar a "));
    Serial.println(analysis.recommendedSteps);
  } else {
    Serial.println(F("- ERROR ALEATORIO"));
    Serial.println(F("  Error no sistemático, posiblemente mecánico"));
    Serial.println(F("  Valor actual es razonable"));
  }

  Serial.println();

  // Evaluación
  Serial.println(F("EVALUACION:"));
  if (abs(analysis.meanError) < TARGET_PRECISION * 100) {
    Serial.println(F("OK CALIBRACION OPTIMA"));
  } else {
    Serial.println(F("REQUIERE AJUSTE"));
  }

  Serial.println();
}

void printDataLog() {
  Serial.println(F("\n========================================"));
  Serial.println(F("REGISTRO CSV"));
  Serial.println(F("========================================\n"));

  Serial.println(F("target_rev,steps,pulses_expected,pulses_detected,actual_rev,error_rev,error_deg,error_pct,calc_steps"));

  for (int i = 0; i < testCount; i++) {
    Serial.print(tests[i].targetRevolutions);
    Serial.print(F(","));
    Serial.print(tests[i].stepsExecuted);
    Serial.print(F(","));
    Serial.print(tests[i].targetRevolutions * PULSES_PER_REVOLUTION);
    Serial.print(F(","));
    Serial.print(tests[i].pulsesDetected);
    Serial.print(F(","));
    Serial.print(tests[i].actualRevolutions, 4);
    Serial.print(F(","));
    Serial.print(tests[i].errorRevolutions, 4);
    Serial.print(F(","));
    Serial.print(tests[i].errorDegrees, 2);
    Serial.print(F(","));
    Serial.print(tests[i].errorPercent, 3);
    Serial.print(F(","));
    Serial.println(tests[i].calculatedSteps);
  }

  Serial.println();
}

void printRecommendations(StatisticalAnalysis analysis) {
  Serial.println(F("========================================"));
  Serial.println(F("RECOMENDACIONES"));
  Serial.println(F("========================================\n"));

  Serial.print(F("Valor actual: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev"));

  if (analysis.isSystematic && abs(analysis.meanError) > TARGET_PRECISION * 100) {
    Serial.print(F("Valor recomendado: "));
    Serial.print(analysis.recommendedSteps);
    Serial.println(F(" pasos/rev"));

    long difference = analysis.recommendedSteps - STEPS_PER_REVOLUTION;
    Serial.print(F("Diferencia: "));
    if (difference >= 0) Serial.print(F("+"));
    Serial.print(difference);
    Serial.println(F(" pasos"));

    Serial.println(F("\nPROXIMOS PASOS:"));
    Serial.println(F("1. Actualizar STEPS_PER_REVOLUTION en este sketch"));
    Serial.println(F("2. Volver a ejecutar para verificar"));
    Serial.println(F("3. Repetir hasta error < 1%"));
  } else {
    Serial.println(F("\nCALIBRACION COMPLETADA!"));
    Serial.println(F("Valor actual es OPTIMO"));
    Serial.println(F("\nPROXIMOS PASOS:"));
    Serial.println(F("1. Actualizar todos los sketches"));
    Serial.println(F("2. Actualizar CALIBRATION.md"));
  }

  Serial.println();
}

void printHeader() {
  Serial.println(F("\n============================================="));
  Serial.println(F("  CALIBRACION AUTOMATICA CON IR SENSOR"));
  Serial.println(F("  Motor 28BYJ-48 + Photo Interrupter"));
  Serial.println(F("  Resolucion: 36 pulsos/revolucion (10°)"));
  Serial.println(F("=============================================\n"));

  Serial.print(F("Iteracion: "));
  Serial.print(currentIteration + 1);
  Serial.print(F(" / "));
  Serial.println(MAX_ITERATIONS);

  Serial.print(F("Precision objetivo: ±"));
  Serial.print(TARGET_PRECISION * 100, 1);
  Serial.println(F("%"));

  Serial.print(F("Resolucion angular: "));
  Serial.print(DEGREES_PER_PULSE, 1);
  Serial.println(F("° por pulso"));

  Serial.println();
}

// ============================================
// Setup y Loop
// ============================================

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configurar pines
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();

  // Configurar interrupción
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), onPulseDetected, FALLING);

  printHeader();

  // Verificar sensor
  Serial.println(F("Verificando sensor..."));
  int sensorState = digitalRead(IR_SENSOR_PIN);
  Serial.print(F("Estado del sensor: "));
  Serial.println(sensorState == HIGH ? F("BLOQUEADO") : F("LIBRE"));

  Serial.println(F("\nIMPORTANTE:"));
  Serial.println(F("- Asegurate de que el disco codificador este instalado"));
  Serial.println(F("- El sensor debe detectar la ranura del disco"));
  Serial.println(F("- Si no detecta, ejecuta sketch 10 primero\n"));

  delay(3000);

  // Iniciar calibración automática
  performAutoCalibration();
}

void loop() {
  stopMotor();
  delay(1000);
}

void performAutoCalibration() {
  for (currentIteration = 0; currentIteration < MAX_ITERATIONS; currentIteration++) {
    Serial.println(F("\n###########################################"));
    Serial.print(F("ITERACION "));
    Serial.print(currentIteration + 1);
    Serial.println(F("###########################################\n"));

    // Ejecutar batería
    runCalibrationBattery();

    // Analizar
    StatisticalAnalysis analysis = analyzeResults();

    // Mostrar resultados
    printTestSummary();
    printStatisticalAnalysis(analysis);
    printDataLog();
    printRecommendations(analysis);

    // Verificar si alcanzamos precisión
    if (abs(analysis.meanError) < TARGET_PRECISION * 100) {
      Serial.println(F("\n*** CALIBRACION EXITOSA ***"));
      break;
    }

    // Ajustar automáticamente
    if (analysis.isSystematic && currentIteration < MAX_ITERATIONS - 1) {
      Serial.println(F("\n*** APLICANDO AJUSTE ***"));
      Serial.print(F("De "));
      Serial.print(STEPS_PER_REVOLUTION);
      Serial.print(F(" a "));
      Serial.println(analysis.recommendedSteps);

      STEPS_PER_REVOLUTION = analysis.recommendedSteps;

      Serial.println(F("\nEsperando 5 segundos...\n"));
      delay(5000);
    } else {
      break;
    }
  }

  Serial.println(F("\n============================================="));
  Serial.println(F("  CALIBRACION FINALIZADA"));
  Serial.println(F("=============================================\n"));

  Serial.print(F("Valor final: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev"));
}

/*
 * GUIA DE USO:
 *
 * 1. REQUISITOS PREVIOS:
 *    - Ejecutar sketch 10_ir_sensor_verification primero
 *    - Verificar que el sensor detecta revoluciones correctamente
 *    - Disco codificador instalado y alineado
 *
 * 2. PROCESO AUTOMATICO:
 *    - Ejecuta 5 pruebas (1, 5, 10, 20, 50 revoluciones)
 *    - Cuenta revoluciones reales con el sensor IR
 *    - Calcula STEPS_PER_REVOLUTION óptimo
 *    - Ajusta automáticamente si hay error sistemático
 *    - Repite hasta error < 1%
 *
 * 3. VENTAJAS VS CALIBRACION MANUAL:
 *    - Totalmente objetivo (sin juicio visual)
 *    - Repetible y consistente
 *    - Detecta error sistemático automáticamente
 *    - Más rápido (2-5 minutos)
 *
 * 4. LIMITACIONES:
 *    - Resolución: 1 revolución completa (no mide ángulos)
 *    - Requiere disco codificador preciso
 *    - Sensible a rebotes del sensor (debounce)
 *
 * 5. MEJORAS POSIBLES:
 *    - Disco con múltiples ranuras (mayor resolución)
 *    - Promedio de múltiples mediciones
 *    - Compensación de temperatura
 *
 * 6. INTERPRETACION:
 *    - Error positivo: Motor da más vueltas → Reducir pasos
 *    - Error negativo: Motor da menos vueltas → Aumentar pasos
 *    - Error < 1%: Calibración óptima
 */
