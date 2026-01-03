/*
 * Experimento 7: Sistema de Calibración Automática con AS5600
 *
 * OBJETIVO:
 * - Calibración automática de STEPS_PER_REVOLUTION usando sensor AS5600
 * - Medición precisa de error en Grados/Minutos/Segundos
 * - Análisis estadístico completo (error sistemático vs aleatorio)
 * - Calibración iterativa hasta alcanzar precisión objetivo
 * - Registro de datos para análisis posterior
 *
 * HARDWARE REQUERIDO:
 * - Motor 28BYJ-48 + Driver ULN2003
 * - Arduino UNO o ESP32
 * - Sensor AS5600 (Encoder Magnético I2C)
 * - Imán diametral montado en el eje del motor
 *
 * CONEXIONES MOTOR:
 * Arduino Pin 8  -> Driver IN1
 * Arduino Pin 9  -> Driver IN2
 * Arduino Pin 10 -> Driver IN3
 * Arduino Pin 11 -> Driver IN4
 *
 * CONEXIONES AS5600:
 * AS5600 SDA -> Arduino A4 (UNO) / GPIO21 (ESP32)
 * AS5600 SCL -> Arduino A5 (UNO) / GPIO22 (ESP32)
 * AS5600 VCC -> 5V
 * AS5600 GND -> GND
 *
 * METODOLOGÍA:
 * 1. Ejecutar batería de pruebas con diferentes números de revoluciones
 * 2. Medir error absoluto con sensor AS5600
 * 3. Calcular error por revolución
 * 4. Determinar si es error sistemático o aleatorio
 * 5. Ajustar STEPS_PER_REVOLUTION automáticamente
 * 6. Repetir hasta alcanzar precisión objetivo (<0.1°)
 *
 * PRECISIÓN OBJETIVO:
 * - Error absoluto: <0.1° (6 minutos de arco)
 * - Error por revolución: <0.02° por revolución
 * - Desviación estándar: <0.05°
 */

#include "AS5600.h"

// ============================================
// Configuración del Motor
// ============================================

#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Valor inicial (de calibración manual previa)
long STEPS_PER_REVOLUTION = 4097;  // Valor a calibrar automáticamente
const int DELAY_MS = 2;

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
// Estructuras de Datos
// ============================================

struct CalibrationTest {
  int targetRevolutions;      // Revoluciones objetivo
  long stepsExecuted;         // Pasos ejecutados
  AngleMeasurement initial;   // Ángulo inicial
  AngleMeasurement final;     // Ángulo final
  float errorDegrees;         // Error total en grados
  float errorMinutes;         // Error en minutos de arco
  float errorSeconds;         // Error en segundos de arco
  float errorPerRev;          // Error por revolución
  float errorPercent;         // Error porcentual
};

struct StatisticalAnalysis {
  float meanError;            // Error promedio
  float absError;             // Error absoluto promedio
  float standardDeviation;    // Desviación estándar
  float maxError;             // Error máximo
  float minError;             // Error mínimo
  bool isSystematic;          // ¿Error sistemático?
  float systematicError;      // Componente sistemático
  float randomError;          // Componente aleatorio
  long recommendedSteps;      // Pasos recomendados
};

// ============================================
// Variables Globales
// ============================================

AS5600 sensor;
const int MAX_TESTS = 10;
CalibrationTest tests[MAX_TESTS];
int testCount = 0;

// Configuración de pruebas
const int TEST_REVOLUTIONS[] = {1, 5, 10, 20, 50};
const int NUM_TESTS = 5;

// Parámetros de calibración iterativa
const float TARGET_PRECISION = 0.1;      // Objetivo: <0.1° de error
const int MAX_ITERATIONS = 5;             // Máximo 5 iteraciones
int currentIteration = 0;

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
  bool direction = (steps >= 0);
  long absSteps = abs(steps);

  for (long i = 0; i < absSteps; i++) {
    executeStep(currentStep);

    if (direction) {
      currentStep++;
      if (currentStep >= 8) currentStep = 0;
    } else {
      currentStep--;
      if (currentStep < 0) currentStep = 7;
    }

    delay(DELAY_MS);
  }
}

void rotateRevolutions(int revolutions) {
  long steps = (long)revolutions * STEPS_PER_REVOLUTION;
  rotateSteps(steps);
}

// ============================================
// Funciones de Calibración
// ============================================

