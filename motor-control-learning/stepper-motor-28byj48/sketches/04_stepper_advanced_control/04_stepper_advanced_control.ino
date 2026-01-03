/*
 * Experimento 4 (v2): Control Avanzado del Motor Paso a Paso
 * 28BYJ-48 + ULN2003
 *
 * EXTRA (v2):
 * - Aceleración/desaceleración trapezoidal en movimientos finitos
 * - Stop suave en modo continuo
 * - Ir a ángulo absoluto por camino corto ('g')
 * - Secuencia demo ('x')
 * - Límites opcionales ('l')
 * - Jogging aproximado ('j' adelante, 'k' atrás)
 */

#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Constantes del motor (tu calibración)
const int   STEPS_PER_REVOLUTION = 4097;
const float DEGREES_PER_STEP     = 360.0f / STEPS_PER_REVOLUTION;

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
int  currentStep      = 0;
int  delayTime        = 2;     // objetivo (ms)
int  direction        = 1;     // 1 adelante, -1 atrás
bool isRunning        = false; // continuo (f/b)
bool stopPending      = false; // stop suave en continuo

unsigned long totalSteps = 0;
float currentPosition = 0.0f;  // 0..360

// Límites de velocidad
const int MIN_DELAY = 1;   // más rápido
const int MAX_DELAY = 20;  // más lento

// Movimiento suave (rampas)
const int START_DELAY_FOR_RAMPS = 20;   // delay inicial en rampas (ms)
const int ACCEL_STEPS_DEFAULT   = 600;  // pasos para acelerar (se recorta según movimiento)

// Jogging aproximado
bool jogging = false;
unsigned long lastJogMs = 0;
const unsigned long JOG_TIMEOUT_MS = 250; // si no llegan 'j/k' en este tiempo, frena y para

// Límites (opcional)
bool limitsEnabled = false;
// Rango en grados "signed": [-180..180]. Ajusta a tu caso real si quieres limitar.
const float LIMIT_MIN_SIGNED = -180.0f;
const float LIMIT_MAX_SIGNED =  180.0f;

// Estado interno del continuo (para rampa)
int runDelay = START_DELAY_FOR_RAMPS;

// ============================
// Utilidades
// ============================

static inline float normalize360(float deg) {
  while (deg >= 360.0f) deg -= 360.0f;
  while (deg < 0.0f)    deg += 360.0f;
  return deg;
}

static inline float toSignedDeg(float deg0_360) {
  // 0..360 -> (-180..180]
  if (deg0_360 > 180.0f) deg0_360 -= 360.0f;
  return deg0_360;
}

static inline int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static inline int lerpDelay(int a, int b, float t) {
  // t: 0..1
  float v = a + (b - a) * t;
  if (v < 0) v = 0;
  return (int)(v + 0.5f);
}

// ============================
// Motor low-level
// ============================

