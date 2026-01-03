# Diagrama de Conexiones - AS5600 + Motor 28BYJ-48

## Descripción General

Este documento describe todas las conexiones eléctricas necesarias para el sistema de calibración automática con sensor AS5600.

## Componentes del Sistema

1. **Arduino UNO** (o ESP32)
2. **Motor 28BYJ-48** (5V stepper motor)
3. **Driver ULN2003APG** (darlington array)
4. **Sensor AS5600** (magnetic encoder I2C)
5. **Fuente de alimentación** 5V (opcional, puede usar USB)

## Tabla de Conexiones

### Motor 28BYJ-48 → Driver ULN2003

| Motor (Cable) | Color  | Driver ULN2003 |
|---------------|--------|----------------|
| Pin 1         | Naranja| IN1            |
| Pin 2         | Rosa   | IN2            |
| Pin 3         | Amarillo| IN3           |
| Pin 4         | Azul   | IN4            |
| Pin 5 (común) | Rojo   | +5V (opcional)|

**Nota:** El cable rojo puede conectarse al +5V del driver o dejarse desconectado (el motor funciona igual).

### Driver ULN2003 → Arduino UNO

| Driver ULN2003 | Arduino UNO |
|----------------|-------------|
| IN1            | Pin 8       |
| IN2            | Pin 9       |
| IN3            | Pin 10      |
| IN4            | Pin 11      |
| + (VCC)        | 5V          |
| - (GND)        | GND         |

### Sensor AS5600 → Arduino UNO

| AS5600 | Arduino UNO | Descripción |
|--------|-------------|-------------|
| VCC    | 5V          | Alimentación 3.3V-5V |
| GND    | GND         | Tierra |
| SDA    | A4 (SDA)    | I2C Data |
| SCL    | A5 (SCL)    | I2C Clock |
| DIR    | GND o VCC   | Dirección de conteo (opcional) |
| PGO    | N/C         | Programming output (no conectar) |

**Notas importantes:**
- El AS5600 acepta tanto 3.3V como 5V
- SDA/SCL ya tienen pull-ups internos en el AS5600 (normalmente no necesitas añadir resistencias)
- Si hay problemas de comunicación I2C, añadir pull-ups de 4.7kΩ a VCC

### Conexiones para ESP32 (Alternativa)

Si usas ESP32 en lugar de Arduino UNO:

| AS5600 | ESP32      | Driver | ESP32 |
|--------|------------|--------|-------|
| VCC    | 3.3V       | IN1    | GPIO16|
| GND    | GND        | IN2    | GPIO17|
| SDA    | GPIO21     | IN3    | GPIO18|
| SCL    | GPIO22     | IN4    | GPIO19|

**Nota:** ESP32 trabaja a 3.3V, así que alimenta el AS5600 con 3.3V (no con 5V).

## Diagrama de Conexión Completo