CalibrationTest performAutoTest(int revolutions) {
  CalibrationTest test;

  Serial.print(F("\n>>> Ejecutando prueba: "));
  Serial.print(revolutions);
  Serial.println(F(" revoluciones"));

  // Esperar estabilización
  delay(500);

  // 1. Registrar ángulo inicial
  test.initial = sensor.readPreciseAngle();
  sensor.resetRevolutionCount();

  Serial.print(F("Angulo inicial: "));
  printAngleDMS(test.initial);
  Serial.println();

  // 2. Ejecutar revoluciones
  test.targetRevolutions = revolutions;
  test.stepsExecuted = (long)revolutions * STEPS_PER_REVOLUTION;

  Serial.print(F("Ejecutando "));
  Serial.print(test.stepsExecuted);
  Serial.println(F(" pasos..."));

  rotateRevolutions(revolutions);

  // Esperar estabilización
  delay(500);

  // 3. Registrar ángulo final
  test.final = sensor.readPreciseAngle();

  Serial.print(F("Angulo final: "));
  printAngleDMS(test.final);
  Serial.println();

  // 4. Calcular error
  // Error esperado: debería volver a 0° (o muy cerca)
  float expectedAngle = 0.0;  // Después de N revoluciones completas
  float actualAngle = test.final.degrees;

  // Manejar wrap-around (359° es casi 0°)
  if (actualAngle > 180) {
    actualAngle = actualAngle - 360.0;
  }

  test.errorDegrees = actualAngle - expectedAngle;
  test.errorPerRev = test.errorDegrees / revolutions;
  test.errorPercent = (test.errorDegrees / (revolutions * 360.0)) * 100.0;

  // Convertir error a minutos y segundos
  int errorDeg, errorMin;
  float errorSec;
  AS5600::decimalToDMS(abs(test.errorDegrees), errorDeg, errorMin, errorSec);
  test.errorMinutes = errorDeg * 60.0 + errorMin + (errorSec / 60.0);
  test.errorSeconds = AS5600::errorInArcSeconds(abs(test.errorDegrees));

  // Mostrar resultados
  Serial.print(F("Error total: "));
  if (test.errorDegrees >= 0) Serial.print(F("+"));
  printErrorDMS(test.errorDegrees);
  Serial.print(F(" ("));
  Serial.print(test.errorDegrees, 4);
  Serial.println(F(" grados)"));

  Serial.print(F("Error/rev: "));
  if (test.errorPerRev >= 0) Serial.print(F("+"));
  Serial.print(test.errorPerRev, 4);
  Serial.println(F(" grados/rev"));

  Serial.println(F("OK Prueba completada\n"));

  delay(1000);

  return test;
}

void runCalibrationBattery() {
  Serial.println(F("\n========================================"));
  Serial.println(F("BATERIA DE PRUEBAS DE CALIBRACION"));
  Serial.println(F("========================================\n"));

  Serial.print(F("Valor actual STEPS_PER_REVOLUTION: "));
  Serial.println(STEPS_PER_REVOLUTION);
  Serial.println();

  testCount = 0;

  for (int i = 0; i < NUM_TESTS; i++) {
    tests[testCount] = performAutoTest(TEST_REVOLUTIONS[i]);
    testCount++;

    // Volver a posición inicial
    if (i < NUM_TESTS - 1) {
      Serial.println(F("Volviendo a posicion inicial..."));
      rotateRevolutions(-TEST_REVOLUTIONS[i]);
      delay(2000);
    }
  }

  Serial.println(F("========================================"));
  Serial.println(F("BATERIA COMPLETADA"));
  Serial.println(F("========================================\n"));
}