void executeStep(int stepIndex) {
  digitalWrite(IN1, halfStepSequence[stepIndex][0]);
  digitalWrite(IN2, halfStepSequence[stepIndex][1]);
  digitalWrite(IN3, halfStepSequence[stepIndex][2]);
  digitalWrite(IN4, halfStepSequence[stepIndex][3]);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

bool canStep(int dir) {
  if (!limitsEnabled) return true;

  float nextPos = normalize360(currentPosition + DEGREES_PER_STEP * dir);
  float nextSigned = toSignedDeg(nextPos);

  return (nextSigned >= LIMIT_MIN_SIGNED && nextSigned <= LIMIT_MAX_SIGNED);
}

bool stepOnce(int dir) {
  if (!canStep(dir)) return false;

  executeStep(currentStep);

  currentStep += dir;
  if (currentStep >= 8) currentStep = 0;
  if (currentStep < 0)  currentStep = 7;

  totalSteps++;
  currentPosition = normalize360(currentPosition + DEGREES_PER_STEP * dir);
  return true;
}

// ============================
// Movimiento con rampas
// ============================

bool moveStepsTrapezoid(long steps, int dir) {
  if (steps <= 0) return true;

  int targetDelay = clampInt(delayTime, MIN_DELAY, MAX_DELAY);
  int startDelay  = max(targetDelay, START_DELAY_FOR_RAMPS);

  long accelSteps = min((long)ACCEL_STEPS_DEFAULT, steps / 2);
  long decelSteps = accelSteps; // simétrico
  long cruiseStart = accelSteps;
  long cruiseEnd   = steps - decelSteps;

  for (long i = 0; i < steps; i++) {
    if (!stepOnce(dir)) {
      Serial.println(F("! LIMITE alcanzado: movimiento abortado"));
      stopMotor();
      return false;
    }

    int d = targetDelay;

    if (accelSteps > 0 && i < cruiseStart) {
      float t = (float)i / (float)accelSteps; // 0..1
      d = lerpDelay(startDelay, targetDelay, t);
    } else if (decelSteps > 0 && i >= cruiseEnd) {
      float t = (float)(i - cruiseEnd) / (float)decelSteps; // 0..1
      d = lerpDelay(targetDelay, startDelay, t);
    }

    delay(d);
  }

  return true;
}

bool rotateAngleSmooth(float degrees) {
  degrees = constrain(degrees, 0.0f, 360.0f);
  long steps = (long)(degrees / DEGREES_PER_STEP + 0.5f);

  Serial.print(F("Pasos calculados: "));
  Serial.println(steps);

  return moveStepsTrapezoid(steps, direction);
}

bool rotateRevolutionsSmooth(int revolutions) {
  if (revolutions <= 0) return true;

  long steps = (long)STEPS_PER_REVOLUTION * revolutions;
  unsigned long startTime = millis();

  long progressInterval = steps / 10;

  for (long i = 0; i < steps; i++) {
    if (!stepOnce(direction)) {
      Serial.println(F("! LIMITE alcanzado: movimiento abortado"));
      stopMotor();
      return false;
    }

    // Delay con rampa trapezoidal (reutilizamos lógica simple)
    int targetDelay = clampInt(delayTime, MIN_DELAY, MAX_DELAY);
    int startDelay  = max(targetDelay, START_DELAY_FOR_RAMPS);

    long accelSteps = min((long)ACCEL_STEPS_DEFAULT, steps / 2);
    long decelSteps = accelSteps;
    long cruiseStart = accelSteps;
    long cruiseEnd   = steps - decelSteps;

    int d = targetDelay;
    if (accelSteps > 0 && i < cruiseStart) {
      float t = (float)i / (float)accelSteps;
      d = lerpDelay(startDelay, targetDelay, t);
    } else if (decelSteps > 0 && i >= cruiseEnd) {
      float t = (float)(i - cruiseEnd) / (float)decelSteps;
      d = lerpDelay(targetDelay, startDelay, t);
    }

    delay(d);

    if (progressInterval > 0 && i % progressInterval == 0) {
      int progress = (int)((i * 100L) / steps);
      Serial.print(progress);
      Serial.println("%");
    }
  }

  unsigned long endTime = millis();
  float timeSeconds = (endTime - startTime) / 1000.0f;
  Serial.print(F("Tiempo: "));
  Serial.print(timeSeconds, 2);
  Serial.println(F(" seg"));

  return true;
}

bool moveToAbsoluteAngle(float targetDeg) {
  targetDeg = normalize360(targetDeg);

  // delta en rango (-180..180]
  float delta = targetDeg - currentPosition;
  while (delta > 180.0f)  delta -= 360.0f;
  while (delta <= -180.0f) delta += 360.0f;

  int dir = (delta >= 0.0f) ? 1 : -1;
  long steps = (long)(fabs(delta) / DEGREES_PER_STEP + 0.5f);

  Serial.print(F("Delta: "));
  Serial.print(delta, 2);
  Serial.print(F("° | Dir: "));
  Serial.print(dir == 1 ? F("ADELANTE") : F("ATRAS"));
  Serial.print(F(" | Pasos: "));
  Serial.println(steps);

  return moveStepsTrapezoid(steps, dir);
}

// ============================
// Serial input helpers
// ============================

float readFloatBlocking() {
  while (!Serial.available()) delay(10);
  float v = Serial.parseFloat();
  while (Serial.available()) Serial.read(); // limpiar buffer
  return v;
}

// ============================
// Info
// ============================

void printSpeed() {
  float stepsPerSecond = 1000.0f / max(1, delayTime);
  float rpm = (stepsPerSecond / STEPS_PER_REVOLUTION) * 60.0f;

  Serial.print(F("  Delay obj: "));
  Serial.print(delayTime);
  Serial.print(F(" ms | Velocidad aprox: "));
  Serial.print(rpm, 2);
  Serial.println(F(" RPM"));
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

  Serial.print(F("  Posicion signed: "));
  Serial.print(toSignedDeg(currentPosition), 2);
  Serial.println(F("°"));

  float revolutions = totalSteps / (float)STEPS_PER_REVOLUTION;
  Serial.print(F("  Revoluciones (acum): "));
  Serial.println(revolutions, 2);

  Serial.print(F("  Estado: "));
  Serial.println(isRunning ? F("EN MOVIMIENTO") : F("DETENIDO"));

  if (isRunning) {
    Serial.print(F("  Direccion: "));
    Serial.println(direction == 1 ? F("ADELANTE") : F("ATRAS"));
  }

  Serial.print(F("  Limites: "));
  Serial.println(limitsEnabled ? F("ACTIVOS") : F("INACTIVOS"));

  Serial.println(F("==================================="));
  Serial.println();
}

void printWelcomeMessage() {
  Serial.println();
  Serial.println(F("======================================="));
  Serial.println(F("  28BYJ-48 Control Avanzado (v2)"));
  Serial.println(F("======================================="));
  Serial.println();
  Serial.println(F("COMANDOS:"));
  Serial.println(F("  f/F : continuo ADELANTE"));
  Serial.println(F("  b/B : continuo ATRAS"));
  Serial.println(F("  s/S : STOP suave"));
  Serial.println(F("  +   : mas velocidad"));
  Serial.println(F("  -   : menos velocidad"));
  Serial.println(F("  r/R : 1 revolucion (360) con rampas"));
  Serial.println(F("  h/H : 180 con rampas"));
  Serial.println(F("  q/Q : 90 con rampas"));
  Serial.println(F("  1-9 : N revoluciones con rampas"));
  Serial.println(F("  a/A : angulo (0-360) con rampas"));
  Serial.println(F("  g/G : ir a angulo ABSOLUTO (0-360) camino corto"));
  Serial.println(F("  x/X : secuencia demo (90 CW, pausa, 180 CCW, pausa, 90 CW)"));
  Serial.println(F("  l/L : toggle limites"));
  Serial.println(F("  j   : jog adelante (enviar repetido)"));
  Serial.println(F("  k   : jog atras (enviar repetido)"));
  Serial.println(F("  i/I : info"));
  Serial.println(F("  ?   : ayuda"));
  Serial.println();
  printSpeed();
  Serial.println();
}

// ============================
// Control continuo
// ============================

void startContinuous(int dir) {
  jogging = false;
  isRunning = true;
  stopPending = false;
  direction = (dir >= 0) ? 1 : -1;

  int targetDelay = clampInt(delayTime, MIN_DELAY, MAX_DELAY);
  runDelay = max(targetDelay, START_DELAY_FOR_RAMPS); // empieza lento y acelera
}

void requestStopSmooth() {
  stopPending = true;
  jogging = false;
}

// ============================
// Commands
// ============================

void processCommand(char cmd) {
  switch (cmd) {
    case 'f':
    case 'F':
      startContinuous(1);
      Serial.println(F("> Continuo ADELANTE"));
      printSpeed();
      break;

    case 'b':
    case 'B':
      startContinuous(-1);
      Serial.println(F("< Continuo ATRAS"));
      printSpeed();
      break;

    case 's':
    case 'S':
      if (isRunning) {
        Serial.println(F("# STOP suave"));
        requestStopSmooth();
      } else {
        stopMotor();
        Serial.println(F("# Motor ya estaba detenido"));
        printInfo();
      }
      break;

    case '+':
      delayTime = clampInt(delayTime - 1, MIN_DELAY, MAX_DELAY);
      Serial.println(F("+ Velocidad aumentada"));
      printSpeed();
      break;

    case '-':
      delayTime = clampInt(delayTime + 1, MIN_DELAY, MAX_DELAY);
      Serial.println(F("- Velocidad disminuida"));
      printSpeed();
      break;

    case 'r':
    case 'R':
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("@ 1 revolucion completa..."));
      rotateRevolutionsSmooth(1);
      Serial.println(F("OK"));
      printInfo();
      break;

    case 'h':
    case 'H':
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("@ 180 grados..."));
      direction = 1; // usa direction actual si quieres, aquí fijo a 1
      rotateAngleSmooth(180.0f);
      Serial.println(F("OK"));
      printInfo();
      break;

    case 'q':
    case 'Q':
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("@ 90 grados..."));
      direction = 1;
      rotateAngleSmooth(90.0f);
      Serial.println(F("OK"));
      printInfo();
      break;

    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': {
      isRunning = false; jogging = false; stopPending = false;
      int revs = cmd - '0';
      Serial.print(F("@ "));
      Serial.print(revs);
      Serial.println(F(" revoluciones..."));
      rotateRevolutionsSmooth(revs);
      Serial.println(F("OK"));
      printInfo();
      break;
    }

    case 'a':
    case 'A': {
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("Ingresa angulo (0-360):"));
      float angle = readFloatBlocking();
      if (angle >= 0.0f && angle <= 360.0f) {
        Serial.print(F("Rotando "));
        Serial.print(angle, 1);
        Serial.println(F(" grados..."));
        rotateAngleSmooth(angle);
        Serial.println(F("OK"));
        printInfo();
      } else {
        Serial.println(F("! Angulo invalido (0-360)"));
      }
      break;
    }

    case 'g':
    case 'G': {
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("Ingresa angulo ABSOLUTO (0-360):"));
      float target = readFloatBlocking();
      if (target >= 0.0f && target <= 360.0f) {
        Serial.print(F("Ir a "));
        Serial.print(target, 1);
        Serial.println(F(" grados (camino corto)..."));
        moveToAbsoluteAngle(target);
        Serial.println(F("OK"));
        printInfo();
      } else {
        Serial.println(F("! Angulo invalido (0-360)"));
      }
      break;
    }

    case 'x':
    case 'X': {
      isRunning = false; jogging = false; stopPending = false;
      Serial.println(F("@ Secuencia demo: 90 CW, pausa, 180 CCW, pausa, 90 CW"));
      direction = 1;
      moveStepsTrapezoid((long)(90.0f / DEGREES_PER_STEP + 0.5f),  1);
      delay(400);
      moveStepsTrapezoid((long)(180.0f / DEGREES_PER_STEP + 0.5f), -1);
      delay(400);
      moveStepsTrapezoid((long)(90.0f / DEGREES_PER_STEP + 0.5f),  1);
      Serial.println(F("OK"));
      printInfo();
      break;
    }

    case 'l':
    case 'L':
      limitsEnabled = !limitsEnabled;
      Serial.print(F("# Limites: "));
      Serial.println(limitsEnabled ? F("ACTIVOS") : F("INACTIVOS"));
      printInfo();
      break;

    case 'j': { // jog adelante (hay que reenviar para “mantener”)
      jogging = true;
      isRunning = true;
      stopPending = false;
      direction = 1;
      lastJogMs = millis();
      if (runDelay < delayTime) runDelay = delayTime;
      runDelay = max(runDelay, START_DELAY_FOR_RAMPS);
      Serial.println(F("> Jog ADELANTE (envia 'j' repetido)"));
      break;
    }

    case 'k': { // jog atras (hay que reenviar para “mantener”)
      jogging = true;
      isRunning = true;
      stopPending = false;
      direction = -1;
      lastJogMs = millis();
      if (runDelay < delayTime) runDelay = delayTime;
      runDelay = max(runDelay, START_DELAY_FOR_RAMPS);
      Serial.println(F("< Jog ATRAS (envia 'k' repetido)"));
      break;
    }

    case 'i':
    case 'I':
      printInfo();
      break;

    case '?':
      printWelcomeMessage();
      break;

    case '\n':
    case '\r':
      break;

    default:
      Serial.print(F("! Comando desconocido: "));
      Serial.println(cmd);
      Serial.println(F("Presiona '?'"));
      break;
  }
}

