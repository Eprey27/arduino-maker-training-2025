/*
 * Experimento 10: Verificación del Photo Interrupter (IR Switch)
 *
 * OBJETIVO:
 * - Verificar funcionamiento del sensor IR photo interrupter
 * - Detectar ranura/marca en disco acoplado al eje del motor
 * - Contar revoluciones con precisión
 * - Medir timing y validar detección
 *
 * HARDWARE REQUERIDO:
 * - Arduino UNO o ESP32
 * - Photo Interrupter (IR Switch) del kit Keyestudio
 * - Disco codificador con ranura(s) montado en el eje del motor
 * - Motor 28BYJ-48 + Driver ULN2003 (para pruebas)
 *
 * SENSOR PHOTO INTERRUPTER:
 * - Tipo: Sensor infrarrojo de horquilla (fork-type)
 * - Componentes: LED IR emisor + Fototransistor receptor
 * - Salida: Digital (HIGH cuando bloqueado, LOW cuando libre)
 * - Voltaje: 3.3V - 5V
 * - Distancia gap: Típicamente 5-10mm
 *
 * CONEXIONES:
 * Photo Interrupter:
 *   VCC  -> Arduino 5V
 *   GND  -> Arduino GND
 *   OUT  -> Arduino Pin 2 (interrupción)
 *
 * Motor (opcional para pruebas):
 *   IN1  -> Arduino Pin 8
 *   IN2  -> Arduino Pin 9
 *   IN3  -> Arduino Pin 10
 *   IN4  -> Arduino Pin 11
 *
 * DISCO CODIFICADOR:
 * - Disco de cartón/plástico con 1 ranura
 * - Diámetro: ajustado al sensor (típicamente 20-40mm)
 * - Ranura: 3-5mm de ancho
 * - Montado en el eje del motor
 */

// ============================================
// Configuración de Pines
// ============================================

#define IR_SENSOR_PIN 2  // Pin con interrupción (INT0 en Arduino UNO)

// Pines del motor (opcional)
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Configuración del sensor
const int PULSES_PER_REVOLUTION = 36;  // El sensor genera 36 pulsos por vuelta
const float DEGREES_PER_PULSE = 360.0 / PULSES_PER_REVOLUTION;  // = 10° por pulso

// ============================================
// Variables Globales
// ============================================

volatile long revolutionCount = 0;     // Contador de revoluciones (interrupt)
volatile unsigned long lastTriggerTime = 0;  // Último trigger del sensor
volatile unsigned long triggerInterval = 0;  // Intervalo entre triggers

unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 500;  // Actualizar cada 500ms

bool motorEnabled = false;
int currentStep = 0;
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

// Estadísticas
struct Statistics {
  long totalRevolutions;
  unsigned long minInterval;
  unsigned long maxInterval;
  unsigned long avgInterval;
  float rpm;
};

Statistics stats = {0, 999999, 0, 0, 0.0};

// ============================================
// Función de Interrupción
// ============================================

void onSensorTrigger() {
  unsigned long currentTime = micros();

  // Calcular intervalo desde el último trigger
  if (lastTriggerTime > 0) {
    triggerInterval = currentTime - lastTriggerTime;

    // Filtrar rebotes (debounce): ignorar triggers < 10ms
    if (triggerInterval > 10000) {  // 10ms en microsegundos
      revolutionCount++;

      // Actualizar estadísticas
      if (triggerInterval < stats.minInterval) stats.minInterval = triggerInterval;
      if (triggerInterval > stats.maxInterval) stats.maxInterval = triggerInterval;
    }
  }

  lastTriggerTime = currentTime;
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

void rotateOneStep() {
  executeStep(currentStep);
  currentStep++;
  if (currentStep >= 8) currentStep = 0;
  delay(DELAY_MS);
}

// ============================================
// Funciones de Análisis
// ============================================

void calculateStatistics() {
  stats.totalRevolutions = revolutionCount;

  if (triggerInterval > 0) {
    // Calcular RPM basado en el último intervalo
    // RPM = 60,000,000 / intervalo_en_microsegundos
    stats.rpm = 60000000.0 / (float)triggerInterval;
    stats.avgInterval = triggerInterval;
  } else {
    stats.rpm = 0;
  }
}

void printStatistics() {
  Serial.println(F("\n=== ESTADISTICAS DEL SENSOR ==="));

  Serial.print(F("Revoluciones detectadas: "));
  Serial.println(stats.totalRevolutions);

  Serial.print(F("RPM actual: "));
  Serial.print(stats.rpm, 2);
  Serial.println(F(" rev/min"));

  if (stats.minInterval < 999999) {
    Serial.print(F("Intervalo minimo: "));
    Serial.print(stats.minInterval / 1000.0, 2);
    Serial.println(F(" ms"));
  }

  if (stats.maxInterval > 0) {
    Serial.print(F("Intervalo maximo: "));
    Serial.print(stats.maxInterval / 1000.0, 2);
    Serial.println(F(" ms"));
  }

  Serial.print(F("Ultimo intervalo: "));
  Serial.print(triggerInterval / 1000.0, 2);
  Serial.println(F(" ms"));

  Serial.println();
}

void printSensorStatus() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);

  Serial.print(F("Estado sensor: "));
  if (sensorValue == HIGH) {
    Serial.println(F("BLOQUEADO (disco en ranura)"));
  } else {
    Serial.println(F("LIBRE (sin obstruccion)"));
  }
}

