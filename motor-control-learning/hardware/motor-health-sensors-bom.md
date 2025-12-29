# Motor Health Monitoring System - Bill of Materials

## 📋 Lista de Compra de Sensores

Esta lista incluye todos los componentes necesarios para implementar el sistema completo de monitoreo de salud del motor.

---

## 🔴 PRIORIDAD ALTA (Esenciales)

### 1. INA219 - Sensor de Corriente y Voltaje
- **Función**: Medir consumo de corriente y voltaje del motor
- **Especificaciones**:
  - Rango: 0-26V, ±3.2A
  - Interfaz: I2C (0x40)
  - Precisión: ±0.5%
  - Shunt resistor: 0.1Ω incluido
- **Precio aproximado**: €3-5
- **Dónde comprar**:
  - AliExpress: "INA219 current sensor module"
  - Amazon: "INA219 Arduino sensor"
  - Tiendas locales: Electrónica/robótica
- **Alternativa más barata**: ACS712 (analógico, menos preciso, €1-2)
- **Notas**: Fundamental para análisis de consumo. INA219 es muy superior al ACS712.

### 2. TCRT5000 - Sensor Óptico Infrarrojo (RPM)
- **Función**: Medir revoluciones por minuto del motor
- **Especificaciones**:
  - Tipo: IR LED + fototransistor
  - Distancia: 2-5mm
  - Salida: Digital + Analógica
  - Potenciómetro de sensibilidad incluido
- **Precio aproximado**: €1-2 (pack de 5: €3-4)
- **Dónde comprar**:
  - AliExpress: "TCRT5000 sensor module"
  - Amazon: "TCRT5000 infrared reflective sensor"
- **Alternativa**: Sensor Hall + imán (€2-3, requiere imán en eje)
- **Notas**: Necesitarás cinta reflectante blanca para marcar el eje del motor

### 3. ADXL345 - Acelerómetro 3 Ejes (Vibración)
- **Función**: Detectar vibraciones y desbalanceos del motor
- **Especificaciones**:
  - Rango: ±2g, ±4g, ±8g, ±16g (seleccionable)
  - Interfaz: I2C (0x53) o SPI
  - Resolución: 13-bit
  - Sensibilidad: Muy alta
- **Precio aproximado**: €2-4
- **Dónde comprar**:
  - AliExpress: "ADXL345 module GY-291"
  - Amazon: "ADXL345 accelerometer Arduino"
- **Alternativa**: MPU6050 (€2-3, incluye giroscopio, más datos pero más complejo)
- **Notas**: CRÍTICO que esté montado rígidamente, no en breadboard

---

## 🟡 PRIORIDAD MEDIA (Muy recomendados)

### 4. MLX90614 - Sensor de Temperatura Infrarrojo
- **Función**: Medir temperatura del motor sin contacto
- **Especificaciones**:
  - Rango: -70°C a +380°C
  - Interfaz: I2C (0x5A)
  - Precisión: ±0.5°C
  - Campo de visión: ~90° cono
  - Sin contacto (ideal para motores giratorios)
- **Precio aproximado**: €8-12
- **Dónde comprar**:
  - AliExpress: "MLX90614 infrared temperature sensor"
  - Amazon: "MLX90614ESF temperature sensor"
- **Alternativa BARATA**: DS18B20 (€2-3, pero requiere contacto, menos práctico)
- **Notas**: Más caro pero mucho mejor para motores. DS18B20 es backup aceptable.

### 5. DHT22 - Sensor de Temperatura y Humedad Ambiente
- **Función**: Medir temperatura ambiente y humedad
- **Especificaciones**:
  - Rango temperatura: -40°C a +80°C
  - Rango humedad: 0-100% RH
  - Precisión temp: ±0.5°C
  - Precisión humid: ±2-5% RH
  - Protocolo: OneWire (1 pin digital)
- **Precio aproximado**: €3-5
- **Dónde comprar**:
  - AliExpress: "DHT22 AM2302 sensor"
  - Amazon: "DHT22 temperature humidity sensor"