// ============================
// Arduino setup/loop
// ============================

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(15000);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotor();
  printWelcomeMessage();
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    processCommand(command);
  }

  // Si estamos en jogging, si no llegan repetidos, frenamos
  if (jogging && (millis() - lastJogMs) > JOG_TIMEOUT_MS) {
    requestStopSmooth();
  }

  if (isRunning) {
    // step con control de rampa simple en continuo
    if (!stepOnce(direction)) {
      Serial.println(F("! LIMITE alcanzado: motor detenido"));
      isRunning = false;
      jogging = false;
      stopPending = false;
      stopMotor();
      printInfo();
      return;
    }

    delay(runDelay);

    int targetDelay = clampInt(delayTime, MIN_DELAY, MAX_DELAY);
    int startDelay  = max(targetDelay, START_DELAY_FOR_RAMPS);

    if (stopPending) {
      // frenado: subir delay hasta startDelay y parar
      if (runDelay < startDelay) {
        runDelay++;
      } else {
        isRunning = false;
        stopPending = false;
        jogging = false;
        stopMotor();
        Serial.println(F("# Motor DETENIDO (suave)"));
        printInfo();
      }
    } else {
      // aceleración/seguimiento: bajar delay hacia objetivo
      if (runDelay > targetDelay) runDelay--;
      if (runDelay < targetDelay) runDelay++; // por si cambias delayTime a peor en caliente
    }
  }
}