```
                                 SISTEMA COMPLETO

                                 Arduino UNO
                        ┌────────────────────────────┐
                        │                            │
                    USB │         Vin  GND  5V  3.3V│
                    ────│         │    │    │    │  │
                        │         │    │    │    │  │
                        │         │    ├────┼────┘  │
                        │         │    │    │       │
                        │  D13 ●──┘    │    │       │
                        │  D12 ●       │    │       │
                        │  D11 ●───────┼────┼──────────────┐
                        │  D10 ●───────┼────┼──────────┐   │
                        │   D9 ●───────┼────┼───────┐  │   │
                        │   D8 ●───────┼────┼────┐  │  │   │
                        │              │    │    │  │  │   │
                        │   D7 ●       │    │    │  │  │   │
                        │   D6 ●       │    │    │  │  │   │
                        │   D5 ●       │    │    │  │  │   │
                        │   D4 ●       │    │    │  │  │   │
                        │   D3 ●       │    │    │  │  │   │
                        │   D2 ●       │    │    │  │  │   │
                        │   D1 ●       │    │    │  │  │   │
                        │   D0 ●       │    │    │  │  │   │
                        │              │    │    │  │  │   │
                        │   A5 ●───────┼────┼────┼──┼──┼───┼──┐
                        │   A4 ●───────┼────┼────┼──┼──┼───┼──┼──┐
                        │   A3 ●       │    │    │  │  │   │  │  │
                        │   A2 ●       │    │    │  │  │   │  │  │
                        │   A1 ●       │    │    │  │  │   │  │  │
                        │   A0 ●       │    │    │  │  │   │  │  │
                        │              │    │    │  │  │   │  │  │
                        │          GND ●────┴────┴──┴──┴───┴──┴──┴──┐
                        │          5V  ●────────────────────────┐   │
                        └────────────────────────────────────┐  │   │
                                                             │  │   │
                                                             │  │   │
     ┌───────────────────────────────────────────────┐      │  │   │
     │         Driver ULN2003APG                     │      │  │   │
     │   ┌────────────────────────────────────┐      │      │  │   │
     │   │  IN1  IN2  IN3  IN4   +   -  (LED)│      │      │  │   │
     │   │   │    │    │    │    │   │       │      │      │  │   │
     │   └───┼────┼────┼────┼────┼───┼───────┘      │      │  │   │
     │       │    │    │    │    │   │              │      │  │   │
     │  ┌────┘    │    │    │    │   │              │      │  │   │
     │  │    ┌────┘    │    │    │   │              │      │  │   │
     │  │    │    ┌────┘    │    │   │              │      │  │   │
     │  │    │    │    ┌────┘    │   │              │      │  │   │
     │  │    │    │    │    ┌────┘   └──────────────┼──────┘  │   │
     │  │    │    │    │    │                       │         │   │
     │  1    2    3    4    └───────────────────────┼─────────┘   │
     │  │    │    │    │                            │             │
     │  │    │    │    │                            │             │
     │  │    │    │    │     OUT1-4 Connector       │             │
     │  └┬───┴┬───┴┬───┴┬────────────────────┐      │             │
     │   │    │    │    │                    │      │             │
     └───┼────┼────┼────┼────────────────────┘      │             │
         │    │    │    │                           │             │
         │    │    │    │                           │             │
         │    │    │    │                           │             │
    ┌────┼────┼────┼────┼────────────┐              │             │
    │    │    │    │    │  Motor     │              │             │
    │    │    │    │    │  28BYJ-48  │              │             │
    │    O    R    Y    B    R        │              │             │
    │    │    │    │    │    │        │              │             │
    │   Ora  Ros Ama  Azu Roj        │              │             │
    │    │    │    │    │    │        │              │             │
    │    1    2    3    4    5        │              │             │
    │    (común - no conectado)       │              │             │
    │                                 │              │             │
    │         [⊗ Imán en eje]         │              │             │
    │                                 │              │             │
    └─────────────────────────────────┘              │             │
                   │                                 │             │
                   │ 1-2mm air gap                   │             │
                   ▼                                 │             │
         ┌─────────────────┐                         │             │
         │   AS5600 Module │                         │             │
         │  ┌───────────┐  │                         │             │
         │  │   CHIP    │  │                         │             │
         │  └───────────┘  │                         │             │
         │                 │                         │             │
         │ VCC GND SDA SCL │                         │             │
         │  │   │   │   │  │                         │             │
         └──┼───┼───┼───┼──┘                         │             │
            │   │   │   │                            │             │
            │   └───┼───┼────────────────────────────┘             │
            │       │   │                                          │
            │       └───┼──────────────────────────────────────────┘
            │           │
            └───────────┘

Leyenda:
O = Naranja, R = Rosa, Y = Amarillo, B = Azul, Roj = Rojo
● = Pin Arduino
┌─┐ = Componente
─── = Cable/conexión
```

## Esquema Simplificado por Bloques