StatisticalAnalysis analyzeResults() {
  StatisticalAnalysis analysis;

  if (testCount == 0) {
    Serial.println(F("ERROR: No hay datos para analizar"));
    return analysis;
  }

  // Calcular promedios
  float sumError = 0;
  float sumAbsError = 0;
  float maxErr = tests[0].errorPerRev;
  float minErr = tests[0].errorPerRev;

  for (int i = 0; i < testCount; i++) {
    sumError += tests[i].errorPerRev;
    sumAbsError += abs(tests[i].errorPerRev);

    if (tests[i].errorPerRev > maxErr) maxErr = tests[i].errorPerRev;
    if (tests[i].errorPerRev < minErr) minErr = tests[i].errorPerRev;
  }

  analysis.meanError = sumError / testCount;
  analysis.absError = sumAbsError / testCount;
  analysis.maxError = maxErr;
  analysis.minError = minErr;

  // Calcular desviación estándar
  float sumSquaredDiff = 0;
  for (int i = 0; i < testCount; i++) {
    float diff = tests[i].errorPerRev - analysis.meanError;
    sumSquaredDiff += diff * diff;
  }
  analysis.standardDeviation = sqrt(sumSquaredDiff / testCount);

  // Determinar si es error sistemático
  // Si el error promedio es mucho mayor que la desviación estándar, es sistemático
  float systematicRatio = abs(analysis.meanError) / (analysis.standardDeviation + 0.001);
  analysis.isSystematic = (systematicRatio > 3.0);  // Regla 3-sigma

  analysis.systematicError = analysis.meanError;
  analysis.randomError = analysis.standardDeviation;

  // Calcular pasos recomendados
  if (analysis.isSystematic && abs(analysis.meanError) > 0.01) {
    // Ajustar basado en error sistemático
    float correctionFactor = 1.0 + (analysis.meanError / 360.0);
    analysis.recommendedSteps = (long)(STEPS_PER_REVOLUTION / correctionFactor);
  } else {
    analysis.recommendedSteps = STEPS_PER_REVOLUTION;  // Ya está bien calibrado
  }

  return analysis;
}

// ============================================
// Funciones de Visualización
// ============================================

void printAngleDMS(AngleMeasurement angle) {
  Serial.print(angle.deg);
  Serial.print(F("°"));

  if (angle.minutes < 10) Serial.print(F("0"));
  Serial.print(angle.minutes);
  Serial.print(F("'"));

  if (angle.seconds < 10) Serial.print(F("0"));
  Serial.print(angle.seconds, 2);
  Serial.print(F("\""));
}

void printErrorDMS(float errorDegrees) {
  int deg, min;
  float sec;
  AS5600::decimalToDMS(abs(errorDegrees), deg, min, sec);

  if (deg != 0) {
    Serial.print(deg);
    Serial.print(F("° "));
  }
  if (min != 0) {
    Serial.print(min);
    Serial.print(F("' "));
  }
  Serial.print(sec, 2);
  Serial.print(F("\""));
}

void printTestSummary() {
  Serial.println(F("\n========================================"));
  Serial.println(F("RESUMEN DE PRUEBAS"));
  Serial.println(F("========================================\n"));

  Serial.println(F("Rev\tError(°)\tError/Rev(°)\tError(M'S\")"));
  Serial.println(F("---\t--------\t------------\t-----------"));

  for (int i = 0; i < testCount; i++) {
    Serial.print(tests[i].targetRevolutions);
    Serial.print(F("\t"));

    if (tests[i].errorDegrees >= 0) Serial.print(F("+"));
    Serial.print(tests[i].errorDegrees, 4);
    Serial.print(F("\t\t"));

    if (tests[i].errorPerRev >= 0) Serial.print(F("+"));
    Serial.print(tests[i].errorPerRev, 4);
    Serial.print(F("\t\t"));

    printErrorDMS(tests[i].errorDegrees);
    Serial.println();
  }

  Serial.println();
}

void printStatisticalAnalysis(StatisticalAnalysis analysis) {
  Serial.println(F("\n========================================"));
  Serial.println(F("ANALISIS ESTADISTICO"));
  Serial.println(F("========================================\n"));

  Serial.print(F("Error promedio por rev: "));
  if (analysis.meanError >= 0) Serial.print(F("+"));
  Serial.print(analysis.meanError, 4);
  Serial.println(F(" grados"));

  Serial.print(F("Error absoluto promedio: "));
  Serial.print(analysis.absError, 4);
  Serial.println(F(" grados"));

  Serial.print(F("Desviacion estandar: "));
  Serial.print(analysis.standardDeviation, 4);
  Serial.println(F(" grados"));

  Serial.print(F("Error maximo: "));
  if (analysis.maxError >= 0) Serial.print(F("+"));
  Serial.print(analysis.maxError, 4);
  Serial.println(F(" grados/rev"));

  Serial.print(F("Error minimo: "));
  if (analysis.minError >= 0) Serial.print(F("+"));
  Serial.print(analysis.minError, 4);
  Serial.println(F(" grados/rev"));

  Serial.println();

  // Tipo de error
  Serial.println(F("TIPO DE ERROR:"));
  if (analysis.isSystematic) {
    if (analysis.systematicError > 0) {
      Serial.println(F("- ERROR SISTEMATICO POSITIVO"));
      Serial.println(F("  El motor gira consistentemente DE MAS."));
      Serial.println(F("  STEPS_PER_REVOLUTION es muy ALTO."));
      Serial.print(F("  Recomendacion: reducir a ~"));
      Serial.println(analysis.recommendedSteps);
    } else {
      Serial.println(F("- ERROR SISTEMATICO NEGATIVO"));
      Serial.println(F("  El motor gira consistentemente DE MENOS."));
      Serial.println(F("  STEPS_PER_REVOLUTION es muy BAJO."));
      Serial.print(F("  Recomendacion: aumentar a ~"));
      Serial.println(analysis.recommendedSteps);
    }
  } else {
    Serial.println(F("- ERROR ALEATORIO"));
    Serial.println(F("  El error no es sistemático."));
    Serial.println(F("  Posibles causas: juego mecánico, vibración."));
    Serial.println(F("  Calibracion actual es adecuada."));
  }

  Serial.println();

  // Evaluación de precisión
  Serial.println(F("EVALUACION DE PRECISION:"));
  if (analysis.absError < TARGET_PRECISION) {
    Serial.print(F("OK CALIBRACION OPTIMA (error < "));
    Serial.print(TARGET_PRECISION, 2);
    Serial.println(F(" grados)"));
  } else {
    Serial.print(F("REQUIERE AJUSTE (error > "));
    Serial.print(TARGET_PRECISION, 2);
    Serial.println(F(" grados)"));
  }

  Serial.println();
}

