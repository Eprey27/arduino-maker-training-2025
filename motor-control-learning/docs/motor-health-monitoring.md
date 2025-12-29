# Sistema de Monitoreo de Salud del Motor (Motor Health Monitoring System)

## 🎯 Objetivo

Crear un sistema completo de diagnóstico que mida múltiples parámetros del motor DC para determinar su estado de salud antes de tareas exigentes. Este sistema permitirá detectar desgaste, problemas mecánicos, y establecer una línea base de rendimiento.

## 📊 Parámetros a Monitorear

### 1. **RPM (Revoluciones Por Minuto)**
- **Importancia**: Indica velocidad y consistencia del motor
- **Método**: Sensor óptico (IR LED + fototransistor) o sensor Hall
- **Valores esperados**: Pendiente caracterización
- **Anomalías**: Fluctuaciones >10%, velocidad inconsistente

### 2. **Consumo de Corriente**
- **Importancia**: Indica carga mecánica y eficiencia
- **Método**: Sensor INA219 (I2C, alta precisión) o ACS712 (analógico)
- **Valores normales**: 60-90mA @ 5V (según tests previos)
- **Anomalías**: Picos >150mA, consumo errático, corriente creciente en el tiempo

### 3. **Voltaje de Alimentación**
- **Importancia**: Verificar caída de voltaje bajo carga
- **Método**: Divisor de voltaje o INA219
- **Valores normales**: ~4.9-5.0V bajo carga
- **Anomalías**: Caída >0.2V indica resistencia alta o fuente insuficiente

### 4. **Temperatura del Motor**
- **Importancia**: Detectar fricción excesiva, desgaste de rodamientos
- **Método**:
  - MLX90614 (IR, sin contacto, ideal)
  - DS18B20 (contacto, económico)
  - Termistor NTC (barato)
- **Medición adicional**: Temperatura ambiente (DHT22 o BME280)
- **Valores normales**: Delta <10°C después de 5 min @ 5V
- **Anomalías**: Delta >15°C, temperatura creciente sin estabilizar

### 5. **Vibraciones**
- **Importancia**: Detectar desbalanceo, rodamientos dañados, eje doblado
- **Método**:
  - ADXL345 (acelerómetro 3 ejes, I2C, económico)
  - MPU6050 (accel + gyro, más completo)
- **Medición**: Vibración en X, Y, Z a diferentes RPM
- **Ubicación**: Montado firmemente a 2-5cm del motor
- **Valores**: Establecer baseline, detectar cambios >50%
- **Análisis**: FFT para detectar frecuencias problemáticas

### 6. **Ruido Acústico**
- **Importancia**: Detectar desgaste de escobillas, rodamientos
- **Método**:
  - Módulo KY-038 / LM393 (simple, económico)
  - MAX9814 (con AGC, mejor calidad)
- **Ubicación**: 10cm del motor, posición fija
- **Medición**: dB promedio y picos
- **Valores**: Baseline en diferentes RPM
- **Anomalías**: Ruido >20% sobre baseline, clics, chirridos

## 🛠️ Hardware Necesario

### Sensores Principales

| Sensor | Función | Precio aprox | Comunicación | Prioridad |
|--------|---------|--------------|--------------|-----------|
| **INA219** | Voltaje + Corriente | €3-5 | I2C | ALTA |
| **TCRT5000** | RPM (óptico IR) | €1-2 | Digital/Analog | ALTA |
| **MLX90614** | Temp IR motor | €8-12 | I2C | MEDIA |
| **DS18B20** | Temp contacto | €2-3 | 1-Wire | MEDIA (alt) |
| **DHT22** | Temp + Humid ambiente | €3-5 | Digital | MEDIA |
| **ADXL345** | Acelerómetro 3 ejes | €2-4 | I2C/SPI | ALTA |
| **KY-038** | Micrófono | €1-2 | Analog + Digital | BAJA |

