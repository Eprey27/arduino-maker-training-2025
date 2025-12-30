# Próximos Pasos: Sistema de Calibración Automática con Sensores

## Estado Actual del Proyecto ✓

**Fecha:** 2025-12-30

### Completado

- ✓ Calibración manual del motor 28BYJ-48
- ✓ Valor final calibrado: **4097 pasos por revolución**
- ✓ Error sistemático medido: +0.266°/rev (con 4100 pasos)
- ✓ Todos los sketches actualizados con el valor calibrado
- ✓ Sistema de prueba de precisión estadística implementado
- ✓ Documentación completa en `CALIBRATION.md`

### Precisión Alcanzada (Manual)

- **Método:** Observación visual y conteo de revoluciones
- **Desviación estándar:** 0.1385°
- **Error promedio:** +0.266°/rev antes del ajuste final
- **Limitación:** Medición subjetiva basada en marcas visuales

---

## Objetivo: Sistema de Calibración Automática con Sensores

### Propósito

Desarrollar un sistema de calibración de alta precisión que elimine el error humano mediante:

1. **Medición automática de posición angular**
2. **Cálculo preciso en grados, minutos y segundos de arco**
3. **Detección de error sistemático vs aleatorio**
4. **Calibración iterativa automática**
5. **Registro histórico de mediciones**

### Precisión Objetivo

