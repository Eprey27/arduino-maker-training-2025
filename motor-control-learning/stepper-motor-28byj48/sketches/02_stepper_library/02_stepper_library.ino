/*
 * Experimento 2: Control con Librería Stepper.h
 *
 * OBJETIVO:
 * - Aprender a usar la librería estándar Stepper de Arduino
 * - Comparar con el control manual del experimento 1
 * - Experimentar con diferentes velocidades de forma sencilla
 *
 * HARDWARE:
 * - Motor 28BYJ-48 (5V)
 * - Driver ULN2003APG
 * - Arduino UNO o ESP32
 *
 * CONEXIONES:
 * Arduino Pin 8  -> Driver IN1
 * Arduino Pin 9  -> Driver IN3
 * Arduino Pin 10 -> Driver IN2
 * Arduino Pin 11 -> Driver IN4
 *
 * NOTA: El orden de los pines es diferente al experimento 1
 *       porque la librería Stepper usa una secuencia específica
 */

#include <Stepper.h>

// Definir el número de pasos por revolución
// Para el 28BYJ-48 con relación de reducción 64:1
// En modo full-step: 32 pasos del motor × 64 reducción = 2048 pasos
const int STEPS_PER_REVOLUTION = 2048;

// Inicializar la librería Stepper en los pines 8 a 11
// El orden de los pines es: IN1, IN3, IN2, IN4
Stepper myStepper(STEPS_PER_REVOLUTION, 8, 10, 9, 11);

void setup() {
  // Inicializar comunicación serial
  Serial.begin(115200);
  Serial.println("=================================");
  Serial.println("Motor Paso a Paso 28BYJ-48");
  Serial.println("Experimento 2: Librería Stepper");
  Serial.println("=================================");
  Serial.println();

  // Establecer velocidad inicial en RPM (revoluciones por minuto)
  // El 28BYJ-48 funciona bien entre 10-15 RPM máximo
  myStepper.setSpeed(10);

  Serial.println("Configuración completada");
  Serial.println("Velocidad inicial: 10 RPM");
  Serial.println();

  delay(2000);
}

void loop() {
  // Demo 1: Rotación completa hacia adelante a 10 RPM
  Serial.println("--- Demo 1: Rotación completa CW (sentido horario) ---");
  Serial.println("Velocidad: 10 RPM");
  myStepper.setSpeed(10);

  unsigned long startTime = millis();
  myStepper.step(STEPS_PER_REVOLUTION);
  unsigned long endTime = millis();

  Serial.print("Tiempo transcurrido: ");
  Serial.print((endTime - startTime) / 1000.0);
  Serial.println(" segundos");
  Serial.println();

  delay(2000);

  // Demo 2: Rotación completa hacia atrás a 10 RPM
  Serial.println("--- Demo 2: Rotación completa CCW (sentido antihorario) ---");
  Serial.println("Velocidad: 10 RPM");
  myStepper.setSpeed(10);

  startTime = millis();
  myStepper.step(-STEPS_PER_REVOLUTION);  // Número negativo = dirección inversa
  endTime = millis();

  Serial.print("Tiempo transcurrido: ");
  Serial.print((endTime - startTime) / 1000.0);
  Serial.println(" segundos");
  Serial.println();

  delay(2000);

  // Demo 3: Media revolución a 15 RPM
  Serial.println("--- Demo 3: Media revolución (180 grados) CW ---");
  Serial.println("Velocidad: 15 RPM");
  myStepper.setSpeed(15);

  startTime = millis();
  myStepper.step(STEPS_PER_REVOLUTION / 2);
  endTime = millis();

  Serial.print("Tiempo transcurrido: ");
  Serial.print((endTime - startTime) / 1000.0);
  Serial.println(" segundos");
  Serial.println();

  delay(2000);

  // Demo 4: Vuelta a posición original
  Serial.println("--- Demo 4: Vuelta a posición original ---");
  Serial.println("Velocidad: 15 RPM");
  myStepper.setSpeed(15);

  startTime = millis();
  myStepper.step(-STEPS_PER_REVOLUTION / 2);
  endTime = millis();

  Serial.print("Tiempo transcurrido: ");
  Serial.print((endTime - startTime) / 1000.0);
  Serial.println(" segundos");
  Serial.println();

  delay(2000);

  // Demo 5: Cuarto de revolución (90 grados) a velocidad lenta
  Serial.println("--- Demo 5: Cuarto de revolución (90 grados) ---");
  Serial.println("Velocidad: 5 RPM (lenta)");
  myStepper.setSpeed(5);

  startTime = millis();
  myStepper.step(STEPS_PER_REVOLUTION / 4);
  endTime = millis();

  Serial.print("Tiempo transcurrido: ");
  Serial.print((endTime - startTime) / 1000.0);
  Serial.println(" segundos");
  Serial.println();

  delay(2000);

  // Vuelta al inicio
  Serial.println("--- Regresando a posición inicial ---");
  myStepper.setSpeed(10);
  myStepper.step(-STEPS_PER_REVOLUTION / 4);
  Serial.println();

  // Pausa antes de repetir el ciclo
  Serial.println("Pausa de 5 segundos antes de repetir...");
  Serial.println("=====================================");
  Serial.println();
  delay(5000);
}

/*
 * VENTAJAS de usar la librería Stepper:
 * - Código más simple y legible
 * - No necesitas implementar la secuencia de pasos manualmente
 * - Control de velocidad en RPM (más intuitivo)
 * - Dirección fácil de controlar (valores positivos/negativos)
 *
 * DESVENTAJAS:
 * - Menos control granular sobre la secuencia
 * - La función step() es bloqueante (el programa se detiene hasta completar)
 * - Difícil de integrar con otras tareas simultáneas
 *
 * EJERCICIOS PARA EXPERIMENTAR:
 *
 * 1. Cambia las velocidades:
 *    - Prueba con 20 RPM, 30 RPM
 *    - ¿A qué velocidad empieza a perder pasos el motor?
 *    - ¿A qué velocidad es demasiado lento?
 *
 * 2. Crea nuevos movimientos:
 *    - 3 revoluciones completas seguidas
 *    - Movimiento de vaivén (adelante-atrás repetidamente)
 *    - Incrementos de 45 grados (STEPS_PER_REVOLUTION / 8)
 *
 * 3. Mide el tiempo real:
 *    - Compara el tiempo medido con el tiempo teórico
 *    - A 10 RPM, una revolución debería tomar 6 segundos
 *    - ¿Coincide con tu medición?
 *
 * 4. Precisión de posición:
 *    - Marca la posición inicial del eje del motor
 *    - Ejecuta 10 revoluciones completas
 *    - ¿El motor regresa exactamente a la misma posición?
 *
 * 5. Prueba el holding torque:
 *    - El motor debería mantener su posición incluso cuando está detenido
 *    - Intenta girar el eje manualmente cuando está parado
 *    - ¿Sientes resistencia?
 */
