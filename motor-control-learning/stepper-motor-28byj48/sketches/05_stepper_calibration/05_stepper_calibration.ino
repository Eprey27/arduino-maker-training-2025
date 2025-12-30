/*
 * Experimento 5: Sistema de Calibración del Motor 28BYJ-48
 *
 * OBJETIVO:
 * - Medir la precisión real del motor comparada con valores teóricos
 * - Calcular factores de corrección basados en mediciones reales
 * - Generar constantes calibradas para usar en otros sketches
 * - Identificar si hay pérdida de pasos o desviaciones sistemáticas
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
 *
 * MATERIALES NECESARIOS PARA CALIBRACIÓN:
 * - Marcador o cinta adhesiva para marcar posición inicial
 * - Regla o transportador (opcional, para verificar ángulos)
 * - Papel y lápiz para anotar observaciones
 *
 * INSTRUCCIONES:
 * 1. Marca la posición inicial del eje del motor con un marcador
 * 2. Marca también una referencia fija (la base o mesa)
 * 3. Ejecuta el sketch y sigue las instrucciones
 * 4. Responde con precisión a las preguntas sobre posición alcanzada
 * 5. Al final obtendrás el factor de corrección calibrado
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Constantes TEÓRICAS del motor (valores iniciales)
const int STEPS_PER_REV_THEORETICAL = 4096;  // Valor ajustado basado en pruebas
const int DELAY_MS = 2;  // Delay entre pasos

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

// Estructura para almacenar resultados de calibración
struct CalibrationData {
  int targetAngle;           // Ángulo objetivo
  long stepsExecuted;        // Pasos ejecutados
  float angleReported;       // Ángulo reportado por el usuario
  float errorDegrees;        // Error en grados
  float errorPercent;        // Error en porcentaje
};

// Array para almacenar múltiples mediciones
const int MAX_MEASUREMENTS = 10;
CalibrationData measurements[MAX_MEASUREMENTS];
int measurementCount = 0;

// Resultados de calibración
float totalError = 0.0;
float averageError = 0.0;
float correctionFactor = 1.0;
long calibratedStepsPerRev = STEPS_PER_REV_THEORETICAL;

void setup() {
  Serial.begin(115200);

  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();
  delay(2000);

  printWelcomeMessage();
  delay(3000);

  // Ejecutar calibración
  runCalibration();

  // Calcular y mostrar resultados
  calculateCalibration();
  printCalibrationResults();

  // Prueba de verificación con valores calibrados
  Serial.println();
  Serial.println(F("=== VERIFICACION CON VALORES CALIBRADOS ==="));
  delay(2000);
  verifyCalibration();

  Serial.println();
  Serial.println(F("=== CALIBRACION COMPLETADA ==="));
  Serial.println(F("Anota los valores calibrados y usalos en tus sketches."));
}

void loop() {
  stopMotor();
  delay(1000);
}

// ============================================
// Proceso de calibración
// ============================================

void runCalibration() {
  Serial.println(F("=== INICIANDO PROCESO DE CALIBRACION ==="));
  Serial.println();

  // Ángulos de prueba (de menor a mayor para mejor precisión)
  int testAngles[] = {45, 90, 180, 360, 360, 360};  // Repetir 360° 3 veces
  int numTests = 6;

  for (int i = 0; i < numTests; i++) {
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Prueba "));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.println(numTests);
    Serial.println();

    performCalibrationTest(testAngles[i], i);

    // Pausa entre pruebas
    if (i < numTests - 1) {
      Serial.println();
      Serial.println(F("Preparate para la siguiente prueba..."));
      delay(3000);
    }
  }
}

void performCalibrationTest(int targetAngle, int testIndex) {
  // Calcular pasos necesarios (teórico)
  long steps = ((long)targetAngle * STEPS_PER_REV_THEORETICAL) / 360;

  Serial.print(F("Angulo objetivo: "));
  Serial.print(targetAngle);
  Serial.println(F("°"));

  Serial.print(F("Pasos teoricos: "));
  Serial.println(steps);

  Serial.println();
  Serial.println(F("Ejecutando movimiento..."));

  // Ejecutar movimiento
  unsigned long stepsBefore = totalSteps;
  rotateSteps((int)steps, DELAY_MS);
  unsigned long stepsExecuted = totalSteps - stepsBefore;

  Serial.println(F("Movimiento completado."));
  Serial.println();

  // Solicitar feedback del usuario
  Serial.println(F("OBSERVA LA POSICION ACTUAL DEL EJE"));
  Serial.println();
  Serial.println(F("Opciones:"));
  Serial.println(F("  1. Angulo correcto (marca donde esperabas)"));
  Serial.println(F("  2. Se paso de largo (ingresa angulo estimado)"));
  Serial.println(F("  3. Se quedo corto (ingresa angulo estimado)"));
  Serial.println();
  Serial.println(F("Ingresa '1' si es correcto, o el angulo real alcanzado:"));

  // Esperar respuesta
  float angleReported = waitForUserFeedback(targetAngle);

  // Guardar medición
  measurements[measurementCount].targetAngle = targetAngle;
  measurements[measurementCount].stepsExecuted = stepsExecuted;
  measurements[measurementCount].angleReported = angleReported;
  measurements[measurementCount].errorDegrees = angleReported - targetAngle;
  measurements[measurementCount].errorPercent =
    (measurements[measurementCount].errorDegrees / targetAngle) * 100.0;

  measurementCount++;

  // Mostrar resumen de esta medición
  Serial.println();
  Serial.print(F("Objetivo: "));
  Serial.print(targetAngle);
  Serial.print(F("° | Alcanzado: "));
  Serial.print(angleReported, 1);
  Serial.print(F("° | Error: "));
  Serial.print(measurements[measurementCount-1].errorDegrees, 2);
  Serial.println(F("°"));
}

float waitForUserFeedback(int targetAngle) {
  while (!Serial.available()) {
    delay(10);
  }

  float input = Serial.parseFloat();

  // Limpiar buffer
  while (Serial.available()) {
    Serial.read();
  }

  // Si ingresó 1, significa que es correcto
  if (input == 1.0) {
    return (float)targetAngle;
  }

  // Si ingresó 0 o valor fuera de rango, pedir de nuevo
  if (input <= 0 || input > 720) {
    Serial.println(F("Valor invalido. Ingresa el angulo real alcanzado:"));
    return waitForUserFeedback(targetAngle);
  }

  return input;
}

// ============================================
// Cálculos de calibración
// ============================================

void calculateCalibration() {
  Serial.println();
  Serial.println(F("=== CALCULANDO CALIBRACION ==="));
  Serial.println();

  // Calcular error promedio
  float sumError = 0.0;
  float sumPercentError = 0.0;

  for (int i = 0; i < measurementCount; i++) {
    sumError += measurements[i].errorDegrees;
    sumPercentError += measurements[i].errorPercent;
  }

  averageError = sumError / measurementCount;
  float avgPercentError = sumPercentError / measurementCount;

  // Calcular factor de corrección
  // Si hay error sistemático, ajustamos STEPS_PER_REVOLUTION
  // correctionFactor = angleReported / targetAngle
  // Nuevo STEPS_PER_REV = THEORETICAL / correctionFactor

  float sumCorrectionFactors = 0.0;
  for (int i = 0; i < measurementCount; i++) {
    float factor = (float)measurements[i].angleReported / measurements[i].targetAngle;
    sumCorrectionFactors += factor;
  }

  correctionFactor = sumCorrectionFactors / measurementCount;

  // Calcular STEPS_PER_REVOLUTION calibrado
  calibratedStepsPerRev = (long)((float)STEPS_PER_REV_THEORETICAL / correctionFactor);

  Serial.print(F("Error promedio: "));
  Serial.print(averageError, 2);
  Serial.println(F("°"));

  Serial.print(F("Error porcentual promedio: "));
  Serial.print(avgPercentError, 2);
  Serial.println(F("%"));

  Serial.println();
  Serial.print(F("Factor de correccion: "));
  Serial.println(correctionFactor, 6);

  Serial.println();
}

void printCalibrationResults() {
  Serial.println(F("========================================"));
  Serial.println(F("  RESULTADOS DE CALIBRACION"));
  Serial.println(F("========================================"));
  Serial.println();

  // Tabla de mediciones
  Serial.println(F("Mediciones individuales:"));
  Serial.println(F("Objetivo | Alcanzado | Error(°) | Error(%)"));
  Serial.println(F("---------|-----------|----------|----------"));

  for (int i = 0; i < measurementCount; i++) {
    Serial.print(F("  "));
    Serial.print(measurements[i].targetAngle);
    Serial.print(F("°"));
    printSpaces(6 - countDigits(measurements[i].targetAngle));
    Serial.print(F("| "));
    Serial.print(measurements[i].angleReported, 1);
    Serial.print(F("°"));
    printSpaces(7 - countDigits((int)measurements[i].angleReported));
    Serial.print(F("| "));
    printSignedFloat(measurements[i].errorDegrees, 2);
    printSpaces(7 - countDigits((int)abs(measurements[i].errorDegrees)));
    Serial.print(F("| "));
    printSignedFloat(measurements[i].errorPercent, 1);
    Serial.println(F("%"));
  }

  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  VALORES CALIBRADOS"));
  Serial.println(F("========================================"));
  Serial.println();

  Serial.print(F("STEPS_PER_REVOLUTION teorico: "));
  Serial.println(STEPS_PER_REV_THEORETICAL);

  Serial.print(F("STEPS_PER_REVOLUTION calibrado: "));
  Serial.println(calibratedStepsPerRev);

  Serial.println();
  Serial.print(F("Factor de correccion: "));
  Serial.println(correctionFactor, 6);

  Serial.println();
  Serial.println(F("CODIGO PARA USAR EN TUS SKETCHES:"));
  Serial.println(F("----------------------------------------"));
  Serial.print(F("const int STEPS_PER_REVOLUTION = "));
  Serial.print(calibratedStepsPerRev);
  Serial.println(F(";"));
  Serial.println(F("// O alternativamente:"));
  Serial.print(F("const int STEPS_PER_REVOLUTION = "));
  Serial.print(STEPS_PER_REV_THEORETICAL);
  Serial.println(F(";"));
  Serial.print(F("const float CALIBRATION_FACTOR = "));
  Serial.print(correctionFactor, 6);
  Serial.println(F(";"));
  Serial.println(F("// Usar: steps = (angle * STEPS_PER_REVOLUTION * CALIBRATION_FACTOR) / 360;"));
  Serial.println(F("----------------------------------------"));
}

void verifyCalibration() {
  Serial.println(F("Prueba de verificacion: 360 grados con valor calibrado"));
  Serial.println();

  // Usar valor calibrado
  long steps = ((long)360 * calibratedStepsPerRev) / 360;

  Serial.print(F("Pasos calibrados para 360°: "));
  Serial.println(steps);
  Serial.println();
  Serial.println(F("Ejecutando..."));

  rotateSteps((int)steps, DELAY_MS);

  Serial.println();
  Serial.println(F("Verifica si completo exactamente 1 revolucion."));
  Serial.println(F("Si es asi, la calibracion es correcta!"));
}

// ============================================
// Funciones auxiliares de movimiento
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

// ============================================
// Funciones de utilidad para formateo
// ============================================

void printSpaces(int count) {
  for (int i = 0; i < count; i++) {
    Serial.print(F(" "));
  }
}

int countDigits(int number) {
  if (number == 0) return 1;
  int count = 0;
  if (number < 0) {
    count = 1;  // Para el signo negativo
    number = -number;
  }
  while (number > 0) {
    count++;
    number /= 10;
  }
  return count;
}

void printSignedFloat(float value, int decimals) {
  if (value >= 0) {
    Serial.print(F("+"));
  }
  Serial.print(value, decimals);
}

void printWelcomeMessage() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  SISTEMA DE CALIBRACION"));
  Serial.println(F("  Motor 28BYJ-48"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Este sketch te ayudara a calibrar tu motor"));
  Serial.println(F("para obtener maxima precision."));
  Serial.println();
  Serial.println(F("PREPARACION:"));
  Serial.println(F("1. Marca la posicion inicial del eje"));
  Serial.println(F("2. Marca una referencia fija"));
  Serial.println(F("3. Ten papel y lapiz listos"));
  Serial.println();
  Serial.println(F("Durante las pruebas, necesitaras"));
  Serial.println(F("reportar el angulo REAL alcanzado."));
  Serial.println();
  Serial.println(F("Si no tienes transportador, estima:"));
  Serial.println(F("- 45° = 1/8 de vuelta"));
  Serial.println(F("- 90° = 1/4 de vuelta"));
  Serial.println(F("- 180° = 1/2 vuelta"));
  Serial.println(F("- 360° = 1 vuelta completa"));
  Serial.println();
  Serial.println(F("Iniciando en 3 segundos..."));
  Serial.println(F("========================================"));
}

/*
 * INTERPRETACION DE RESULTADOS:
 *
 * Error positivo (+):
 * - El motor giro MAS de lo esperado
 * - El valor STEPS_PER_REVOLUTION es demasiado alto
 * - Necesitas REDUCIR el valor calibrado
 *
 * Error negativo (-):
 * - El motor giro MENOS de lo esperado
 * - El valor STEPS_PER_REVOLUTION es demasiado bajo
 * - Necesitas AUMENTAR el valor calibrado
 *
 * Causas comunes de error:
 * - Friccion en el eje
 * - Voltaje inconsistente
 * - Delay demasiado rapido (perdida de pasos)
 * - Carga mecanica en el eje
 * - Variaciones en el motor especifico
 *
 * RECOMENDACIONES:
 * - Si error < 2%, la calibracion es excelente
 * - Si error 2-5%, la calibracion es buena
 * - Si error > 5%, verifica conexiones y carga mecanica
 * - Repite la calibracion varias veces para confirmar
 */
