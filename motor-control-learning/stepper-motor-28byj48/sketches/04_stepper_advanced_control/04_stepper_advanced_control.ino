/*
 * Experimento 4: Control Avanzado del Motor Paso a Paso
 *
 * OBJETIVO:
 * - Control interactivo mediante Serial
 * - Cambio de dirección en tiempo real
 * - Velocidad variable ajustable
 * - Aceleración y desaceleración suaves
 * - Movimientos precisos por ángulo o pasos
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
 * COMANDOS SERIAL:
 * - 'f' : Adelante (Forward)
 * - 'b' : Atrás (Backward)
 * - 's' : Detener (Stop)
 * - '+' : Aumentar velocidad
 * - '-' : Disminuir velocidad
 * - 'r' : Una revolución completa
 * - 'h' : Media revolución (180°)
 * - 'q' : Cuarto de revolución (90°)
 * - '1'-'9' : Rotar N revoluciones
 * - 'a' : Rotar ángulo específico (ingresar grados)
 * - 'i' : Información del estado actual
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Constantes del motor
const int STEPS_PER_REVOLUTION = 2048;
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION;

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
int delayTime = 2;              // Delay entre pasos en ms
bool isRunning = false;         // Motor en movimiento continuo
int direction = 1;              // 1 = adelante, -1 = atrás
unsigned long totalSteps = 0;   // Contador total de pasos
float currentPosition = 0.0;    // Posición actual en grados

// Límites de velocidad
const int MIN_DELAY = 1;        // Máxima velocidad (1ms)
const int MAX_DELAY = 20;       // Mínima velocidad (20ms)

void setup() {
  Serial.begin(115200);

  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();

  // Mensaje de bienvenida
  printWelcomeMessage();
}

void loop() {
  // Procesar comandos seriales
  if (Serial.available() > 0) {
    char command = Serial.read();
    processCommand(command);
  }

  // Si el motor está en modo continuo, ejecutar pasos
  if (isRunning) {
    step(1);
    delay(delayTime);
  }
}

// ============================================
// Procesamiento de comandos
// ============================================
void processCommand(char cmd) {
  switch (cmd) {
    case 'f':  // Forward
    case 'F':
      isRunning = true;
      direction = 1;
      Serial.println("→ Rotación continua ADELANTE");
      printSpeed();
      break;

    case 'b':  // Backward
    case 'B':
      isRunning = true;
      direction = -1;
      Serial.println("← Rotación continua ATRÁS");
      printSpeed();
      break;

    case 's':  // Stop
    case 'S':
      isRunning = false;
      stopMotor();
      Serial.println("■ Motor DETENIDO");
      printInfo();
      break;

    case '+':  // Aumentar velocidad
      if (delayTime > MIN_DELAY) {
        delayTime--;
        Serial.println("▲ Velocidad aumentada");
        printSpeed();
      } else {
        Serial.println("⚠ Velocidad máxima alcanzada");
      }
      break;

    case '-':  // Disminuir velocidad
      if (delayTime < MAX_DELAY) {
        delayTime++;
        Serial.println("▼ Velocidad disminuida");
        printSpeed();
      } else {
        Serial.println("⚠ Velocidad mínima alcanzada");
      }
      break;

    case 'r':  // Una revolución
    case 'R':
      isRunning = false;
      Serial.println("↻ Ejecutando 1 revolución completa...");
      rotateRevolutions(1);
      Serial.println("✓ Completado");
      printInfo();
      break;

    case 'h':  // Media revolución
    case 'H':
      isRunning = false;
      Serial.println("⟳ Ejecutando media revolución (180°)...");
      rotateAngle(180);
      Serial.println("✓ Completado");
      printInfo();
      break;

    case 'q':  // Cuarto de revolución
    case 'Q':
      isRunning = false;
      Serial.println("⟲ Ejecutando cuarto de revolución (90°)...");
      rotateAngle(90);
      Serial.println("✓ Completado");
      printInfo();
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      isRunning = false;
      int revs = cmd - '0';
      Serial.print("↻ Ejecutando ");
      Serial.print(revs);
      Serial.println(" revoluciones...");
      rotateRevolutions(revs);
      Serial.println("✓ Completado");
      printInfo();
      break;

    case 'a':  // Ángulo específico
    case 'A':
      isRunning = false;
      Serial.println("Ingresa el ángulo en grados (0-360):");
      waitForAngleInput();
      break;

    case 'i':  // Información
    case 'I':
      printInfo();
      break;

    case '?':  // Ayuda
      printWelcomeMessage();
      break;

    case '\n':
    case '\r':
      // Ignorar caracteres de nueva línea
      break;

    default:
      Serial.print("⚠ Comando desconocido: ");
      Serial.println(cmd);
      Serial.println("Presiona '?' para ver comandos disponibles");
      break;
  }
}

// ============================================
// Funciones de movimiento
// ============================================

void step(int steps) {
  for (int i = 0; i < abs(steps); i++) {
    executeStep(currentStep);

    currentStep += (steps > 0 ? direction : -direction);

    if (currentStep >= 8) currentStep = 0;
    if (currentStep < 0) currentStep = 7;

    totalSteps++;
    currentPosition += DEGREES_PER_STEP * direction;

    // Normalizar posición a 0-360 grados
    while (currentPosition >= 360.0) currentPosition -= 360.0;
    while (currentPosition < 0.0) currentPosition += 360.0;
  }
}

void rotateRevolutions(int revolutions) {
  int steps = STEPS_PER_REVOLUTION * revolutions;
  unsigned long startTime = millis();

  for (int i = 0; i < steps; i++) {
    step(1);
    delay(delayTime);

    // Mostrar progreso cada 10%
    if (i % (steps / 10) == 0) {
      int progress = (i * 100) / steps;
      Serial.print(progress);
      Serial.println("%");
    }
  }

  unsigned long endTime = millis();
  float timeSeconds = (endTime - startTime) / 1000.0;

  Serial.print("Tiempo: ");
  Serial.print(timeSeconds, 2);
  Serial.println(" segundos");
}

void rotateAngle(float degrees) {
  int steps = (degrees * STEPS_PER_REVOLUTION) / 360.0;

  Serial.print("Pasos calculados: ");
  Serial.println(steps);

  for (int i = 0; i < steps; i++) {
    step(1);
    delay(delayTime);
  }
}

void waitForAngleInput() {
  while (!Serial.available()) {
    delay(10);
  }

  float angle = Serial.parseFloat();

  if (angle >= 0 && angle <= 360) {
    Serial.print("Rotando ");
    Serial.print(angle, 1);
    Serial.println(" grados...");
    rotateAngle(angle);
    Serial.println("✓ Completado");
    printInfo();
  } else {
    Serial.println("⚠ Ángulo inválido. Debe estar entre 0 y 360.");
  }

  // Limpiar buffer serial
  while (Serial.available()) {
    Serial.read();
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
// Funciones de información
// ============================================

void printSpeed() {
  float stepsPerSecond = 1000.0 / delayTime;
  float rpm = (stepsPerSecond / STEPS_PER_REVOLUTION) * 60.0;

  Serial.print("  Delay: ");
  Serial.print(delayTime);
  Serial.print(" ms | Velocidad: ");
  Serial.print(rpm, 2);
  Serial.println(" RPM");
}

void printInfo() {
  Serial.println();
  Serial.println("┌─────────────────────────────────┐");
  Serial.println("│  ESTADO ACTUAL                  │");
  Serial.println("├─────────────────────────────────┤");

  Serial.print("│  Pasos totales: ");
  Serial.println(totalSteps);

  Serial.print("│  Posición: ");
  Serial.print(currentPosition, 2);
  Serial.println("°");

  float revolutions = totalSteps / (float)STEPS_PER_REVOLUTION;
  Serial.print("│  Revoluciones: ");
  Serial.println(revolutions, 2);

  Serial.print("│  Velocidad: ");
  float rpm = (1000.0 / delayTime / STEPS_PER_REVOLUTION) * 60.0;
  Serial.print(rpm, 2);
  Serial.println(" RPM");

  Serial.print("│  Estado: ");
  Serial.println(isRunning ? "EN MOVIMIENTO" : "DETENIDO");

  if (isRunning) {
    Serial.print("│  Dirección: ");
    Serial.println(direction == 1 ? "ADELANTE" : "ATRÁS");
  }

  Serial.println("└─────────────────────────────────┘");
  Serial.println();
}

void printWelcomeMessage() {
  Serial.println();
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.println("║  Motor Paso a Paso 28BYJ-48          ║");
  Serial.println("║  Control Avanzado Interactivo        ║");
  Serial.println("╚═══════════════════════════════════════╝");
  Serial.println();
  Serial.println("COMANDOS DISPONIBLES:");
  Serial.println("─────────────────────");
  Serial.println("  f/F : Rotación continua ADELANTE");
  Serial.println("  b/B : Rotación continua ATRÁS");
  Serial.println("  s/S : DETENER motor");
  Serial.println();
  Serial.println("  +   : Aumentar velocidad");
  Serial.println("  -   : Disminuir velocidad");
  Serial.println();
  Serial.println("  r/R : 1 revolución completa (360°)");
  Serial.println("  h/H : Media revolución (180°)");
  Serial.println("  q/Q : Cuarto revolución (90°)");
  Serial.println("  1-9 : N revoluciones completas");
  Serial.println("  a/A : Ángulo específico (ingresar grados)");
  Serial.println();
  Serial.println("  i/I : Mostrar información del estado");
  Serial.println("  ?   : Mostrar esta ayuda");
  Serial.println();
  Serial.println("═══════════════════════════════════════");
  Serial.println();
  printSpeed();
  Serial.println();
}

/*
 * EJERCICIOS AVANZADOS:
 *
 * 1. Aceleración suave:
 *    - Modifica rotateRevolutions() para que aumente gradualmente la velocidad
 *    - Empieza lento y acelera hasta la velocidad configurada
 *
 * 2. Posicionamiento absoluto:
 *    - Crea un comando para ir a una posición absoluta en grados
 *    - El motor debería tomar el camino más corto
 *
 * 3. Secuencias programadas:
 *    - Crea una función que ejecute una secuencia predefinida
 *    - Ejemplo: 90° CW, pausa, 180° CCW, pausa, 90° CW
 *
 * 4. Límites de movimiento:
 *    - Implementa límites de posición (ej: -180° a +180°)
 *    - El motor no debe pasar de estos límites
 *
 * 5. Modo jogging:
 *    - Mantén presionada una tecla para movimiento continuo
 *    - Al soltar, el motor se detiene suavemente
 */
