# Experimento: Motor Paso a Paso 28BYJ-48 con Driver ULN2003

## Objetivo
Aprender a controlar un motor paso a paso (stepper motor) 28BYJ-48 utilizando el driver ULN2003APG y comprender sus características de funcionamiento.

## Componentes

### Motor 28BYJ-48
- **Tipo**: Motor paso a paso unipolar de 5 bobinas
- **Voltaje**: 5V DC
- **Pasos por revolución**: 2048 pasos (en modo half-step)
  - Motor interno: 64 pasos por revolución
  - Relación de reducción: 1/64 (aproximadamente 1/63.68395)
  - Pasos totales = 64 × 64 = 4096 pasos (full-step) o 2048 (half-step según configuración)
- **Número de serie**: 20563200
- **Ángulo de paso**: 5.625° (64 pasos/revolución antes de la reducción)
- **Consumo**: ~240mA (todas las bobinas activadas)

### Driver ULN2003APG
- **Chip**: ULN2003A (Darlington Array de 7 canales)
- **Entradas activas**: IN1, IN2, IN3, IN4 (controlan las 4 bobinas del motor)
- **Capacidad total**: 7 entradas (solo usamos 4 para este motor)
- **Alimentación**: Acepta rango de 5V a 12V según especificaciones
  - **Para motor 28BYJ-48 de 5V**: Alimentar con 5V desde Arduino
  - **Jumper**: Algunos modelos incluyen jumper (función puede variar según fabricante)
- **Indicadores LED**: 4 LEDs (A, B, C, D) que muestran qué bobina está activa
- **Resistencias**: 4 resistencias para los LEDs

### Conexión
```
Arduino/ESP32          Driver ULN2003
---------------        --------------
Digital Pin 8    →     IN1
Digital Pin 9    →     IN2
Digital Pin 10   →     IN3
Digital Pin 11   →     IN4
5V               →     + (Alimentación driver)
GND              →     - (Tierra común)

Driver ULN2003         Motor 28BYJ-48
--------------         --------------
Conector de 5 pines → Conector del motor
```

## Conceptos Fundamentales

### ¿Qué es un Motor Paso a Paso?
A diferencia del motor DC que viste en el experimento anterior, un motor paso a paso:
- Se mueve en **pasos discretos** y precisos (no es continuo)
- Puedes controlar **posición exacta** sin necesidad de encoders
- Mantiene **torque en reposo** (holding torque)
- Requiere **secuencias de activación** de bobinas específicas
- NO se controla con PWM simple

### Tipos de Secuencias de Paso

#### 1. Full-Step (Paso Completo)
- Activa 2 bobinas simultáneamente
- Máximo torque
- 4096 pasos por revolución completa

#### 2. Half-Step (Medio Paso)
- Alterna entre 1 y 2 bobinas activas
- Mayor suavidad de movimiento
- 8 pasos por ciclo (vs 4 en full-step)
- 2048 pasos por revolución completa
- Menor torque pero más precisión

#### 3. Wave Drive
- Solo 1 bobina activa a la vez
- Menor consumo
- Menor torque

### Secuencias de Activación

**Half-Step (8 pasos por ciclo):**
```
Paso  IN1  IN2  IN3  IN4
  0    1    0    0    0
  1    1    1    0    0
  2    0    1    0    0
  3    0    1    1    0
  4    0    0    1    0
  5    0    0    1    1
  6    0    0    0    1
  7    1    0    0    1
```

## Plan de Aprendizaje

### Fase 1: Verificación Básica
**Sketch**: `01_basic_stepper_test.ino`
- Implementación manual de la secuencia half-step
- Observar los LEDs del driver
- Verificar funcionamiento básico
- **Objetivo**: Entender cómo funcionan las secuencias de pasos

### Fase 2: Control con Librería
**Sketch**: `02_stepper_library.ino`
- Uso de la librería `Stepper.h` de Arduino
- Control simplificado de pasos
- **Objetivo**: Aprender a usar herramientas estándar

### Fase 3: Mediciones y Precisión
**Sketch**: `03_stepper_measurement.ino`
- Medir velocidad máxima
- Probar diferentes velocidades
- Verificar precisión de pasos
- **Objetivo**: Caracterizar el motor

### Fase 4: Control Avanzado
**Sketch**: `04_stepper_advanced_control.ino`
- Control de dirección (CW/CCW)
- Velocidad variable
- Aceleración/deceleración
- Control por Serial
- **Objetivo**: Aplicaciones prácticas

## Consideraciones Importantes

### ⚠️ Verificar ANTES de Conectar
1. **Alimentación del driver**: Usa 5V desde Arduino para el motor 28BYJ-48 de 5V
2. **Conexiones firmes**: Verifica que todos los cables estén bien conectados
3. **No conectar/desconectar** el motor con el driver energizado

### Limitaciones
- **Velocidad máxima**: ~15 RPM (revoluciones por minuto) en condiciones óptimas
- **No funciona bien a muy alta velocidad**: Puede perder pasos
- **Consumo**: Ten una fuente capaz de suministrar al menos 300mA

### Ventajas del 28BYJ-48
- ✓ Muy económico
- ✓ Excelente para aprendizaje
- ✓ Precisión angular buena
- ✓ Bajo ruido
- ✓ Fácil de usar con Arduino

### Desventajas
- ✗ Velocidad limitada
- ✗ Torque bajo
- ✗ No es fácil modificar para usar microstepping

## Próximos Pasos

1. Verifica todas las conexiones según el diagrama
2. Alimenta el driver con 5V desde Arduino
3. Ejecuta el sketch básico (01_basic_stepper_test.ino)
4. Observa los LEDs del driver mientras el motor gira
5. Progresa a través de los sketches en orden

## Referencias
- Datasheet ULN2003: Darlington array de 7 canales
- 28BYJ-48: Motor unipolar de 5V con reducción 1:64