- **Alternativa**: BME280 (€4-6, I2C, incluye presión barométrica)
- **Notas**: Necesario para calcular Delta-T (temperatura motor - ambiente)

---

## 🟢 PRIORIDAD BAJA (Opcionales pero útiles)

### 6. KY-038 - Módulo de Micrófono (Sonido)
- **Función**: Detectar ruido acústico del motor
- **Especificaciones**:
  - Micrófono electret
  - Salida: Analógica + Digital (threshold)
  - Potenciómetro de sensibilidad
  - Rango frecuencia: 50Hz-10kHz típico
- **Precio aproximado**: €1-2
- **Dónde comprar**:
  - AliExpress: "KY-038 sound sensor module"
  - Amazon: "sound sensor Arduino"
- **Alternativa**: MAX9814 (€3-4, mejor calidad, AGC automático)
- **Notas**: No mide dB reales, solo valores relativos. Útil para detectar cambios.

---

## 🛠️ HARDWARE ADICIONAL NECESARIO

### Componentes de Montaje
- **Breadboard grande** (830 puntos): €3-5
  - O mejor: **PCB prototipo** soldable (€2-3) para montaje permanente
- **Plataforma de montaje rígida**: €0 (madera/plástico reciclado)
  - Para ADXL345: debe ser RÍGIDO, no breadboard
- **Cinta adhesiva de doble cara** (foam): €2-3
- **Tornillos/tuercas M3** (para fijar sensores): €2-3

### Arduino
- **Arduino MEGA 2560**: €8-15 (recomendado, más pines)
  - O mantener **Arduino UNO** (limitado a ~6 sensores simultáneos)
- **Cables Dupont** (M-M, M-F, F-F): €2-3 (si no tienes)
- **Resistencias pull-up 4.7kΩ** (2x para I2C): €0.50 (pack de 100)

### Display (Opcional pero muy útil)
- **OLED 0.96" I2C 128x64**: €3-5
  - Para visualizar datos en tiempo real sin PC
  - Dirección I2C: 0x3C (no conflicto con sensores)

### Almacenamiento de Datos (Opcional)
- **Módulo SD Card**: €2-3
  - Para logging de datos a largo plazo
  - Requiere SPI (pines D10-D13 en UNO)

### Otros
- **Cinta reflectante blanca**: €2-3 (para sensor RPM)
  - O marcador/pintura blanca
- **Fuente de alimentación externa 5V/2A**: €5-8 (si motor + sensores consumen >500mA)
- **Cables de protoboard de calidad**: €3-5 (mejoran fiabilidad)

---

## 💰 RESUMEN DE COSTOS

### Opción Mínima (Básica funcional)
| Componente | Precio |
|------------|--------|
| INA219 | €4 |
| TCRT5000 | €1 |
| ADXL345 | €3 |
| DHT22 | €4 |
| KY-038 | €1 |
| Cableado/montaje | €5 |
| **TOTAL** | **~€18** |

### Opción Completa (Con IR y display)
| Componente | Precio |
|------------|--------|
| INA219 | €4 |
| TCRT5000 | €1 |
| MLX90614 | €10 |
| DHT22 | €4 |
| ADXL345 | €3 |
| KY-038 | €1 |
| OLED Display | €4 |
| SD Module | €2 |
| Arduino MEGA | €12 |
| Montaje/cables | €8 |
| **TOTAL** | **~€49** |

### Opción Económica (Con alternativas baratas)
| Componente | Precio |
|------------|--------|
| INA219 | €4 |
| TCRT5000 | €1 |
| DS18B20 (temp contacto) | €2 |
| DHT22 | €4 |
| ADXL345 | €3 |
| KY-038 | €1 |
| Montaje básico | €3 |
| **TOTAL** | **~€18** |

---

## 🛒 ESTRATEGIA DE COMPRA RECOMENDADA