void printRealTimeData() {
  Serial.print(F("Rev: "));
  Serial.print(revolutionCount);
  Serial.print(F("\t"));

  Serial.print(F("RPM: "));
  Serial.print(stats.rpm, 2);
  Serial.print(F("\t"));

  Serial.print(F("Intervalo: "));
  Serial.print(triggerInterval / 1000.0, 2);
  Serial.print(F("ms\t"));

  printSensorStatus();
}

// ============================================
// Pruebas Automáticas
// ============================================

void testStaticDetection() {
  Serial.println(F("\n========================================"));
  Serial.println(F("PRUEBA 1: DETECCION ESTATICA"));
  Serial.println(F("========================================\n"));

  Serial.println(F("Gira manualmente el eje del motor lentamente."));
  Serial.println(F("Observa los cambios en el estado del sensor.\n"));

  unsigned long startTime = millis();
  int lastState = digitalRead(IR_SENSOR_PIN);
  int toggleCount = 0;

  while (millis() - startTime < 10000) {  // 10 segundos
    int currentState = digitalRead(IR_SENSOR_PIN);

    if (currentState != lastState) {
      toggleCount++;
      Serial.print(F("["));
      Serial.print(millis());
      Serial.print(F("ms] Cambio detectado: "));
      Serial.println(currentState == HIGH ? F("BLOQUEADO") : F("LIBRE"));
      lastState = currentState;
      delay(50);  // Debounce
    }

    delay(10);
  }

  Serial.print(F("\nCambios de estado detectados: "));
  Serial.println(toggleCount);

  if (toggleCount == 0) {
    Serial.println(F("ADVERTENCIA: No se detectaron cambios"));
    Serial.println(F("  - Verifica el disco codificador"));
    Serial.println(F("  - Verifica la alineacion del sensor"));
  } else if (toggleCount >= 2) {
    Serial.println(F("OK: Sensor funcionando correctamente"));
  }

  Serial.println();
}

void testRevolutionCounting() {
  Serial.println(F("\n========================================"));
  Serial.println(F("PRUEBA 2: CONTEO DE REVOLUCIONES"));
  Serial.println(F("========================================\n"));

  Serial.println(F("Gira el eje manualmente completando 10 revoluciones."));
  Serial.println(F("Observa si el contador incrementa correctamente.\n"));

  // Resetear contador
  noInterrupts();
  revolutionCount = 0;
  interrupts();

  Serial.println(F("Contador reseteado. Comienza a girar..."));
  Serial.println(F("Presiona RESET cuando termines.\n"));

  long lastCount = 0;

  while (true) {
    if (revolutionCount != lastCount) {
      Serial.print(F("Revolucion #"));
      Serial.print(revolutionCount);
      Serial.print(F(" detectada - Intervalo: "));
      Serial.print(triggerInterval / 1000.0, 2);
      Serial.println(F(" ms"));
      lastCount = revolutionCount;
    }
    delay(100);
  }
}

// ============================================
// Menú Interactivo
// ============================================

void printMenu() {
  Serial.println(F("\n========================================"));
  Serial.println(F("MENU DE PRUEBAS"));
  Serial.println(F("========================================\n"));
  Serial.println(F("1 - Ver estado del sensor en tiempo real"));
  Serial.println(F("2 - Prueba de deteccion estatica"));
  Serial.println(F("3 - Prueba de conteo de revoluciones"));
  Serial.println(F("4 - Ver estadisticas"));
  Serial.println(F("5 - Resetear contador de revoluciones"));
  Serial.println(F("6 - Activar/Desactivar motor"));
  Serial.println(F("m - Mostrar menu nuevamente"));
  Serial.println();
}