### Hardware Adicional
- Arduino Mega 2560 (más pines que UNO) o mantener UNO con multiplexado
- Display OLED 128x64 I2C (para visualización en tiempo real)
- SD Card Module (para logging de datos)
- Breadboard grande o PCB prototipo
- Soporte/fixture para posicionar sensores de forma repetible

## 📐 Arquitectura del Sistema

### Fase A: Sensores Individuales (Tests Aislados)
```
motor-control-learning/sketches/04_motor_health/
├── 00_individual_tests/
│   ├── rpm_sensor_test/
│   │   └── rpm_sensor_test.ino         # Test TCRT5000 o Hall
│   ├── current_sensor_test/
│   │   └── current_sensor_test.ino     # Test INA219
│   ├── temp_sensor_test/
│   │   └── temp_sensor_test.ino        # Test MLX90614 + DHT22
│   ├── vibration_sensor_test/
│   │   └── vibration_sensor_test.ino   # Test ADXL345
│   └── sound_sensor_test/
│       └── sound_sensor_test.ino       # Test KY-038
```

### Fase B: Integración Modular
```
└── 01_integrated_monitoring/
    ├── basic_monitoring/
    │   └── basic_monitoring.ino        # RPM + Corriente + Temp
    ├── advanced_monitoring/
    │   └── advanced_monitoring.ino     # Todo integrado
    └── health_diagnostic/
        └── health_diagnostic.ino       # Análisis automático
```

### Fase C: Sistema Completo con Logging
```
└── 02_complete_system/
    └── motor_health_analyzer/
        ├── motor_health_analyzer.ino   # Sketch principal
        ├── sensors.h                   # Librería de sensores
        ├── display.h                   # Control OLED
        ├── logger.h                    # SD card logging
        └── analyzer.h                  # Análisis de salud
```

## 🔌 Conexiones (Borrador)

### I2C Bus (compartido)
```
Arduino SDA (A4 en UNO) ─┬─ INA219 SDA
                         ├─ ADXL345 SDA
                         ├─ MLX90614 SDA
                         └─ OLED SDA

Arduino SCL (A5 en UNO) ─┬─ INA219 SCL
                         ├─ ADXL345 SCL
                         ├─ MLX90614 SCL
                         └─ OLED SCL
```

### Pines Analógicos
```
A0 → Sensor RPM TCRT5000 (si no es digital)
A1 → Sensor de sonido KY-038
A2 → [Reservado]
```

### Pines Digitales
```
D2 → RPM sensor (interrupción INT0)
D3 → [Reservado para interrupción INT1]
D4-D7 → SD Card Module (SPI si se usa)
D10-D13 → [Reservado SPI]
```

### OneWire (si se usa DS18B20)
```
D8 → DS18B20 Data
```

### Alimentación
```
Motor → INA219 Vin+ → 5V source
Motor GND → INA219 Vin- → GND
INA219 medirá corriente en serie
```

## 📋 Protocolo de Diagnóstico

### Test Estándar de Salud del Motor

**Condiciones Iniciales**:
1. Motor en frío (temp ambiente)
2. Sin carga mecánica
3. Todos los sensores calibrados
4. Posición de sensores marcada y fija

**Secuencia de Test** (5 minutos):
```
1. Medición baseline (0s - 30s):
   - Motor apagado
   - Temp ambiente, vibración ambiente, ruido ambiente

2. Arranque suave (30s - 60s):
   - PWM de 0 a 100 en 30 segundos
   - Medir corriente de arranque
   - Detectar voltaje mínimo de arranque

3. Velocidad baja (60s - 120s):
   - PWM = 50 (2.5V aprox)
   - Medir todos los parámetros

4. Velocidad media (120s - 180s):
   - PWM = 128 (2.5V)
   - Medir estabilidad RPM

5. Velocidad alta (180s - 240s):
   - PWM = 200
   - Medir temperatura, corriente, vibraciones

6. Velocidad máxima (240s - 270s):
   - PWM = 255 (5V)
   - Medir límites del motor

7. Cooldown (270s - 300s):
   - Motor apagado
   - Medir tiempo de desaceleración
   - Medir enfriamiento
```

