/*
 * Experimento 1: Control Básico del Motor Paso a Paso 28BYJ-48
 *
 * OBJETIVO:
 * - Entender cómo funciona la secuencia de pasos manual
 * - Observar los LEDs del driver ULN2003 mientras el motor gira
 * - Verificar que el motor y el driver funcionan correctamente
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
 * Arduino 5V     -> Driver +
 * Arduino GND    -> Driver -
 *
 * IMPORTANTE: Verifica que el jumper del driver esté en 5V (no en 12V)
 */

// Definición de pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Secuencia Half-Step (8 pasos por ciclo completo)
// Esta secuencia proporciona mayor suavidad y precisión
const int halfStepSequence[8][4] = {
  {1, 0, 0, 0},  // Paso 0: Solo bobina 1
  {1, 1, 0, 0},  // Paso 1: Bobinas 1 y 2
  {0, 1, 0, 0},  // Paso 2: Solo bobina 2
  {0, 1, 1, 0},  // Paso 3: Bobinas 2 y 3
  {0, 0, 1, 0},  // Paso 4: Solo bobina 3
  {0, 0, 1, 1},  // Paso 5: Bobinas 3 y 4
  {0, 0, 0, 1},  // Paso 6: Solo bobina 4
  {1, 0, 0, 1}   // Paso 7: Bobinas 4 y 1
};

// Variables globales
int currentStep = 0;          // Paso actual en la secuencia
int delayTime = 2;            // Tiempo entre pasos en milisegundos (ajustable)
int stepsCompleted = 0;       // Contador de pasos ejecutados

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  Serial.println("=================================");
  Serial.println("Motor Paso a Paso 28BYJ-48");
  Serial.println("Experimento 1: Control Básico");
  Serial.println("=================================");
  Serial.println();

  // Configurar pines como salida
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Inicializar todos los pines en LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("Configuración completada");
  Serial.println("Observa los LEDs del driver ULN2003");
  Serial.println("Deberías ver una secuencia de encendido ordenada");
  Serial.println();

  delay(2000);  // Pausa antes de comenzar

  Serial.println("Iniciando rotación...");
  Serial.println();
}

void loop() {
  // Ejecutar 1 revolución completa (4096 pasos en half-step)
  // luego hacer una pausa

  if (stepsCompleted < 4096) {
    // Ejecutar un paso
    executeStep(currentStep);

    // Avanzar al siguiente paso en la secuencia
    currentStep++;
    if (currentStep >= 8) {
      currentStep = 0;
    }

    stepsCompleted++;

    // Mostrar progreso cada 512 pasos (aproximadamente cada 45 grados)
    if (stepsCompleted % 512 == 0) {
      float degrees = (stepsCompleted / 4096.0) * 360.0;
      Serial.print("Progreso: ");
      Serial.print(stepsCompleted);
      Serial.print(" pasos (");
      Serial.print(degrees, 1);
      Serial.println(" grados)");
    }

    // Pequeña pausa entre pasos
    delay(delayTime);

  } else {
    // Revolución completa
    Serial.println();
    Serial.println("¡Revolución completa!");
    Serial.println("Pausa de 3 segundos...");
    Serial.println();

    // Apagar todas las bobinas durante la pausa
    stopMotor();

    delay(3000);

    // Reiniciar contador
    stepsCompleted = 0;
    currentStep = 0;

    Serial.println("Iniciando nueva revolución...");
    Serial.println();
  }
}

// Función para ejecutar un paso específico de la secuencia
void executeStep(int step) {
  digitalWrite(IN1, halfStepSequence[step][0]);
  digitalWrite(IN2, halfStepSequence[step][1]);
  digitalWrite(IN3, halfStepSequence[step][2]);
  digitalWrite(IN4, halfStepSequence[step][3]);
}

// Función para detener el motor (apagar todas las bobinas)
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/*
 * EJERCICIOS PARA EXPERIMENTAR:
 *
 * 1. Cambia el valor de 'delayTime':
 *    - Prueba con 5ms, 10ms, 1ms
 *    - ¿Qué pasa si es muy rápido? (menos de 1ms)
 *    - ¿Qué pasa si es muy lento? (más de 20ms)
 *
 * 2. Observa los LEDs del driver:
 *    - ¿Ves un patrón secuencial?
 *    - ¿Cuántos LEDs se encienden simultáneamente?
 *
 * 3. Modifica el número de pasos:
 *    - Cambia 4096 por 2048 (media revolución)
 *    - Cambia por 1024 (cuarto de revolución)
 *
 * 4. Invierte la dirección:
 *    - En lugar de incrementar currentStep, decrémentalo
 *    - Necesitarás manejar cuando llegue a -1 (ponerlo en 7)
 *
 * 5. Escucha el motor:
 *    - ¿Escuchas el sonido de los pasos?
 *    - ¿Cambia el tono con diferentes velocidades?
 */