void handleSerialCommand() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case '1':
        Serial.println(F("\nModo tiempo real activado (10 segundos)"));
        Serial.println(F("Formato: Rev | RPM | Intervalo | Estado\n"));
        for (int i = 0; i < 20; i++) {
          calculateStatistics();
          printRealTimeData();
          delay(500);
        }
        printMenu();
        break;

      case '2':
        testStaticDetection();
        printMenu();
        break;

      case '3':
        testRevolutionCounting();
        break;

      case '4':
        calculateStatistics();
        printStatistics();
        printMenu();
        break;

      case '5':
        noInterrupts();
        revolutionCount = 0;
        stats.minInterval = 999999;
        stats.maxInterval = 0;
        interrupts();
        Serial.println(F("\nContador reseteado"));
        printMenu();
        break;

      case '6':
        motorEnabled = !motorEnabled;
        if (motorEnabled) {
          Serial.println(F("\nMotor ACTIVADO - girando lentamente..."));
          Serial.println(F("Observa el conteo de revoluciones"));
        } else {
          Serial.println(F("\nMotor DESACTIVADO"));
          stopMotor();
        }
        printMenu();
        break;

      case 'm':
      case 'M':
        printMenu();
        break;

      default:
        Serial.println(F("Comando no reconocido"));
        printMenu();
        break;
    }
  }
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
  // INT0 (pin 2) en Arduino UNO
  // FALLING: trigger cuando pasa de HIGH a LOW (disco entra en ranura)
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), onSensorTrigger, FALLING);

  // Header
  Serial.println(F("\n============================================="));
  Serial.println(F("  VERIFICACION PHOTO INTERRUPTER (IR)"));
  Serial.println(F("  Sistema de Calibracion con Sensor IR"));
  Serial.println(F("=============================================\n"));

  Serial.println(F("Sensor: Photo Interrupter del kit Keyestudio"));
  Serial.println(F("Tipo: Fork-type IR sensor (LED + Phototransistor)"));
  Serial.println(F("Configuracion: 36 pulsos por revolucion"));
  Serial.print(F("Resolucion angular: "));
  Serial.print(DEGREES_PER_PULSE, 1);
  Serial.println(F("° por pulso\n"));

  delay(1000);

  // Verificar estado inicial
  Serial.println(F("=== VERIFICACION INICIAL ===\n"));

  Serial.print(F("Pin del sensor: "));
  Serial.println(IR_SENSOR_PIN);

  printSensorStatus();

  Serial.println(F("\nSi el estado no cambia al pasar el disco:"));
  Serial.println(F("  1. Verifica las conexiones (VCC, GND, OUT)"));
  Serial.println(F("  2. Verifica el disco codificador (ranura clara)"));
  Serial.println(F("  3. Verifica la alineacion del sensor\n"));

  delay(2000);

  printMenu();
}

void loop() {
  // Manejar comandos seriales
  handleSerialCommand();

  // Si el motor está activado, girarlo lentamente
  if (motorEnabled) {
    rotateOneStep();
  }

  // Actualización periódica en modo pasivo
  unsigned long currentTime = millis();
  if (!motorEnabled && (currentTime - lastPrintTime >= PRINT_INTERVAL)) {
    lastPrintTime = currentTime;

    // Solo mostrar actualizaciones si hay actividad
    static long lastRevCount = 0;
    if (revolutionCount != lastRevCount) {
      calculateStatistics();
      printRealTimeData();
      lastRevCount = revolutionCount;
    }
  }
}

/*
 * GUIA DE USO:
 *
 * 1. MONTAJE DEL DISCO CODIFICADOR:
 *    - Cortar disco de cartón (20-40mm diámetro)
 *    - Hacer 1 ranura de 3-5mm de ancho
 *    - Pegar disco en el eje del motor (centrado)
 *
 * 2. POSICIONAMIENTO DEL SENSOR:
 *    - Colocar el photo interrupter de forma que el disco
 *      pase por la ranura/horquilla del sensor
 *    - Asegurar que la ranura del disco alinea con el gap del sensor
 *    - Distancia típica: disco completamente dentro del gap (5-10mm)
 *
 * 3. CONEXIONES:
 *    - VCC -> 5V
 *    - GND -> GND
 *    - OUT -> Pin 2 (con interrupción)
 *
 * 4. PRUEBAS:
 *    a) Cargar este sketch
 *    b) Abrir Serial Monitor (115200 baud)
 *    c) Ejecutar pruebas del menú
 *    d) Girar eje manualmente para verificar detección
 *
 * 5. INTERPRETACION:
 *    - BLOQUEADO (HIGH): Disco obstruye el haz IR
 *    - LIBRE (LOW): Ranura permite paso del IR
 *    - Cada transición BLOQUEADO→LIBRE = 1 revolución
 *
 * 6. CALIBRACION:
 *    - Contar manualmente 10 revoluciones
 *    - Verificar que el contador coincida
 *    - Si no coincide: revisar debounce o múltiples ranuras
 *
 * 7. PRECISION:
 *    - Resolución: 1 revolución completa
 *    - No mide ángulos intermedios (solo cuenta vueltas)
 *    - Para mayor precisión: usar disco con más ranuras
 *
 * 8. SIGUIENTES PASOS:
 *    - Una vez verificado el sensor
 *    - Ejecutar sketch 11_ir_auto_calibration
 *    - Sistema medirá error en revoluciones completas
 */