**Datos a Registrar**:
- Timestamp
- PWM value
- RPM actual
- Corriente (mA)
- Voltaje (V)
- Temp motor (°C)
- Temp ambiente (°C)
- Vibración X, Y, Z (g)
- Nivel de ruido (dB o ADC)

**Formato de Salida** (CSV para SD card):
```csv
Time(ms),PWM,RPM,Current(mA),Voltage(V),TempMotor(C),TempAmb(C),VibX(g),VibY(g),VibZ(g),Sound(dB)
0,0,0,0,5.00,22.5,22.5,0.01,0.01,0.98,35
1000,25,450,45,4.98,22.6,22.5,0.15,0.12,1.05,42
...
```

## 📊 Análisis de Salud

### Indicadores de Motor Saludable
- ✅ Corriente estable ±5mA en misma velocidad
- ✅ RPM estable ±10 RPM en misma velocidad
- ✅ Delta temperatura <10°C después de 5 min
- ✅ Vibraciones consistentes, sin picos
- ✅ Ruido estable, sin chirridos

### Señales de Advertencia (Warning)
- ⚠️ Corriente aumenta 10-20% respecto baseline
- ⚠️ Fluctuaciones RPM 10-20%
- ⚠️ Delta temperatura 10-15°C
- ⚠️ Vibraciones aumentan 20-50%
- ⚠️ Ruido aumenta 20-40%

### Señales Críticas (Error)
- ❌ Corriente >150mA o aumenta >20%
- ❌ Motor no arranca o arranca erráticamente
- ❌ Delta temperatura >15°C
- ❌ Vibraciones >50% sobre baseline
- ❌ Ruido >40% o sonidos anormales (clics, raspaduras)
- ❌ Temperatura no se estabiliza

### Sistema de Puntuación
```
Health Score = 100 puntos iniciales

Por cada anomalía:
- Warning: -5 puntos
- Error: -20 puntos

Clasificación:
- 90-100: Excelente (verde)
- 70-89: Bueno (amarillo)
- 50-69: Advertencia (naranja)
- <50: Crítico - no usar para tareas exigentes (rojo)
```

## 🎓 Objetivos de Aprendizaje

1. **Sensores I2C**: Comunicación multi-dispositivo, direcciones
2. **Interrupciones**: Medición precisa de RPM
3. **Muestreo**: Aliasing, frecuencias de Nyquist
4. **Análisis de señal**: FFT básica para vibraciones
5. **Logging de datos**: SD cards, formato CSV
6. **Displays**: I2C OLED, gráficos en tiempo real
7. **Diagnóstico**: Establecer baselines, detectar derivas

## 🚀 Plan de Implementación

### Sprint 1: Sensor de RPM (Fundacional)
- [ ] Adquirir TCRT5000 o sensor Hall
- [ ] Test individual del sensor
- [ ] Crear marca reflectante o imán en eje motor
- [ ] Implementar medición con interrupciones
- [ ] Validar precisión (comparar con cálculo teórico)

### Sprint 2: Sensor de Corriente y Voltaje
- [ ] Adquirir INA219
- [ ] Test individual I2C
- [ ] Integrar en serie con motor
- [ ] Calibrar mediciones
- [ ] Crear curva I-V del motor

### Sprint 3: Temperatura
- [ ] Adquirir MLX90614 (IR) o DS18B20
- [ ] Adquirir DHT22 para ambiente
- [ ] Test de sensores
- [ ] Establecer posición de medición IR
- [ ] Crear curva térmica del motor

### Sprint 4: Vibraciones
- [ ] Adquirir ADXL345
- [ ] Test y calibración
- [ ] Diseñar fixture de montaje rígido
- [ ] Capturar baseline de vibraciones
- [ ] Implementar análisis FFT básico

### Sprint 5: Ruido
- [ ] Adquirir módulo de micrófono
- [ ] Test y calibración
- [ ] Establecer posición y distancia fija
- [ ] Capturar baseline de ruido
- [ ] Correlacionar con RPM

