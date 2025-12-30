/*
 * Prueba de Verificación: STEPS_PER_REVOLUTION
 *
 * OBJETIVO:
 * - Verificar de forma simple y directa el valor correcto de STEPS_PER_REVOLUTION
 * - Probar diferentes valores comunes: 2048, 4096, 2037.8864
 * - Determinar cuál es el más preciso para tu motor específico
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
 * PREPARACIÓN:
 * 1. Marca claramente la posición inicial del eje con un marcador
 * 2. Marca una línea de referencia en la base (mesa o soporte)
 * 3. Observa si el motor regresa EXACTAMENTE a la misma posición
 *
 * TEORÍA:
 * El motor 28BYJ-48 tiene:
 * - Motor interno: 32 pasos por revolución (en full-step)
 * - Reducción: 64:1 (según fabricante)
 * - En half-step: 32 × 2 = 64 pasos del motor
 * - Pasos totales: 64 × 64 = 4096 pasos por revolución
 *
 * PERO algunos motores tienen reducción 63.68395:1 real
 * - Pasos totales: 64 × 63.68395 = 4075.7728 pasos
 * - En half-step notation: 2037.8864 pasos (si se cuenta diferente)
 *
 * Valores comunes a probar:
 * - 2048: Valor estándar documentado (half-step)
 * - 4096: Valor teórico full calculation (64×64)
 * - 2038: Valor con reducción real medida
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

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
int delayMs = 2;  // Delay entre pasos

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

  // Prueba 1: Valor estándar 2048
  testStepsPerRevolution(2048, "Valor estandar (2048)");

  delay(5000);

  // Prueba 2: Valor teórico 4096
  testStepsPerRevolution(4096, "Valor teorico completo (4096)");

  delay(5000);

  // Prueba 3: Valor con reducción real
  testStepsPerRevolution(2038, "Valor reduccion real (2038)");

  delay(5000);

  // Prueba 4: Prueba manual - permite al usuario especificar
  manualTest();

  Serial.println();
  Serial.println(F("=== PRUEBAS COMPLETADAS ==="));
  Serial.println();
  printConclusions();
}

void loop() {
  stopMotor();
  delay(1000);
}

// ============================================
// Pruebas de verificación
// ============================================

void testStepsPerRevolution(int stepsPerRev, const char* description) {
  Serial.println(F("========================================"));
  Serial.print(F("PRUEBA: "));
  Serial.println(description);
  Serial.println(F("========================================"));
  Serial.println();

  Serial.print(F("Pasos por revolucion: "));
  Serial.println(stepsPerRev);
  Serial.println();

  // Realizar 5 revoluciones completas para amplificar el error
  int revolutions = 5;
  Serial.print(F("Ejecutando "));
  Serial.print(revolutions);
  Serial.println(F(" revoluciones completas..."));
  Serial.println(F("Observa la posicion inicial marcada."));
  Serial.println();

  delay(2000);

  // Ejecutar revoluciones
  unsigned long startTime = millis();

  for (int r = 0; r < revolutions; r++) {
    Serial.print(F("  Revolucion "));
    Serial.print(r + 1);
    Serial.print(F("/"));
    Serial.println(revolutions);

    rotateSteps(stepsPerRev, delayMs);
  }

  unsigned long endTime = millis();
  float totalTime = (endTime - startTime) / 1000.0;

  Serial.println();
  Serial.print(F("Tiempo total: "));
  Serial.print(totalTime, 2);
  Serial.println(F(" seg"));

  Serial.print(F("Tiempo por revolucion: "));
  Serial.print(totalTime / revolutions, 2);
  Serial.println(F(" seg"));

  Serial.println();
  Serial.println(F("PREGUNTA CRITICA:"));
  Serial.println(F("El motor regreso EXACTAMENTE a la posicion inicial?"));
  Serial.println();
  Serial.println(F("  [S] Si, exactamente en la marca"));
  Serial.println(F("  [P] Se paso un poco (giro de mas)"));
  Serial.println(F("  [C] Se quedo corto (giro de menos)"));
  Serial.println();
  Serial.println(F("Ingresa tu respuesta (S/P/C):"));

  // Esperar respuesta
  char response = waitForResponse();

  Serial.println();
  printFeedback(response, stepsPerRev);
  Serial.println();
}

void manualTest() {
  Serial.println(F("========================================"));
  Serial.println(F("PRUEBA MANUAL"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Basandote en los resultados anteriores,"));
  Serial.println(F("puedes probar un valor especifico."));
  Serial.println();
  Serial.println(F("Valores recomendados:"));
  Serial.println(F("  - Si 2048 se paso: prueba 2000-2040"));
  Serial.println(F("  - Si 2048 se quedo corto: prueba 2050-2100"));
  Serial.println(F("  - Si 4096 se paso: prueba 4000-4090"));
  Serial.println();
  Serial.println(F("Ingresa STEPS_PER_REVOLUTION a probar (o 0 para saltar):"));

  // Esperar entrada
  while (!Serial.available()) {
    delay(10);
  }

  int customSteps = Serial.parseInt();

  // Limpiar buffer
  while (Serial.available()) {
    Serial.read();
  }

  if (customSteps > 0 && customSteps < 10000) {
    Serial.println();
    char description[50];
    sprintf(description, "Valor personalizado (%d)", customSteps);
    testStepsPerRevolution(customSteps, description);
  } else {
    Serial.println(F("Prueba manual omitida."));
  }
}

// ============================================
// Funciones auxiliares
// ============================================

void rotateSteps(int steps, int delayTime) {
  for (int i = 0; i < steps; i++) {
    executeStep(currentStep);

    currentStep++;
    if (currentStep >= 8) currentStep = 0;

    delay(delayTime);
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

char waitForResponse() {
  while (!Serial.available()) {
    delay(10);
  }

  char response = Serial.read();

  // Limpiar buffer
  while (Serial.available()) {
    Serial.read();
  }

  // Convertir a mayúscula
  if (response >= 'a' && response <= 'z') {
    response = response - 32;
  }

  return response;
}

void printFeedback(char response, int stepsValue) {
  Serial.print(F("Tu respuesta: "));

  switch (response) {
    case 'S':
      Serial.println(F("EXACTO!"));
      Serial.println();
      Serial.println(F("*** EXCELENTE! ***"));
      Serial.print(F("El valor correcto es: "));
      Serial.println(stepsValue);
      Serial.println(F("Usa este valor en tus sketches."));
      break;

    case 'P':
      Serial.println(F("SE PASO"));
      Serial.println();
      Serial.print(F("El valor "));
      Serial.print(stepsValue);
      Serial.println(F(" es DEMASIADO ALTO."));
      Serial.println(F("Prueba con un valor MENOR."));
      Serial.print(F("Recomendacion: "));
      Serial.println(stepsValue - 20);
      break;

    case 'C':
      Serial.println(F("SE QUEDO CORTO"));
      Serial.println();
      Serial.print(F("El valor "));
      Serial.print(stepsValue);
      Serial.println(F(" es DEMASIADO BAJO."));
      Serial.println(F("Prueba con un valor MAYOR."));
      Serial.print(F("Recomendacion: "));
      Serial.println(stepsValue + 20);
      break;

    default:
      Serial.println(F("RESPUESTA NO RECONOCIDA"));
      Serial.println(F("Usa S (si), P (se paso) o C (se quedo corto)"));
      break;
  }
}

void printHeader() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("  VERIFICACION DE STEPS_PER_REVOLUTION"));
  Serial.println(F("  Motor 28BYJ-48"));
  Serial.println(F("========================================"));
  Serial.println();
  Serial.println(F("Este sketch probara diferentes valores"));
  Serial.println(F("para encontrar el correcto para TU motor."));
  Serial.println();
  Serial.println(F("IMPORTANTE:"));
  Serial.println(F("- Marca CLARAMENTE la posicion inicial"));
  Serial.println(F("- Usa marcador permanente o cinta"));
  Serial.println(F("- Marca en el EJE y en la BASE"));
  Serial.println(F("- Se preciso en tus observaciones"));
  Serial.println();
  Serial.println(F("El motor hara 5 revoluciones completas"));
  Serial.println(F("para amplificar cualquier error."));
  Serial.println();
  Serial.println(F("Iniciando pruebas en 3 segundos..."));
  Serial.println(F("========================================"));
}

void printConclusions() {
  Serial.println(F("CONCLUSIONES:"));
  Serial.println(F("----------------------------------------"));
  Serial.println();
  Serial.println(F("Valores comunes del 28BYJ-48:"));
  Serial.println();
  Serial.println(F("1. STEPS_PER_REVOLUTION = 2048"));
  Serial.println(F("   - Valor mas comun documentado"));
  Serial.println(F("   - Asume reduccion 64:1 exacta"));
  Serial.println(F("   - Funciona para mayoria de motores"));
  Serial.println();
  Serial.println(F("2. STEPS_PER_REVOLUTION = 4096"));
  Serial.println(F("   - Calculo teorico (32 steps × 2 × 64)"));
  Serial.println(F("   - Raramente preciso en practica"));
  Serial.println(F("   - Indica modo full-step si funciona"));
  Serial.println();
  Serial.println(F("3. STEPS_PER_REVOLUTION = 2038"));
  Serial.println(F("   - Reduccion real medida ~63.68:1"));
  Serial.println(F("   - Algunos motores usan este valor"));
  Serial.println(F("   - Mas preciso en ciertas variantes"));
  Serial.println();
  Serial.println(F("RECOMENDACION:"));
  Serial.println(F("Usa el valor que dio resultado EXACTO."));
  Serial.println(F("Si ninguno fue exacto, usa sketch 05"));
  Serial.println(F("de calibracion para ajuste fino."));
  Serial.println();
  Serial.println(F("----------------------------------------"));
}

/*
 * NOTAS TÉCNICAS:
 *
 * POR QUÉ DIFERENTES VALORES:
 *
 * El motor 28BYJ-48 tiene variaciones entre fabricantes:
 *
 * 1. Reducción nominal: 64:1
 *    - Motor: 32 pasos/rev (11.25° por paso)
 *    - Con reducción: 32 × 64 = 2048 pasos en full-step
 *    - En half-step: 2048 pasos también (pero mejor resolución)
 *
 * 2. Reducción real medida: ~63.68395:1
 *    - Debido a tolerancias en engranajes
 *    - 32 × 2 × 63.68395 = 4075.77 pasos en half-step
 *    - O 2037.8864 si se normaliza diferente
 *
 * 3. Confusión Full-step vs Half-step:
 *    - Algunos documentos cuentan 4096 pasos
 *    - Depende de cómo se implementa la secuencia
 *
 * CÓMO DETERMINAR EL TUYO:
 * - Si 5 revoluciones regresan exactamente: CORRECTO
 * - Si se pasa: valor demasiado ALTO (reduce ~10-20)
 * - Si se queda corto: valor demasiado BAJO (aumenta ~10-20)
 * - Error de 1-2° en 5 vueltas = ±4-8 pasos de diferencia
 *
 * DESPUÉS DE ENCONTRAR EL VALOR:
 * - Usa ese valor en todos tus sketches
 * - Documéntalo en tu código
 * - Considera que puede variar ligeramente con la carga
 */