| Medición | Resolución | Precisión Esperada |
|----------|------------|-------------------|
| Grados   | 1°         | ±0.1°             |
| Minutos  | 1'         | ±6' (0.1°)        |
| Segundos | 1"         | ±360" (6')        |

**Nota:** 1° = 60' (minutos), 1' = 60" (segundos)

---

## Opciones de Sensores

### Opción 1: Encoder Rotatorio Óptico (RECOMENDADO)

**Sensor sugerido:** AS5600 (Encoder Magnético de 12 bits)

**Características:**
- Resolución: 4096 posiciones por revolución (12 bits)
- Precisión: ±0.088° (equivalente a nuestro motor calibrado)
- Comunicación: I2C
- Sin contacto (magnético)
- Costo: ~$3-5 USD

**Ventajas:**
- Alta resolución nativa
- No requiere contacto mecánico
- Fácil integración I2C
- Medición de ángulo absoluto

**Desventajas:**
- Requiere imán diametral montado en el eje
- Limitado a una revolución (0-360°)

**Precisión alcanzable:**
```
Resolución: 360° / 4096 = 0.088° = 5.27' = 316.4"
```

### Opción 2: Encoder Incremental de Alta Resolución

**Sensor sugerido:** Encoder óptico 600 PPR (Pulsos Por Revolución)

**Características:**
- Resolución: 600-2400 PPR
- Salidas: A, B (cuadratura) + Z (índice)
- Modo cuadratura: 2400 pulsos efectivos (600 PPR × 4)

**Ventajas:**
- Muy alta precisión
- Detección de dirección
- Pulso de índice para referencia absoluta

**Desventajas:**
- Más costoso (~$10-20 USD)
- Requiere acoplamiento mecánico al eje
- Más complejo de instalar

**Precisión alcanzable:**
```
600 PPR × 4 (cuadratura) = 2400 pulsos/rev
Resolución: 360° / 2400 = 0.15° = 9' = 540"
```

### Opción 3: Sensor Hall Magnético + Disco Codificador

**Sensor sugerido:** MLX90316 (Encoder magnético rotatorio)

**Características:**
- Resolución: 14 bits (16384 posiciones)
- Comunicación: SPI o PWM
- Rango: 360° absoluto

**Ventajas:**
- Máxima resolución
- Ángulo absoluto
- Sin contacto

**Desventajas:**
- Más costoso (~$8-12 USD)
- Requiere configuración SPI

**Precisión alcanzable:**
```
Resolución: 360° / 16384 = 0.022° = 1.32' = 79.2"
```

### Opción 4: Sensor de Posición Lineal (Alternativa)

**Sensor sugerido:** Potenciómetro de precisión multi-turn

**Características:**
- Medición analógica
- Puede medir múltiples revoluciones
- Simple de implementar

**Ventajas:**
- Económico (~$2-5 USD)
- Fácil de programar (lectura ADC)

**Desventajas:**
- Menor precisión (~1-2°)
- Desgaste mecánico
- Requiere calibración ADC

---

## Arquitectura del Sistema

### Hardware Propuesto

```
Arduino/ESP32
    ├── Motor 28BYJ-48 + Driver ULN2003
    │   └── Pines: 8, 9, 10, 11
    │
    ├── AS5600 Encoder Magnético (I2C)
    │   ├── SDA -> Pin A4 (Arduino) / GPIO21 (ESP32)
    │   ├── SCL -> Pin A5 (Arduino) / GPIO22 (ESP32)
    │   └── Imán diametral en eje del motor
    │
    └── Display OLED (Opcional)
        └── Mostrar ángulo en tiempo real
```

### Montaje Mecánico

1. **Imán diametral** montado en el eje del motor
2. **Sensor AS5600** posicionado a 0.5-3mm del imán
3. **Soporte fijo** para mantener el sensor estable
4. **Alineación** del sensor con el centro del eje

### Software - Estructura de Sketches

```
sketches/
├── 07_auto_calibration_system/
│   ├── 07_auto_calibration_system.ino     # Sistema completo
│   ├── AS5600.h                            # Librería del sensor
│   └── CalibrationEngine.h                 # Motor de calibración
│
├── 08_sensor_verification/
│   └── 08_sensor_verification.ino          # Verificar sensor
│
└── 09_precision_comparison/
    └── 09_precision_comparison.ino         # Comparar con calibración manual
```

---

## Especificación del Sketch 07: Sistema de Calibración Automática

### Funcionalidades

#### 1. Detección y Configuración del Sensor

```cpp
bool initializeSensor() {
  // Detectar AS5600 en I2C
  // Configurar modos y resolución
  // Verificar funcionalidad
  // Retornar true si exitoso
}
```

#### 2. Medición de Ángulo Preciso

```cpp
struct AngleMeasurement {
  float degrees;        // Grados (0-360)
  int minutes;          // Minutos de arco (0-59)
  float seconds;        // Segundos de arco (0-59.99)
  unsigned long timestamp;
};

AngleMeasurement readPreciseAngle() {
  // Leer valor raw del sensor
  // Convertir a grados decimales
  // Calcular minutos y segundos
  // Timestamp para análisis temporal
}
```

#### 3. Prueba de Calibración Automática

```cpp
struct CalibrationTest {
  int targetRevolutions;      // Revoluciones objetivo
  long stepsExecuted;         // Pasos ejecutados
  AngleMeasurement initial;   // Ángulo inicial
  AngleMeasurement final;     // Ángulo final
  float errorDegrees;         // Error en grados
  float errorMinutes;         // Error en minutos
  float errorSeconds;         // Error en segundos
  float errorPerRevolution;   // Error/rev
};

CalibrationTest performAutoTest(int revolutions) {
  // 1. Registrar ángulo inicial
  // 2. Ejecutar N revoluciones
  // 3. Registrar ángulo final
  // 4. Calcular error absoluto
  // 5. Calcular error por revolución
  // 6. Retornar resultados estructurados
}
```

#### 4. Análisis Estadístico Mejorado

```cpp
struct StatisticalAnalysis {
  float meanError;              // Error promedio
  float standardDeviation;      // Desviación estándar
  float maxError;               // Error máximo
  float minError;               // Error mínimo
  float systematicError;        // Error sistemático
  float randomError;            // Error aleatorio
  bool isSystematic;            // ¿Error es sistemático?
  long recommendedSteps;        // Pasos recomendados
};

StatisticalAnalysis analyzeResults(CalibrationTest tests[], int count) {
  // Análisis estadístico completo
  // Detección de tendencias
  // Cálculo de valor óptimo
}
```

#### 5. Calibración Iterativa

```cpp
long iterativeCalibration(long initialSteps, float targetPrecision) {
  // 1. Empezar con initialSteps
  // 2. Ejecutar batería de pruebas
  // 3. Calcular error
  // 4. Si error > targetPrecision:
  //    a. Ajustar STEPS_PER_REVOLUTION
  //    b. Repetir pruebas
  // 5. Retornar valor óptimo cuando error < targetPrecision
}
```

#### 6. Formato de Salida en Grados/Minutos/Segundos

```cpp
void printAngleDMS(AngleMeasurement angle) {
  // Formato: 45°30'15.5"
  Serial.print(angle.degrees, 0);
  Serial.print("°");
  Serial.print(angle.minutes);
  Serial.print("'");
  Serial.print(angle.seconds, 2);
  Serial.print("\"");
}

void printErrorDMS(float errorDegrees) {
  int deg = (int)errorDegrees;
  float remainder = (errorDegrees - deg) * 60.0;
  int min = (int)remainder;
  float sec = (remainder - min) * 60.0;

  if (deg != 0) {
    Serial.print(deg);
    Serial.print("° ");
  }
  if (min != 0) {
    Serial.print(min);
    Serial.print("' ");
  }
  Serial.print(sec, 2);
  Serial.print("\"");
}
```

#### 7. Registro de Datos (Data Logging)

```cpp
void logCalibrationData(CalibrationTest test) {
  // Formato CSV para análisis posterior
  // timestamp,revolutions,steps,initial_angle,final_angle,error_deg,error_min,error_sec

  Serial.print(test.initial.timestamp);
  Serial.print(",");
  Serial.print(test.targetRevolutions);
  Serial.print(",");
  Serial.print(test.stepsExecuted);
  Serial.print(",");
  Serial.print(test.initial.degrees, 4);
  Serial.print(",");
  Serial.print(test.final.degrees, 4);
  Serial.print(",");
  Serial.print(test.errorDegrees, 4);
  Serial.print(",");
  Serial.print(test.errorMinutes, 2);
  Serial.print(",");
  Serial.print(test.errorSeconds, 2);
  Serial.println();
}
```

---

## Plan de Implementación

### Fase 1: Adquisición de Hardware (Día 1-2)

- [ ] Comprar sensor AS5600 + imán diametral
- [ ] Opcional: Display OLED I2C
- [ ] Materiales de montaje (soporte, pegamento, tornillos)

**Proveedores sugeridos:**
- AliExpress: AS5600 Encoder Module (~$3 USD)
- Amazon: Kit AS5600 con imán (~$8 USD)

### Fase 2: Montaje Mecánico (Día 3)

- [ ] Diseñar soporte para sensor AS5600
  - Distancia: 0.5-3mm del imán
  - Alineación centrada con el eje
- [ ] Montar imán en el eje del motor
- [ ] Fijar sensor en posición
- [ ] Verificar que no haya roce mecánico

### Fase 3: Verificación del Sensor (Día 4)

- [ ] Crear sketch `08_sensor_verification.ino`
- [ ] Probar comunicación I2C
- [ ] Leer ángulos en tiempo real
- [ ] Verificar precisión con transportador manual
- [ ] Calibrar offset del sensor si es necesario

### Fase 4: Desarrollo del Sistema de Calibración (Día 5-6)

- [ ] Crear sketch `07_auto_calibration_system.ino`
- [ ] Implementar lectura precisa de ángulos
- [ ] Implementar conversión a grados/minutos/segundos
- [ ] Implementar pruebas automáticas
- [ ] Implementar análisis estadístico

### Fase 5: Calibración Automática (Día 7)

- [ ] Ejecutar batería de pruebas con valor actual (4097)
- [ ] Analizar error real con sensor
- [ ] Ejecutar calibración iterativa
- [ ] Determinar valor óptimo final
- [ ] Actualizar todos los sketches

### Fase 6: Validación y Documentación (Día 8)

- [ ] Comparar resultados con calibración manual
- [ ] Crear sketch `09_precision_comparison.ino`
- [ ] Documentar precisión alcanzada
- [ ] Actualizar `CALIBRATION.md` con datos del sensor
- [ ] Crear gráficos de error (opcional)

---

## Resultados Esperados

### Mejoras sobre el Método Manual

| Aspecto | Manual | Con Sensor |
|---------|--------|------------|
| Precisión de medición | ±2-5° | ±0.088° |
| Resolución | Visual (1-5°) | 0.088° (5.27') |
| Repetibilidad | Baja | Alta |
| Tiempo de calibración | 15-20 min | 2-5 min |
| Error humano | Alto | Eliminado |
| Datos cuantitativos | Limitados | Completos |

### Métricas a Documentar

1. **Error absoluto en D°M'S"**
   - Por ejemplo: 0°15'45" (15.75 minutos de arco)

2. **Error por revolución**
   - Grados/rev, minutos/rev, segundos/rev

3. **Desviación estándar**
   - En segundos de arco

4. **Tipo de error**
   - Sistemático vs aleatorio
   - Tendencia (positivo/negativo)

5. **Valor calibrado final**
   - STEPS_PER_REVOLUTION optimizado
   - Factor de corrección preciso

---

## Consideraciones Técnicas

### Conversión Grados ↔ Minutos ↔ Segundos

```cpp
// Conversión de grados decimales a DMS
void decimalToDMS(float decimal, int &deg, int &min, float &sec) {
  deg = (int)decimal;
  float remainder = (decimal - deg) * 60.0;
  min = (int)remainder;
  sec = (remainder - min) * 60.0;
}

// Conversión de DMS a grados decimales
float DMStoDecimal(int deg, int min, float sec) {
  return deg + (min / 60.0) + (sec / 3600.0);
}

// Calcular error en segundos de arco
float errorInArcSeconds(float errorDegrees) {
  return errorDegrees * 3600.0;  // 1° = 3600"
}
```

### Detección de Error de Múltiples Revoluciones

```cpp
// El AS5600 solo mide 0-360°
// Para múltiples revoluciones, necesitamos contar vueltas

int revolutionCount = 0;
float lastAngle = 0;
float totalAngle = 0;

void updateTotalAngle(float currentAngle) {
  // Detectar paso por 0° (vuelta completa)
  if (lastAngle > 270 && currentAngle < 90) {
    revolutionCount++;
  } else if (lastAngle < 90 && currentAngle > 270) {
    revolutionCount--;
  }

  totalAngle = (revolutionCount * 360.0) + currentAngle;
  lastAngle = currentAngle;
}
```

---

## Estructura de Archivos Actualizada

```
motor-control-learning/stepper-motor-28byj48/
├── CALIBRATION.md                          # Calibración manual (actual)
├── CALIBRATION_AUTO.md                     # Calibración automática (futuro)
├── NEXT_STEPS.md                           # Este archivo
├── README.md                               # Documentación principal
│
├── sketches/
│   ├── 00_steps_verification/              # ✓ Verificación básica
│   ├── 01_basic_stepper_test/              # ✓ Test básico
│   ├── 02_stepper_library/                 # ✓ Con librería
│   ├── 03_stepper_measurement/             # ✓ Mediciones
│   ├── 04_stepper_advanced_control/        # ✓ Control avanzado
│   ├── 05_stepper_calibration/             # ✓ Calibración manual
│   ├── 06_precision_test/                  # ✓ Test de precisión
│   │
│   ├── 07_auto_calibration_system/         # ⏳ SIGUIENTE PASO
│   │   ├── 07_auto_calibration_system.ino
│   │   ├── AS5600.h                        # Librería sensor
│   │   └── README.md                       # Instrucciones de uso
│   │
│   ├── 08_sensor_verification/             # ⏳ Verificación sensor
│   │   └── 08_sensor_verification.ino
│   │
│   └── 09_precision_comparison/            # ⏳ Comparación
│       └── 09_precision_comparison.ino
│
├── hardware/
│   ├── as5600_mounting.md                  # Guía de montaje
│   ├── sensor_wiring.md                    # Diagrama de conexiones
│   └── parts_list.md                       # Lista de materiales
│
└── data/
    ├── calibration_log.csv                 # Registro de calibraciones
    └── sensor_measurements.csv             # Datos del sensor
```

---

## Recursos y Referencias

### Librerías Arduino para AS5600

1. **AS5600** by Rob Tillaart
   - GitHub: https://github.com/RobTillaart/AS5600
   - Instalación: Arduino Library Manager

2. **Magnet Encoder AS5600** por Seeed Studio
   - Documentación completa
   - Ejemplos de código

### Documentación Técnica

- **Datasheet AS5600:** https://ams.com/as5600
- **Application Note:** Magnetic Rotary Position Sensor
- **Tutorial:** I2C Communication with AS5600

### Cálculos de Precisión

```
Resolución del motor calibrado: 4097 pasos/rev
Resolución del sensor AS5600:   4096 posiciones/rev

Grados por paso motor:  360° / 4097 = 0.0879°
Grados por paso sensor: 360° / 4096 = 0.0879°

¡Resoluciones casi idénticas! - Perfecta coincidencia
```

---

## Preguntas para Resolver

### Hardware

- [ ] ¿Qué sensor específico compraremos? (Recomendado: AS5600)
- [ ] ¿Necesitamos display OLED para visualización?
- [ ] ¿Cómo montaremos el imán en el eje del motor?

### Software

- [ ] ¿Qué nivel de precisión objetivo? (Sugerido: <0.1° = 6')
- [ ] ¿Cuántas pruebas por calibración? (Sugerido: 10-20)
- [ ] ¿Guardamos datos en SD card o solo Serial?

### Metodología

- [ ] ¿Calibración de una sola pasada o iterativa?
- [ ] ¿Temperatura ambiente afecta? ¿Medir temperatura?
- [ ] ¿Probar con diferentes velocidades/delays?

---

## Comando para Retomar

Cuando regreses, puedes decir:

```
"Continúa con el desarrollo del sistema de calibración automática
según NEXT_STEPS.md. Empecemos por crear el sketch 08_sensor_verification
para verificar el sensor AS5600 cuando lo tenga instalado."
```

O si ya tienes el sensor:

```
"Tengo el sensor AS5600 instalado. Ayúdame a crear el sketch
07_auto_calibration_system siguiendo la especificación de NEXT_STEPS.md"
```

---

## Notas Finales

### Estado del Proyecto

- **Calibración manual:** ✓ COMPLETADA
- **Valor actual:** 4097 pasos/rev
- **Precisión manual:** ±0.1385° (desviación estándar)
- **Próximo hito:** Calibración automática con sensor

### Beneficios del Nuevo Sistema

1. **Precisión:** De ±2-5° (visual) a ±0.088° (sensor)
2. **Objetividad:** Eliminación del error humano
3. **Repetibilidad:** Calibración consistente
4. **Datos:** Métricas cuantitativas en D°M'S"
5. **Automatización:** Proceso de 2-5 minutos

### Próxima Sesión

1. Revisar este documento
2. Verificar hardware disponible
3. Decidir sensor a utilizar
4. Comenzar con Fase correspondiente

---

**Documento creado:** 2025-12-30
**Autor:** Sistema de calibración motor 28BYJ-48
**Versión:** 1.0
**Estado:** Pendiente de implementación

¡Buena suerte con la implementación! 🎯