void printDataLog() {
  Serial.println(F("\n========================================"));
  Serial.println(F("REGISTRO DE DATOS (CSV)"));
  Serial.println(F("========================================\n"));

  Serial.println(F("timestamp,revolutions,steps,initial_deg,final_deg,error_deg,error_min,error_sec,error_per_rev"));

  for (int i = 0; i < testCount; i++) {
    Serial.print(tests[i].initial.timestamp);
    Serial.print(F(","));
    Serial.print(tests[i].targetRevolutions);
    Serial.print(F(","));
    Serial.print(tests[i].stepsExecuted);
    Serial.print(F(","));
    Serial.print(tests[i].initial.degrees, 4);
    Serial.print(F(","));
    Serial.print(tests[i].final.degrees, 4);
    Serial.print(F(","));
    Serial.print(tests[i].errorDegrees, 4);
    Serial.print(F(","));
    Serial.print(tests[i].errorMinutes, 2);
    Serial.print(F(","));
    Serial.print(tests[i].errorSeconds, 2);
    Serial.print(F(","));
    Serial.print(tests[i].errorPerRev, 4);
    Serial.println();
  }

  Serial.println();
}

void printRecommendations(StatisticalAnalysis analysis) {
  Serial.println(F("========================================"));
  Serial.println(F("RECOMENDACIONES FINALES"));
  Serial.println(F("========================================\n"));

  Serial.print(F("Valor actual: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev"));

  if (analysis.isSystematic && abs(analysis.meanError) > TARGET_PRECISION) {
    Serial.print(F("Valor recomendado: "));
    Serial.print(analysis.recommendedSteps);
    Serial.println(F(" pasos/rev"));

    Serial.println(F("\nPROXIMOS PASOS:"));
    Serial.print(F("1. Actualizar STEPS_PER_REVOLUTION = "));
    Serial.println(analysis.recommendedSteps);
    Serial.println(F("2. Volver a ejecutar este sketch"));
    Serial.println(F("3. Repetir hasta error < 0.1 grados"));
  } else {
    Serial.println(F("\nCALIBRACION COMPLETADA!"));
    Serial.println(F("El valor actual es OPTIMO."));
    Serial.println(F("\nPROXIMOS PASOS:"));
    Serial.println(F("1. Actualizar todos los sketches con este valor"));
    Serial.println(F("2. Actualizar CALIBRATION.md"));
    Serial.println(F("3. Ejecutar sketch 09 para comparación"));
  }

  Serial.println();
}

void printHeader() {
  Serial.println(F("\n============================================="));
  Serial.println(F("  SISTEMA DE CALIBRACION AUTOMATICA"));
  Serial.println(F("  Motor 28BYJ-48 + Sensor AS5600"));
  Serial.println(F("=============================================\n"));

  Serial.print(F("Iteracion: "));
  Serial.print(currentIteration + 1);
  Serial.print(F(" / "));
  Serial.println(MAX_ITERATIONS);

  Serial.print(F("Precision objetivo: <"));
  Serial.print(TARGET_PRECISION, 2);
  Serial.println(F(" grados"));

  Serial.println();
}