```
┌─────────┐
│  USB    │ 5V/GND
└────┬────┘
     │
     │ USB Cable
     │
┌────▼──────────┐
│  Arduino UNO  │
│               │
│  D8-D11 ─────────────┐
│  A4(SDA)────────┐    │
│  A5(SCL)─────┐  │    │
│  5V  ────┐   │  │    │
│  GND ──┐ │   │  │    │
└────────┼─┼───┼──┼────┘
         │ │   │  │
         │ │   │  │
    ┌────┘ └───┼──┼────────┐
    │    ┌─────┘  └─────┐  │
    │    │              │  │
    │    │    I2C       │  │
    │    │    Bus       │  │
    │    │              │  │
┌───▼────▼───┐      ┌───▼──▼─────┐
│  AS5600    │      │ ULN2003    │
│  Sensor    │      │ Driver     │
│            │      │            │
│   [Chip]   │      │ OUT1-4 ────┼──┐
└────────────┘      └────────────┘  │
     │ 1-2mm gap                    │
     ▼                              │
   [⊗ Imán]                         │
     │                              │
     │ En eje                       │
     │                              │
┌────▼──────────────────────────────▼───┐
│        Motor 28BYJ-48                 │
│    5 cables (Naranja, Rosa,           │
│     Amarillo, Azul, Rojo)             │
└───────────────────────────────────────┘
```

## Lista de Verificación de Conexiones

Antes de encender el sistema, verificar:

### Alimentación
- [ ] Arduino conectado a USB o fuente externa
- [ ] Driver ULN2003 conectado a 5V y GND de Arduino
- [ ] AS5600 conectado a 5V y GND de Arduino
- [ ] **NO conectar** múltiples fuentes de 5V simultáneamente

### Bus I2C
- [ ] SDA del AS5600 → A4 del Arduino
- [ ] SCL del AS5600 → A5 del Arduino
- [ ] Cables I2C cortos (<20cm) para reducir ruido
- [ ] Si hay problemas: añadir pull-ups 4.7kΩ

### Motor
- [ ] IN1 (Pin 8) → Cable Naranja
- [ ] IN2 (Pin 9) → Cable Rosa
- [ ] IN3 (Pin 10) → Cable Amarillo
- [ ] IN4 (Pin 11) → Cable Azul
- [ ] Cable Rojo: no conectado o a +5V del driver

### Tierras Comunes
- [ ] GND de Arduino, Driver y AS5600 todos conectados
- [ ] **CRÍTICO:** Tierra común es esencial para I2C

### Montaje Físico
- [ ] Imán montado en eje del motor
- [ ] Sensor AS5600 a 1-2mm del imán
- [ ] Sensor fijo (no gira con el motor)
- [ ] Sin interferencias magnéticas cercanas

## Características Técnicas I2C

### Protocolo I2C del AS5600
- **Dirección I2C:** 0x36 (7-bit)
- **Velocidad:** Hasta 1 MHz (usamos 400 kHz)
- **Pull-ups:** Internos en AS5600 (típicamente 10kΩ)
- **Nivel lógico:** Compatible con 3.3V y 5V

### Configuración Arduino
```cpp
Wire.begin();           // Inicializar I2C
Wire.setClock(400000);  // 400 kHz Fast Mode
```

### Resistencias Pull-up (si necesarias)

Si experimentas problemas de comunicación I2C:

```
      5V
       │
      ┌┴┐
      │ │ 4.7kΩ
      └┬┘
       │
SDA ───┴───────────

      5V
       │
      ┌┴┐
      │ │ 4.7kΩ
      └┬┘
       │
SCL ───┴───────────
```

**Ubicación:** Lo más cerca posible de los pines SDA/SCL

## Problemas Comunes y Soluciones

### No se detecta el AS5600 (Error I2C)

**Síntomas:**
- Sketch reporta "AS5600 no detectado"
- Error code I2C: 2 o 4

**Soluciones:**
1. Verificar conexiones SDA/SCL (no invertidas)
2. Verificar alimentación (VCC y GND correctos)
3. Probar sketch I2C scanner:
   ```cpp
   // Sketch para escanear dispositivos I2C
   Wire.begin();
   for(byte i = 0; i < 128; i++) {
     Wire.beginTransmission(i);
     if(Wire.endTransmission() == 0) {
       Serial.print("Dispositivo encontrado: 0x");
       Serial.println(i, HEX);
     }
   }
   ```