### Fase 1: Fundamentos (Empezar con esto)
1. **INA219** - Corriente es crítico para diagnóstico
2. **TCRT5000** - RPM es fundamental
3. **ADXL345** - Vibraciones revelan muchos problemas
4. **Cinta reflectante** - Para sensor RPM

**Costo Fase 1**: ~€10 | **Tiempo de entrega**: 2-3 semanas (AliExpress)

### Fase 2: Temperatura (Añadir después)
1. **DHT22** - Temperatura ambiente
2. **MLX90614** o **DS18B20** - Temperatura motor

**Costo Fase 2**: €6-14 (según elección)

### Fase 3: Extras (Si quieres completar)
1. **KY-038** - Sonido
2. **OLED Display** - Visualización
3. **SD Module** - Logging

**Costo Fase 3**: €7-8

---

## 🌐 ENLACES DE COMPRA RECOMENDADOS

### AliExpress (Más barato, envío lento 2-4 semanas)
```
Búsqueda: "Arduino sensor kit"
- Muchos vendedores ofrecen kits con 10-37 sensores por €15-30
- Incluyen: INA219, TCRT5000, ADXL345, DHT22, KY-038, etc.
- VENTAJA: Mejor precio, varios sensores
- DESVENTAJA: Envío lento, puede faltar MLX90614
```

### Amazon (Rápido, más caro)
```
Búsqueda: "Arduino sensor module kit"
- Kits de 37 sensores: €25-35
- Envío: 1-2 días (Prime)
- Calidad variable
```

### Tiendas Locales España (ejemplos)
- **Electan** (electan.es): Componentes individuales
- **BricoGeek** (bricogeek.com): Kits Arduino
- **PCComponentes**: Algunos sensores

---

## ⚠️ NOTAS IMPORTANTES

### Compatibilidad I2C
Todos los sensores I2C comparten los mismos pines (SDA/SCL). Asegúrate de que no hay conflictos de dirección:
- **0x40**: INA219 (puede cambiar a 0x41, 0x44, 0x45)
- **0x53**: ADXL345 (puede cambiar a 0x1D)
- **0x5A**: MLX90614 (fijo)
- **0x3C**: OLED Display (fijo)

### Pull-up Resistors I2C
La mayoría de módulos incluyen resistencias pull-up de 4.7kΩ. Si tienes problemas de comunicación I2C:
- Añade resistencias externas de 4.7kΩ entre SDA->5V y SCL->5V
- O reduce a 2.2kΩ si tienes muchos dispositivos

### Alimentación
- Sensores I2C consumen ~50mA total
- Motor ~60-90mA
- Arduino ~50mA
- **Total**: ~150-200mA (OK para USB, pero justo)
- **Recomendación**: Fuente externa 5V/1A si usas todos los sensores

### Calidad de Componentes
- **China/AliExpress**: Baratos pero 5-10% tasa de fallo
- **Compra 1-2 unidades extra** de componentes críticos (INA219, ADXL345)
- **Test individual** antes de integrar

---

## 📦 KIT RECOMENDADO PARA EMPEZAR

Si quieres empezar rápido, busca en AliExpress/Amazon:

**"37 in 1 Arduino Sensor Kit"** (~€25-30)

Incluye:
- ✅ TCRT5000 (RPM)
- ✅ ADXL345 (vibración)
- ✅ DHT22 (temp/humedad)
- ✅ Sound sensor
- ✅ OLED display (algunos kits)
- ✅ Muchos otros sensores

**FALTA en kit**: INA219, MLX90614
**Comprar aparte**:
- INA219: €4
- MLX90614: €10 (opcional)

**Total**: €40-45 con envío

---

## 🔧 HERRAMIENTAS NECESARIAS (Si no tienes)

- Soldador (€15-30) - Solo si haces PCB permanente
- Multímetro (€10-20) - Útil para debug
- Alicates de corte/punta (€5-10)
- Destornilladores pequeños (€5)

---

**Última actualización**: 2024-12-29
**Precios**: Aproximados, pueden variar según región/tienda
**Envío**: No incluido en precios (€2-8 típico)