### Sprint 6: Integración
- [ ] Combinar todos los sensores
- [ ] Resolver conflictos de pines/I2C
- [ ] Implementar display OLED
- [ ] Crear interfaz de usuario

### Sprint 7: Logging y Análisis
- [ ] Implementar SD card logging
- [ ] Crear protocolo de test de 5 min
- [ ] Implementar algoritmo de scoring
- [ ] Generar reportes

### Sprint 8: Validación
- [ ] Realizar tests repetidos
- [ ] Comparar motor nuevo vs usado
- [ ] Simular condiciones de desgaste
- [ ] Ajustar umbrales de detección

## 💾 Estructura de Datos

### Clase SensorData (C++)
```cpp
struct MotorHealthData {
  unsigned long timestamp;    // millis()
  uint8_t pwm;               // 0-255
  uint16_t rpm;              // revoluciones por minuto
  float current_mA;          // corriente en miliamperios
  float voltage_V;           // voltaje en voltios
  float temp_motor_C;        // temperatura motor
  float temp_ambient_C;      // temperatura ambiente
  float vib_x_g;             // vibración eje X en g
  float vib_y_g;             // vibración eje Y en g
  float vib_z_g;             // vibración eje Z en g
  uint16_t sound_level;      // nivel de sonido ADC o dB
  uint8_t health_score;      // 0-100
};
```

## 📚 Librerías Necesarias

```cpp
// I2C Communication
#include <Wire.h>

// Sensores específicos
#include <Adafruit_INA219.h>      // Corriente/Voltaje
#include <Adafruit_MLX90614.h>    // Temperatura IR
#include <OneWire.h>              // DS18B20 (alternativa)
#include <DallasTemperature.h>    // DS18B20
#include <DHT.h>                  // DHT22 ambiente
#include <Adafruit_ADXL345_U.h>   // Acelerómetro

// Display y almacenamiento
#include <Adafruit_SSD1306.h>     // OLED display
#include <SD.h>                   // SD card
#include <SPI.h>                  // SPI para SD

// Análisis (opcional)
#include <arduinoFFT.h>           // FFT para vibraciones
```

## 🔧 Notas de Implementación

### Consideraciones de Diseño
1. **I2C Pull-ups**: Verificar que no haya conflictos (4.7KΩ típico)
2. **Alimentación**: Sensores consumen ~50mA total, considerar fuente externa
3. **Sampling Rate**: Mínimo 2x frecuencia máxima esperada (Nyquist)
4. **Buffer de datos**: Arduino UNO tiene 2KB RAM, usar circular buffer
5. **SD Card**: Escrituras en batch para evitar latencia
6. **Display**: Actualizar cada 200-500ms para legibilidad

### Problemas Anticipados
- **I2C address conflicts**: Usar I2C scanner, cambiar address si es posible
- **Timing**: Timer conflicts entre PWM y mediciones
- **Memory**: UNO limitado, considerar MEGA o reducir buffers
- **Power**: Picos de corriente del motor pueden afectar sensores (usar capacitor)
- **Montaje**: Vibraciones deben medirse en fixture rígido

## 📖 Referencias

- [INA219 Datasheet](https://www.ti.com/product/INA219)
- [ADXL345 Datasheet](https://www.analog.com/en/products/adxl345.html)
- [MLX90614 Datasheet](https://www.melexis.com/en/product/MLX90614/)
- [Motor RPM measurement tutorial](https://howtomechatronics.com/tutorials/arduino/arduino-dc-motor-speed-measurement/)

## 🎯 Próximos Pasos

1. **Definir presupuesto** y priorizar sensores
2. **Adquirir componentes** según Sprint 1-2
3. **Crear sketches de test** individuales
4. **Documentar resultados** de cada sensor
5. **Integrar progresivamente**

---

**Creado**: 2024-12-29
**Versión**: 1.0
**Estado**: Diseño inicial - pendiente implementación
**Autor**: Emilio (con asistencia de Claude Sonnet 4.5)
