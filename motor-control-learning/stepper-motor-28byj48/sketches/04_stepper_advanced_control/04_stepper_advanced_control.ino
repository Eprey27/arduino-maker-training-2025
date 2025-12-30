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
const int STEPS_PER_REVOLUTION = 4097;  // Valor calibrado empíricamente
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
      Serial.println(F("> Rotacion continua ADELANTE"));
      printSpeed();
      break;

    case 'b':  // Backward
    case 'B':
      isRunning = true;
      direction = -1;
      Serial.println(F("< Rotacion continua ATRAS"));
      printSpeed();
      break;

    case 's':  // Stop
    case 'S':
      isRunning = false;
      stopMotor();
      Serial.println(F("# Motor DETENIDO"));
      printInfo();
      break;

    case '+':  // Aumentar velocidad
      if (delayTime > MIN_DELAY) {
        delayTime--;
        Serial.println(F("+ Velocidad aumentada"));
        printSpeed();
      } else {
        Serial.println(F("! Velocidad maxima alcanzada"));
      }
      break;

    case '-':  // Disminuir velocidad
      if (delayTime < MAX_DELAY) {
        delayTime++;
        Serial.println(F("- Velocidad disminuida"));
        printSpeed();
      } else {
        Serial.println(F("! Velocidad minima alcanzada"));
      }
      break;

    case 'r':  // Una revolución
    case 'R':
      isRunning = false;
      Serial.println(F("@ Ejecutando 1 revolucion completa..."));
      rotateRevolutions(1);
      Serial.println(F("OK Completado"));
      printInfo();
      break;

    case 'h':  // Media revolución
    case 'H':
      isRunning = false;
      Serial.println(F("@ Ejecutando media revolucion (180°)..."));
      rotateAngle(180);
      Serial.println(F("OK Completado"));
      printInfo();
      break;

    case 'q':  // Cuarto de revolución
    case 'Q':
      isRunning = false;
      Serial.println(F("@ Ejecutando cuarto revolucion (90°)..."));
      rotateAngle(90);
      Serial.println(F("OK Completado"));
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
      {
        isRunning = false;
        int revs = cmd - '0';
        Serial.print(F("@ Ejecutando "));
        Serial.print(revs);
        Serial.println(F(" revoluciones..."));
        rotateRevolutions(revs);
        Serial.println(F("OK Completado"));
        printInfo();
      }
      break;

    case 'a':  // Ángulo específico
    case 'A':
      isRunning = false;
      Serial.println(F("Ingresa angulo en grados (0-360):"));
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
      Serial.print(F("! Comando desconocido: "));
      Serial.println(cmd);
      Serial.println(F("Presiona '?' para ver comandos"));
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
  long steps = (long)STEPS_PER_REVOLUTION * revolutions;
  unsigned long startTime = millis();

  for (long i = 0; i < steps; i++) {
    step(1);
    delay(delayTime);

    // Mostrar progreso cada 10%
    long progressInterval = steps / 10;
    if (progressInterval > 0 && i % progressInterval == 0) {
      int progress = (i * 100) / steps;
      Serial.print(progress);
      Serial.println("%");
    }
  }

  unsigned long endTime = millis();
  float timeSeconds = (endTime - startTime) / 1000.0;

  Serial.print(F("Tiempo: "));
  Serial.print(timeSeconds, 2);
  Serial.println(F(" seg"));
}

void rotateAngle(float degrees) {
  long steps = ((long)degrees * STEPS_PER_REVOLUTION) / 360;

  Serial.print(F("Pasos calculados: "));
  Serial.println(steps);

  for (long i = 0; i < steps; i++) {
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
    Serial.print(F("Rotando "));
    Serial.print(angle, 1);
    Serial.println(F(" grados..."));
    rotateAngle(angle);
    Serial.println(F("OK Completado"));
    printInfo();
  } else {
    Serial.println(F("! Angulo invalido. Debe estar 0-360."));
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
  Serial.println(F("==================================="));
  Serial.println(F("  ESTADO ACTUAL"));
  Serial.println(F("==================================="));

  Serial.print(F("  Pasos totales: "));
  Serial.println(totalSteps);

  Serial.print(F("  Posicion: "));
  Serial.print(currentPosition, 2);
  Serial.println(F("°"));

  float revolutions = totalSteps / (float)STEPS_PER_REVOLUTION;
  Serial.print(F("  Revoluciones: "));
  Serial.println(revolutions, 2);

  Serial.print(F("  Velocidad: "));
  float rpm = (1000.0 / delayTime / STEPS_PER_REVOLUTION) * 60.0;
  Serial.print(rpm, 2);
  Serial.println(F(" RPM"));

  Serial.print(F("  Estado: "));
  Serial.println(isRunning ? F("EN MOVIMIENTO") : F("DETENIDO"));

  if (isRunning) {
    Serial.print(F("  Direccion: "));
    Serial.println(direction == 1 ? F("ADELANTE") : F("ATRAS"));
  }

  Serial.println(F("==================================="));
  Serial.println();
}

void printWelcomeMessage() {
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("  Motor Paso a Paso 28BYJ-48"));
  Serial.println(F("  Control Avanzado Interactivo"));
  Serial.println(F("======================================="));
  Serial.println();
  Serial.println(F("COMANDOS DISPONIBLES:"));
  Serial.println(F("---------------------"));
  Serial.println(F("  f/F : Rotacion continua ADELANTE"));
  Serial.println(F("  b/B : Rotacion continua ATRAS"));
  Serial.println(F("  s/S : DETENER motor"));
  Serial.println();
  Serial.println(F("  +   : Aumentar velocidad"));
  Serial.println(F("  -   : Disminuir velocidad"));
  Serial.println();
  Serial.println(F("  r/R : 1 revolucion completa (360°)"));
  Serial.println(F("  h/H : Media revolucion (180°)"));
  Serial.println(F("  q/Q : Cuarto revolucion (90°)"));
  Serial.println(F("  1-9 : N revoluciones completas"));
  Serial.println(F("  a/A : Angulo especifico (ingresar grados)"));
  Serial.println();
  Serial.println(F("  i/I : Mostrar informacion del estado"));
  Serial.println(F("  ?   : Mostrar esta ayuda"));
  Serial.println();
  Serial.println(F("======================================="));
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