// ============================================
// Setup y Loop
// ============================================

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configurar pines del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotor();

  printHeader();

  // Inicializar sensor AS5600
  Serial.println(F("Inicializando sensor AS5600..."));
  if (!sensor.begin()) {
    Serial.println(F("ERROR: No se pudo inicializar AS5600"));
    Serial.println(F("Verifica las conexiones I2C"));
    while (1) delay(1000);
  }
  Serial.println(F("OK: Sensor inicializado\n"));

  // Verificar imán
  MagnetStatus magnetStatus = sensor.getMagnetStatus();
  Serial.println(F("Estado del iman:"));
  Serial.print(F("  Detectado: "));
  Serial.println(magnetStatus.detected ? F("SI") : F("NO"));
  Serial.print(F("  Magnitud: "));
  Serial.print(magnetStatus.magnitude);
  Serial.print(F(" ("));
  Serial.print(magnetStatus.optimal ? F("OPTIMO") : F("VERIFICAR"));
  Serial.println(F(")"));

  if (!magnetStatus.detected) {
    Serial.println(F("\nADVERTENCIA: Iman no detectado"));
    Serial.println(F("Verifica la posicion del iman (0.5-3mm)"));
    delay(3000);
  }

  Serial.println();
  delay(2000);

  // Ejecutar calibración automática
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
    Serial.print(F(" / "));
    Serial.println(MAX_ITERATIONS);
    Serial.println(F("###########################################\n"));

    // Ejecutar batería de pruebas
    runCalibrationBattery();

    // Analizar resultados
    StatisticalAnalysis analysis = analyzeResults();

    // Mostrar resultados
    printTestSummary();
    printStatisticalAnalysis(analysis);
    printDataLog();
    printRecommendations(analysis);

    // Verificar si alcanzamos la precisión objetivo
    if (analysis.absError < TARGET_PRECISION) {
      Serial.println(F("\n*** CALIBRACION COMPLETADA EXITOSAMENTE ***"));
      Serial.print(F("Precision alcanzada: "));
      Serial.print(analysis.absError, 4);
      Serial.println(F(" grados"));
      break;
    }

    // Aplicar ajuste automático si hay error sistemático
    if (analysis.isSystematic && currentIteration < MAX_ITERATIONS - 1) {
      Serial.println(F("\n*** APLICANDO AJUSTE AUTOMATICO ***"));
      Serial.print(F("Cambiando de "));
      Serial.print(STEPS_PER_REVOLUTION);
      Serial.print(F(" a "));
      Serial.print(analysis.recommendedSteps);
      Serial.println(F(" pasos/rev"));

      STEPS_PER_REVOLUTION = analysis.recommendedSteps;

      Serial.println(F("\nEsperando 5 segundos antes de la siguiente iteracion..."));
      delay(5000);
    } else {
      break;
    }
  }

  Serial.println(F("\n============================================="));
  Serial.println(F("  CALIBRACION AUTOMATICA FINALIZADA"));
  Serial.println(F("=============================================\n"));

  Serial.print(F("Valor final calibrado: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev"));
  Serial.println();
}

/*
 * GUIA DE USO:
 *
 * 1. REQUISITOS:
 *    - Sensor AS5600 instalado y verificado (ejecutar sketch 08 primero)
 *    - Imán montado en el eje del motor
 *    - Conexiones I2C correctas
 *
 * 2. PROCESO AUTOMATICO:
 *    - El sketch ejecuta 5 pruebas (1, 5, 10, 20, 50 revoluciones)
 *    - Mide el error con precisión de 0.0879°
 *    - Calcula estadísticas completas
 *    - Determina si el error es sistemático
 *    - Ajusta STEPS_PER_REVOLUTION automáticamente
 *    - Repite hasta alcanzar precisión <0.1°
 *
 * 3. INTERPRETACION DE RESULTADOS:
 *    Error positivo (+) = Motor gira DE MÁS → Reducir pasos
 *    Error negativo (-) = Motor gira DE MENOS → Aumentar pasos
 *
 * 4. REGISTRO DE DATOS:
 *    - Los datos CSV pueden copiarse para análisis en Excel/Python
 *    - Formato timestamp,revolutions,steps,initial,final,error...
 *
 * 5. SIGUIENTES PASOS:
 *    - Actualizar todos los sketches con el valor final
 *    - Actualizar CALIBRATION.md
 *    - Ejecutar sketch 09 para comparar con calibración manual
 */
