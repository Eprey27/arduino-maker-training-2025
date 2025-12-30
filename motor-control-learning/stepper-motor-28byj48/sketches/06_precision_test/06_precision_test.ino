/*
 * Experimento 6: Prueba de Precisión y Margen de Error
 *
 * OBJETIVO:
 * - Medir el margen de error real del motor en múltiples revoluciones
 * - Determinar la precisión repetible del sistema
 * - Cuantificar el error acumulativo
 * - Validar el valor calibrado de STEPS_PER_REVOLUTION
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
 * MÉTODO DE PRUEBA:
 * 1. Ejecuta múltiples revoluciones (10, 20, 50)
 * 2. Mide la desviación acumulada
 * 3. Calcula error por revolución
 * 4. Determina si el error es sistemático o aleatorio
 * 5. Proporciona métricas estadísticas
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// VALOR CALIBRADO - Ajusta este valor según tu calibración
const long STEPS_PER_REVOLUTION = 4097;  // Valor calibrado empíricamente
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

// Variables de control
int currentStep = 0;

// Estructura para resultados de prueba
struct PrecisionTest {
  int revolutions;           // Número de revoluciones
  float deviationDegrees;    // Desviación en grados
  float errorPerRev;         // Error por revolución
  float errorPercent;        // Error porcentual
};

const int MAX_TESTS = 10;
PrecisionTest tests[MAX_TESTS];
int testCount = 0;

void setup() {
  Serial.begin(115200);

  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();
  delay(2000);

  printHeader();
  delay(3000);

  // Ejecutar batería de pruebas
  runPrecisionTests();

  // Mostrar análisis estadístico
  printStatisticalAnalysis();

  // Recomendaciones finales
  printRecommendations();
}

void loop() {
  stopMotor();
  delay(1000);
}

// ============================================
// Pruebas de precisión
// ============================================

void runPrecisionTests() {
  Serial.println(F("========================================"));
  Serial.println(F("  INICIANDO PRUEBAS DE PRECISION"));
  Serial.println(F("========================================"));
  Serial.println();

  // Pruebas con diferente número de revoluciones
  int testRevolutions[] = {1, 5, 10, 20, 50};
  int numTests = 5;

  for (int i = 0; i < numTests; i++) {
    Serial.println(F("----------------------------------------"));
    Serial.print(F("PRUEBA "));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.println(numTests);
    Serial.println();

    performPrecisionTest(testRevolutions[i]);

    if (i < numTests - 1) {
      Serial.println();
      Serial.println(F("Proxima prueba en 5 segundos..."));
      Serial.println(F("Verifica la posicion actual antes de continuar."));
      delay(5000);
    }
  }
}

void performPrecisionTest(int revolutions) {
  Serial.print(F("Revoluciones a ejecutar: "));
  Serial.println(revolutions);
  Serial.println();

  // Calcular pasos totales
  long totalSteps = (long)STEPS_PER_REVOLUTION * revolutions;

  Serial.print(F("Pasos totales: "));
  Serial.println(totalSteps);
  Serial.println();

  Serial.println(F("MARCA LA POSICION ACTUAL"));
  Serial.println(F("Presiona ENTER cuando estes listo..."));
  waitForEnter();

  Serial.println();
  Serial.println(F("Ejecutando..."));
  Serial.println();

  // Mostrar progreso
  unsigned long startTime = millis();
  long stepsPerUpdate = totalSteps / 10;
  if (stepsPerUpdate < 100) stepsPerUpdate = 100;

  for (long i = 0; i < totalSteps; i++) {
    executeStep(currentStep);

    currentStep++;
    if (currentStep >= 8) currentStep = 0;

    delay(DELAY_MS);

    // Mostrar progreso cada 10%
    if (i > 0 && i % stepsPerUpdate == 0) {
      int percent = (i * 100) / totalSteps;
      Serial.print(F("Progreso: "));
      Serial.print(percent);
      Serial.println(F("%"));
    }
  }

  unsigned long endTime = millis();
  float timeSeconds = (endTime - startTime) / 1000.0;

  Serial.println();
  Serial.println(F("Movimiento completado."));
  Serial.print(F("Tiempo: "));
  Serial.print(timeSeconds, 2);
  Serial.println(F(" segundos"));
  Serial.println();

  // Solicitar medición del usuario
  Serial.println(F("========================================"));
  Serial.println(F("MIDE LA DESVIACION"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Opciones:"));
  Serial.println(F("1. Regreso EXACTAMENTE a la marca (0°)"));
  Serial.println(F("2. Se paso (ingresa grados positivos, ej: +2.5)"));
  Serial.println(F("3. Se quedo corto (ingresa grados negativos, ej: -1.8)"));
  Serial.println();
  Serial.println(F("Ingresa la desviacion en grados:"));
  Serial.println(F("(0 = perfecto, +N = se paso, -N = se quedo corto)"));

  float deviation = waitForDeviationInput();

  // Calcular métricas
  float errorPerRev = deviation / revolutions;
  float errorPercent = (abs(deviation) / (360.0 * revolutions)) * 100.0;

  // Guardar resultados
  tests[testCount].revolutions = revolutions;
  tests[testCount].deviationDegrees = deviation;
  tests[testCount].errorPerRev = errorPerRev;
  tests[testCount].errorPercent = errorPercent;
  testCount++;

  // Mostrar resumen
  Serial.println();
  Serial.println(F("----------------------------------------"));
  Serial.println(F("RESUMEN DE ESTA PRUEBA:"));
  Serial.println(F("----------------------------------------"));
  Serial.print(F("Revoluciones: "));
  Serial.println(revolutions);
  Serial.print(F("Desviacion total: "));
  printSignedFloat(deviation, 2);
  Serial.println(F("°"));
  Serial.print(F("Error por revolucion: "));
  printSignedFloat(errorPerRev, 4);
  Serial.println(F("°/rev"));
  Serial.print(F("Error porcentual: "));
  Serial.print(errorPercent, 4);
  Serial.println(F("%"));
  Serial.println(F("----------------------------------------"));
}

float waitForDeviationInput() {
  while (!Serial.available()) {
    delay(10);
  }

  float deviation = Serial.parseFloat();

  // Limpiar buffer
  while (Serial.available()) {
    Serial.read();
  }

  // Validar rango razonable (-360 a +360)
  if (deviation < -360.0 || deviation > 360.0) {
    Serial.println(F("Valor fuera de rango. Ingresa entre -360 y +360:"));
    return waitForDeviationInput();
  }

  return deviation;
}

void waitForEnter() {
  while (!Serial.available()) {
    delay(10);
  }

  // Limpiar buffer
  while (Serial.available()) {
    Serial.read();
  }
}

// ============================================
// Análisis estadístico
// ============================================

void printStatisticalAnalysis() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  ANALISIS ESTADISTICO"));
  Serial.println(F("========================================"));
  Serial.println();

  // Tabla de resultados
  Serial.println(F("Tabla de resultados:"));
  Serial.println(F("Rev | Desv(°) | Error/Rev(°) | Error(%)"));
  Serial.println(F("----|---------|--------------|----------"));

  for (int i = 0; i < testCount; i++) {
    Serial.print(F(" "));
    Serial.print(tests[i].revolutions);
    printSpaces(3 - countDigits(tests[i].revolutions));
    Serial.print(F("| "));
    printSignedFloat(tests[i].deviationDegrees, 2);
    printSpaces(6 - countDigits((int)abs(tests[i].deviationDegrees)));
    Serial.print(F("| "));
    printSignedFloat(tests[i].errorPerRev, 4);
    printSpaces(10 - countDigits((int)abs(tests[i].errorPerRev)));
    Serial.print(F("| "));
    Serial.print(tests[i].errorPercent, 4);
    Serial.println(F("%"));
  }

  Serial.println();

  // Calcular estadísticas
  float sumErrorPerRev = 0.0;
  float sumAbsErrorPerRev = 0.0;
  float maxError = tests[0].errorPerRev;
  float minError = tests[0].errorPerRev;

  for (int i = 0; i < testCount; i++) {
    sumErrorPerRev += tests[i].errorPerRev;
    sumAbsErrorPerRev += abs(tests[i].errorPerRev);

    if (tests[i].errorPerRev > maxError) maxError = tests[i].errorPerRev;
    if (tests[i].errorPerRev < minError) minError = tests[i].errorPerRev;
  }

  float avgErrorPerRev = sumErrorPerRev / testCount;
  float avgAbsErrorPerRev = sumAbsErrorPerRev / testCount;

  // Calcular desviación estándar
  float variance = 0.0;
  for (int i = 0; i < testCount; i++) {
    float diff = tests[i].errorPerRev - avgErrorPerRev;
    variance += diff * diff;
  }
  float stdDev = sqrt(variance / testCount);

  // Mostrar estadísticas
  Serial.println(F("ESTADISTICAS:"));
  Serial.println(F("----------------------------------------"));
  Serial.print(F("Error promedio por rev: "));
  printSignedFloat(avgErrorPerRev, 4);
  Serial.println(F("°"));

  Serial.print(F("Error absoluto promedio: "));
  Serial.print(avgAbsErrorPerRev, 4);
  Serial.println(F("°"));

  Serial.print(F("Desviacion estandar: "));
  Serial.print(stdDev, 4);
  Serial.println(F("°"));

  Serial.print(F("Error maximo: "));
  printSignedFloat(maxError, 4);
  Serial.println(F("°"));

  Serial.print(F("Error minimo: "));
  printSignedFloat(minError, 4);
  Serial.println(F("°"));

  Serial.print(F("Rango de error: "));
  Serial.print(maxError - minError, 4);
  Serial.println(F("°"));

  Serial.println();

  // Determinar tipo de error
  Serial.println(F("TIPO DE ERROR:"));
  if (abs(avgErrorPerRev) < 0.01) {
    Serial.println(F("- ERROR ALEATORIO (promedio ~0)"));
    Serial.println(F("  El motor es preciso en promedio."));
    Serial.println(F("  Variaciones son por friccion/temperatura."));
  } else if (avgErrorPerRev > 0) {
    Serial.println(F("- ERROR SISTEMATICO POSITIVO"));
    Serial.println(F("  El motor gira consistentemente DE MAS."));
    Serial.println(F("  STEPS_PER_REVOLUTION es muy ALTO."));
    Serial.print(F("  Recomendacion: reducir a ~"));
    long suggested = STEPS_PER_REVOLUTION - (long)(abs(avgErrorPerRev) * STEPS_PER_REVOLUTION / 360.0);
    Serial.println(suggested);
  } else {
    Serial.println(F("- ERROR SISTEMATICO NEGATIVO"));
    Serial.println(F("  El motor gira consistentemente DE MENOS."));
    Serial.println(F("  STEPS_PER_REVOLUTION es muy BAJO."));
    Serial.print(F("  Recomendacion: aumentar a ~"));
    long suggested = STEPS_PER_REVOLUTION + (long)(abs(avgErrorPerRev) * STEPS_PER_REVOLUTION / 360.0);
    Serial.println(suggested);
  }

  Serial.println();
}

void printRecommendations() {
  Serial.println(F("========================================"));
  Serial.println(F("  RECOMENDACIONES"));
  Serial.println(F("========================================"));
  Serial.println();

  // Calcular precisión general
  float sumAbsError = 0.0;
  for (int i = 0; i < testCount; i++) {
    sumAbsError += abs(tests[i].errorPerRev);
  }
  float avgAbsError = sumAbsError / testCount;

  Serial.print(F("Valor actual: STEPS_PER_REVOLUTION = "));
  Serial.println(STEPS_PER_REVOLUTION);
  Serial.println();

  if (avgAbsError < 0.05) {
    Serial.println(F("*** EXCELENTE PRECISION ***"));
    Serial.println(F("Error < 0.05°/rev"));
    Serial.println(F("Tu calibracion es OPTIMA."));
    Serial.println(F("No se requieren ajustes."));
  } else if (avgAbsError < 0.1) {
    Serial.println(F("*** MUY BUENA PRECISION ***"));
    Serial.println(F("Error < 0.1°/rev"));
    Serial.println(F("Calibracion es muy buena."));
    Serial.println(F("Ajuste opcional para perfeccionamiento."));
  } else if (avgAbsError < 0.5) {
    Serial.println(F("*** BUENA PRECISION ***"));
    Serial.println(F("Error < 0.5°/rev"));
    Serial.println(F("Calibracion aceptable."));
    Serial.println(F("Considera ajuste fino si necesitas mas precision."));
  } else {
    Serial.println(F("*** PRECISION MEJORABLE ***"));
    Serial.println(F("Error > 0.5°/rev"));
    Serial.println(F("Se recomienda ajustar STEPS_PER_REVOLUTION."));
  }

  Serial.println();
  Serial.println(F("FACTORES QUE AFECTAN PRECISION:"));
  Serial.println(F("- Temperatura del motor"));
  Serial.println(F("- Voltaje de alimentacion"));
  Serial.println(F("- Friccion en el eje"));
  Serial.println(F("- Carga mecanica"));
  Serial.println(F("- Velocidad (delay entre pasos)"));
  Serial.println();
}

// ============================================
// Funciones auxiliares
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

void printSpaces(int count) {
  for (int i = 0; i < count; i++) {
    Serial.print(F(" "));
  }
}

int countDigits(int number) {
  if (number == 0) return 1;
  int count = 0;
  if (number < 0) {
    count = 1;
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

void printHeader() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  PRUEBA DE PRECISION Y MARGEN DE ERROR"));
  Serial.println(F("  Motor 28BYJ-48"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.print(F("Valor configurado: "));
  Serial.print(STEPS_PER_REVOLUTION);
  Serial.println(F(" pasos/rev"));
  Serial.println();
  Serial.println(F("Esta prueba medira:"));
  Serial.println(F("- Precision en multiples revoluciones"));
  Serial.println(F("- Error acumulativo"));
  Serial.println(F("- Tipo de error (sistematico/aleatorio)"));
  Serial.println(F("- Recomendaciones de ajuste"));
  Serial.println();
  Serial.println(F("PREPARACION:"));
  Serial.println(F("1. Marca CLARAMENTE la posicion inicial"));
  Serial.println(F("2. Prepara forma de medir desviacion"));
  Serial.println(F("3. Ten transportador o referencia angular"));
  Serial.println(F("4. Anota mediciones para comparacion"));
  Serial.println();
  Serial.println(F("Iniciando en 3 segundos..."));
  Serial.println(F("========================================"));
}

/*
 * INTERPRETACION DE RESULTADOS:
 *
 * ERROR POR REVOLUCION:
 * - < 0.05°/rev : Precision excelente, calibracion optima
 * - 0.05-0.1°/rev : Muy buena precision, ajuste opcional
 * - 0.1-0.5°/rev : Buena precision, aceptable para mayoria de usos
 * - > 0.5°/rev : Mejorable, se recomienda ajuste
 *
 * DESVIACION ESTANDAR:
 * - Baja (< 0.05°) : Error consistente, facil de corregir
 * - Alta (> 0.1°) : Error variable, revisar condiciones mecanicas
 *
 * ERROR SISTEMATICO vs ALEATORIO:
 * - Sistematico: Ajustar STEPS_PER_REVOLUTION
 * - Aleatorio: Mejorar condiciones mecanicas (friccion, voltaje)
 *
 * EJEMPLO DE PRECISION:
 * - Error 0.1°/rev en 100 revoluciones = 10° total
 * - Error 0.01°/rev en 100 revoluciones = 1° total
 *
 * APLICACIONES SEGUN PRECISION:
 * - < 0.05°/rev : Robotica de precision, CNC
 * - < 0.1°/rev : Posicionamiento general, pantografos
 * - < 0.5°/rev : Aplicaciones educativas, demostraciones
 */