4. Añadir pull-ups de 4.7kΩ en SDA y SCL

### Motor no gira o gira errático

**Síntomas:**
- Motor vibra pero no gira
- Giro irregular o con saltos

**Soluciones:**
1. Verificar orden de conexión IN1-IN4
2. Verificar secuencia half-step en código
3. Aumentar DELAY_MS (probar con 5ms)
4. Verificar alimentación del driver (5V estable)

### Lecturas del AS5600 inestables

**Síntomas:**
- Ángulo salta aleatoriamente
- Magnitud varía mucho

**Soluciones:**
1. Reducir longitud de cables I2C
2. Alejar cables I2C de cables del motor
3. Añadir capacitor 100nF entre VCC-GND del AS5600
4. Verificar que imán no haga wobble

## Alimentación y Consumo

### Consumo Típico

| Componente | Corriente Típica | Corriente Máxima |
|------------|------------------|------------------|
| Arduino UNO| 50 mA           | 200 mA           |
| AS5600     | 6.5 mA          | 10 mA            |
| Motor idle | 0 mA            | 0 mA             |
| Motor activo| 120 mA         | 240 mA           |
| **TOTAL**  | **~180 mA**     | **~450 mA**      |

### Opciones de Alimentación

#### Opción 1: Solo USB (Recomendado para pruebas)
- **Capacidad:** 500 mA (USB 2.0)
- **Suficiente para:** Arduino + AS5600 + Motor
- **Limitación:** Motor a velocidad baja-media

#### Opción 2: Fuente Externa
- **Especificaciones:** 5V, 1A mínimo
- **Conexión:** Barrel jack o Vin del Arduino
- **Ventaja:** Motor puede funcionar a velocidad máxima

**ADVERTENCIA:** Si usas fuente externa:
- NO conectar USB y fuente externa simultáneamente
- Usar fuente regulada de 5V (no más de 5.5V)

## Cableado Recomendado

### Tipos de Cable

- **I2C (SDA/SCL):** Cable duplex 22-24 AWG, <30cm
- **Motor (IN1-IN4):** Cable 24-26 AWG
- **Alimentación:** Cable 20-22 AWG

### Código de Colores Sugerido

- **5V:** Rojo
- **GND:** Negro
- **SDA:** Amarillo o Verde
- **SCL:** Naranja o Azul
- **IN1-IN4:** Según preferencia (mantener orden)

### Organización

1. Agrupar cables I2C juntos
2. Separar cables del motor de cables I2C
3. Usar organizadores o bridas
4. Etiquetar conexiones

## Prueba del Sistema Completo

### Secuencia de Verificación

1. **Verificación visual:**
   - [ ] Revisar todas las conexiones
   - [ ] Verificar polaridades (VCC/GND)
   - [ ] Asegurar que no hay cortocircuitos

2. **Primera energización:**
   - [ ] Conectar Arduino a USB
   - [ ] Verificar LED de alimentación en Arduino
   - [ ] Verificar LED en driver ULN2003 (si tiene)

3. **Prueba del sensor:**
   - [ ] Cargar sketch `08_sensor_verification.ino`
   - [ ] Abrir Serial Monitor (115200 baud)
   - [ ] Verificar detección del AS5600
   - [ ] Girar eje manualmente, observar cambio de ángulo

4. **Prueba del motor:**
   - [ ] Cargar sketch `01_basic_stepper_test.ino`
   - [ ] Verificar que motor gira suavemente
   - [ ] Verificar dirección correcta

5. **Prueba integrada:**
   - [ ] Cargar sketch `07_auto_calibration_system.ino`
   - [ ] Ejecutar batería de pruebas
   - [ ] Verificar lecturas coherentes

**¡Una vez completadas estas verificaciones, el sistema está listo para calibración automática!**
