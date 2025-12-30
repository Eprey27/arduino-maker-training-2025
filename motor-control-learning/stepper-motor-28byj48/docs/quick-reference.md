# Referencia Rápida - Motor 28BYJ-48

## Conexiones

```
Arduino/ESP32          Driver ULN2003
─────────────          ──────────────
Pin 8           ──────→ IN1
Pin 9           ──────→ IN2
Pin 10          ──────→ IN3
Pin 11          ──────→ IN4
5V              ──────→ +  (Positivo)
GND             ──────→ -  (Tierra)

Driver ULN2003         Motor 28BYJ-48
──────────────         ──────────────
Conector 5 pines ─────→ Cable conector motor
```

## Especificaciones Clave

| Parámetro | Valor |
|-----------|-------|
| Pasos por revolución (half-step) | 2048 |
| Pasos por revolución (full-step) | 4096 |
| Ángulo por paso | 0.176° (half-step) |
| Relación de reducción | 1:64 |
| Voltaje nominal | 5V DC |
| Consumo de corriente | ~240mA |
| Velocidad máxima recomendada | 15 RPM |
| Par motor (holding torque) | ~34 N·cm |

## Secuencia Half-Step

```
Paso │ IN1 │ IN2 │ IN3 │ IN4 │
─────┼─────┼─────┼─────┼─────┤
  0  │  1  │  0  │  0  │  0  │
  1  │  1  │  1  │  0  │  0  │
  2  │  0  │  1  │  0  │  0  │
  3  │  0  │  1  │  1  │  0  │
  4  │  0  │  0  │  1  │  0  │
  5  │  0  │  0  │  1  │  1  │
  6  │  0  │  0  │  0  │  1  │
  7  │  1  │  0  │  0  │  1  │
```

## Conversiones Útiles

### Pasos ↔ Grados
```
Grados a Pasos:
pasos = (grados × 2048) / 360

Pasos a Grados:
grados = (pasos × 360) / 2048
```

### Delay ↔ RPM
```
RPM a partir de delay (ms):
RPM = (1000 / delay) / 2048 × 60

Delay a partir de RPM:
delay_ms = 60000 / (RPM × 2048)
```

## Valores de Delay Típicos

| Delay (ms) | RPM Aproximado | Uso Recomendado |
|------------|----------------|-----------------|
| 1          | ~29 RPM        | Máxima velocidad (puede perder pasos) |
| 2          | ~15 RPM        | Alta velocidad estable |
| 3          | ~10 RPM        | Velocidad media-alta |
| 5          | ~6 RPM         | Velocidad media |
| 10         | ~3 RPM         | Velocidad baja (muy estable) |
| 20         | ~1.5 RPM       | Velocidad muy baja |

## Ángulos Comunes

| Ángulo | Pasos | Descripción |
|--------|-------|-------------|
| 360°   | 2048  | Revolución completa |
| 180°   | 1024  | Media revolución |
| 90°    | 512   | Cuarto de revolución |
| 45°    | 256   | Octavo de revolución |
| 22.5°  | 128   | 1/16 de revolución |

## Código Básico Mínimo

```cpp
// Definir pines
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Secuencia half-step
const int seq[8][4] = {
  {1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,1,0},
  {0,0,1,0}, {0,0,1,1}, {0,0,0,1}, {1,0,0,1}
};

int step = 0;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Ejecutar un paso
  digitalWrite(IN1, seq[step][0]);
  digitalWrite(IN2, seq[step][1]);
  digitalWrite(IN3, seq[step][2]);
  digitalWrite(IN4, seq[step][3]);

  // Siguiente paso (adelante)
  step++;
  if (step >= 8) step = 0;

  delay(2);  // Ajustar velocidad
}
```

## Detener el Motor

```cpp
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
```

**Nota**: Apagar las bobinas elimina el holding torque.

## Invertir Dirección

**Método 1**: Decrementar el paso
```cpp
step--;
if (step < 0) step = 7;
```

**Método 2**: Invertir la secuencia
```cpp
// Leer la secuencia al revés
const int seqReverse[8][4] = {
  {1,0,0,1}, {0,0,0,1}, {0,0,1,1}, {0,0,1,0},
  {0,1,1,0}, {0,1,0,0}, {1,1,0,0}, {1,0,0,0}
};
```

## Librería Stepper.h

```cpp
#include <Stepper.h>

Stepper myStepper(2048, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(10);  // 10 RPM
}

void loop() {
  myStepper.step(2048);   // Adelante
  delay(1000);
  myStepper.step(-2048);  // Atrás
  delay(1000);
}
```

**Nota**: El orden de pines es diferente: IN1, IN3, IN2, IN4

## Solución de Problemas

| Problema | Posible Causa | Solución |
|----------|---------------|----------|
| Motor no gira | Conexiones incorrectas | Verifica el cableado |
| Motor vibra pero no gira | Velocidad muy alta | Aumenta el delay |
| Gira en dirección incorrecta | Secuencia invertida | Invierte la dirección del paso |
| Se calienta mucho | Voltaje incorrecto | Verifica jumper en 5V |
| Pierde pasos | Delay muy bajo o carga alta | Aumenta delay o reduce carga |
| No tiene fuerza | Bobinas no activas | Verifica que los LEDs se enciendan |

## Configuración del Driver ULN2003

### Verificar Jumper de Voltaje
```
┌──────────────┐
│   ULN2003    │
│              │
│  [5V][12V]   │ ← Jumper debe estar en 5V
│   ●          │    para motor 28BYJ-48 de 5V
└──────────────┘
```

### LEDs Indicadores
- LED A → Bobina 1 (IN1)
- LED B → Bobina 2 (IN2)
- LED C → Bobina 3 (IN3)
- LED D → Bobina 4 (IN4)

Durante la rotación deberías ver un patrón secuencial de LEDs encendiéndose.

## Cálculos de Tiempo

### Tiempo por Revolución
```
Tiempo (segundos) = (2048 × delay_ms) / 1000

Ejemplo con delay = 2ms:
Tiempo = (2048 × 2) / 1000 = 4.096 segundos
```

### RPM desde Tiempo
```
RPM = 60 / tiempo_por_revolucion_segundos

Ejemplo con 4.096 segundos:
RPM = 60 / 4.096 ≈ 14.65 RPM
```

## Aplicaciones Típicas

Este motor es ideal para:
- Relojes y displays rotativos
- Sistemas pan-tilt de cámaras
- Alimentadores y dispensadores
- Válvulas y persianas motorizadas
- Proyectos educativos y prototipos
- Control de posición de baja velocidad

**NO es ideal para**:
- Aplicaciones de alta velocidad
- Alta carga o torque elevado
- Movimiento continuo de larga duración
- Aplicaciones que requieren alta potencia

## Cuidados y Mantenimiento

1. No conectar/desconectar con alimentación activa
2. Verificar polaridad antes de alimentar
3. No exceder 5V en el motor
4. Asegurar buena conexión en el cable de 5 pines
5. Evitar obstrucción del eje del motor
6. No forzar el eje cuando está energizado

## Consumo de Energía

```
Corriente máxima: ~240mA (todas las bobinas)
Corriente típica: ~120-180mA (half-step)
Potencia: ~1.2W máximo

Asegúrate de que tu fuente de alimentación
pueda suministrar al menos 300mA a 5V.
```

## Recursos Adicionales

- Datasheet ULN2003: Driver Darlington array
- Arduino Stepper Library: https://www.arduino.cc/en/Reference/Stepper
- Calculadora de pasos online: (varios disponibles en internet)

---

**Última actualización**: 2025-12-30
