# Guía de Montaje del Photo Interrupter (IR Switch)

## Descripción General

El photo interrupter es un sensor infrarrojo de horquilla (fork-type) que detecta cuando un objeto pasa por su gap. Se usa con un disco codificador montado en el eje del motor para contar revoluciones completas.

## Materiales Necesarios

### Hardware Principal
- **Photo Interrupter** del kit de sensores Keyestudio
  - Tipo: Fork-type IR sensor
  - Componentes: LED IR emisor + Fototransistor receptor
  - Gap: 5-10mm (típico)
- **Disco codificador** (cartón, plástico o impresión 3D)
  - Diámetro: 20-40mm
  - Con 1 ranura de 3-5mm

### Materiales de Montaje
- Cartón grueso o plástico (para disco codificador)
- Pegamento de cianoacrilato o cinta de doble cara
- Soporte para el sensor (impreso 3D o cartón)
- Tijeras o cutter
- Regla y compás

### Conexión Eléctrica
- 3 cables Dupont macho-macho
- Breadboard (opcional)

## Especificaciones del Photo Interrupter

### Características Eléctricas

| Parámetro | Valor |
|-----------|-------|
| Voltaje operación | 3.3V - 5V |
| Corriente consumo | ~10-20 mA |
| Salida | Digital (HIGH/LOW) |
| Tipo salida | TTL compatible |
| Gap del sensor | 5-10mm |
| Longitud onda IR | ~940nm |

### Principio de Funcionamiento

```
Estado LIBRE (sin obstrucción):
    LED IR ─────→ [      ]  ─────→ Fototransistor
    (Emisor)      Gap libre    (Receptor detecta)
    Salida: LOW

Estado BLOQUEADO (con disco):
    LED IR ─────→ [██████]  ╳      Fototransistor
    (Emisor)    Disco bloquea  (No detecta)
    Salida: HIGH
```

### Conexiones

```
Photo Interrupter Module:
   ┌─────────────┐
   │   ┌─────┐   │
   │   │ IR  │   │  <- LED emisor (lado izquierdo)
   │   │     │   │
   │   │ Gap │   │  <- Gap (espacio para disco)
   │   │     │   │
   │   │ PT  │   │  <- Fototransistor (lado derecho)
   │   └─────┘   │
   │             │
   │ VCC GND OUT │
   └──┬───┬───┬──┘
      │   │   │
      │   │   └─── Arduino Pin 2 (con interrupción)
      │   └─────── Arduino GND
      └─────────── Arduino 5V
```

## Diseño del Disco Codificador

### Opción 1: Disco Simple (1 ranura)

**Propósito:** Contar revoluciones completas

```
Vista superior del disco:

        _______________
       /               \
      |   Motor Shaft   |  <- Centro (pegado al eje)
      |       ⊗         |
      |                 |
      |    ┌─────┐      |
      |    │ RAN │      |  <- Ranura (3-5mm ancho)
      |    │ URA │      |
      |    └─────┘      |
      |                 |
       \_______________/

Diámetro: 30mm (ejemplo)
Ranura: 4mm ancho × 15mm largo
```

**Precisión:** 1 pulso por revolución

### Opción 2: Disco Multi-Ranura (mayor resolución)

**Propósito:** Mayor resolución angular

```
Vista superior del disco (8 ranuras):

        _______________
       /  |  |  |  |   \
      |   Motor Shaft   |
      | | ⊗ | | | | | | |  <- 8 ranuras equidistantes
      |   |  |  |  |    |     (cada 45°)
       \__|__|__|__|___/

Ranuras: 8× ranuras de 2mm
Precisión: 8 pulsos/rev = resolución de 45°
```

**Cálculo de ranuras:**
- Más ranuras = mayor resolución
- Típico: 1, 4, 8, 16 o 20 ranuras
- Límite: ancho mínimo de ranura ~2mm

### Plantilla para Construcción

**Disco básico (30mm diámetro, 1 ranura):**

```
Pasos para crear el disco:

1. Dibujar círculo de 30mm diámetro
2. Marcar centro exacto
3. Dibujar ranura:
   - Distancia del centro: 10-12mm
   - Ancho: 4mm
   - Largo: 12-15mm (radial)
4. Recortar disco
5. Recortar ranura (usar cutter)
6. Hacer agujero central (diámetro del eje)
```

**Dimensiones críticas:**
- Disco centrado en el eje (±0.5mm)
- Ranura alineada radialmente
- Bordes de ranura limpios (sin rebabas)

## Montaje Físico

### Paso 1: Preparar el Disco Codificador

**Materiales recomendados:**
- **Cartón grueso:** 2-3mm (económico, fácil de cortar)
- **Plástico rígido:** CD viejo, acrílico (más duradero)
- **Impresión 3D:** PLA negro (profesional)

**Proceso:**

1. **Dibujar y recortar**
   ```
   - Usar compás para círculo perfecto
   - Marcar centro con precisión
   - Recortar con tijeras o cutter
   - Lijar bordes si es necesario
   ```

2. **Hacer ranura**
   ```
   - Marcar posición de la ranura
   - Usar regla para líneas rectas
   - Recortar con cutter afilado
   - Verificar que pase luz (probar con linterna)
   ```

3. **Hacer agujero central**
   ```
   - Diámetro = diámetro del eje motor (~5mm)
   - Centrar exactamente (usar compás)
   - Hacer agujero pequeño, expandir gradualmente
   - Debe ajustar firme pero no forzado
   ```

4. **Acabado**
   ```
   - Pintar de negro (opcional, mejora contraste)
   - Reforzar con cinta transparente (opcional)
   - Verificar que esté plano (sin deformaciones)
   ```

### Paso 2: Montar Disco en el Eje

**Alineación crítica:**

```
Vista lateral del eje:

    Eje del motor 28BYJ-48
    ┌──────────────┐
    │              │
    │    Motor     │
    │              │
    └──────┬───────┘
           │
           │ <- Eje (5mm diámetro)
           ⊙
           │
       ┌───┴───┐
       │ DISCO │ <- Disco codificador
       │   ⊗   │    (pegado al eje)
       └───────┘
```

**Procedimiento:**

1. **Limpiar eje**
   - Remover grasa con alcohol
   - Secar completamente

2. **Posicionar disco**
   - Insertar eje por agujero central
   - Disco perpendicular al eje (90°)
   - Dejar 2-3mm desde la carcasa del motor

3. **Pegar disco**
   - Aplicar gota pequeña de pegamento
   - Centrar visualmente
   - Presionar 10-15 segundos
   - Dejar curar 5 minutos

4. **Verificar centrado**
   - Girar motor manualmente
   - Observar si el disco wobble (balancea)
   - Si balancea: remover, limpiar y repetir

**Tolerancia:** Wobble <0.5mm es aceptable

### Paso 3: Posicionar el Sensor

**Ubicación del sensor:**

```
Vista lateral:

    ┌─────────────┐
    │ Soporte del │
    │   sensor    │
    ├─────────────┤
    │   ┌─────┐   │
    │   │ LED │   │ ← LED IR
    │   │     │   │
    │   │ Gap │   │ ← Disco pasa por aquí
    │   │     │   │
    │   │  PT │   │ ← Fototransistor
    │   └─────┘   │
    └─────────────┘
         │
         │
     ┌───┴───┐
     │ DISCO │ ← Disco atraviesa el gap
     │   █   │
     └───┬───┘
         │
    ┌────┴────┐
    │  Motor  │
    └─────────┘
```

**Alineación:**

1. **Altura vertical**
   - Disco debe estar centrado en el gap del sensor
   - Típicamente: mitad del disco alineado con mitad del gap

2. **Distancia radial**
   - El borde del disco pasa por el centro del gap
   - Disco no debe rozar el sensor

3. **Posición de ranura**
   - Ranura debe atravesar completamente el gap
   - Cuando la ranura alinea: sensor detecta paso de luz

**Métodos de montaje del sensor:**

#### Opción A: Soporte impreso 3D

```
Diseño simple:

    │
    ├─────┐
    │     │
    │  S  │ ← Sensor fijo aquí
    │     │
    └─────┤
    │
    ↓ (tornillo al breadboard/base)
```

#### Opción B: Soporte de cartón

```
Diseño en forma de "L":

    ┌──────┐
    │Sensor│ ← Pegar sensor aquí
    └──┬───┘
       │
       │ Altura ajustable
       │
    ┌──┴─────┐
    │  Base  │ ← Pegar a mesa/base
    └────────┘
```

#### Opción C: Pinza/abrazadera

- Usar clips de oficina doblados
- Fijar sensor con cinta adhesiva
- Ajustar posición manualmente

### Paso 4: Ajuste Fino

**Proceso de ajuste:**

1. **Conectar sensor a Arduino**
   ```
   VCC -> 5V
   GND -> GND
   OUT -> Pin 2
   ```

2. **Cargar sketch de verificación**
   ```
   // Ejecutar: 10_ir_sensor_verification.ino
   ```

3. **Girar motor manualmente**
   - Observar cambios en Serial Monitor
   - Debe detectar: LIBRE → BLOQUEADO → LIBRE

4. **Ajustar si no detecta**
   - Verificar disco atraviesa completamente el gap
   - Ajustar altura del sensor
   - Verificar ranura es lo suficientemente ancha

5. **Validar precisión**
   - Girar exactamente 10 revoluciones manualmente
   - Verificar que cuenta exactamente 10
   - Si no coincide: revisar debounce o ranuras extra

## Diagrama de Conexión

```
                    Arduino UNO
          ┌────────────────────────────┐
          │                            │
          │          5V  GND  Pin2     │
          │          │    │    │       │
          └──────────┼────┼────┼───────┘
                     │    │    │
                     │    │    │
              ┌──────┘    │    │
              │      ┌────┘    │
              │      │    ┌────┘
              │      │    │
         ┌────┴──────┴────┴────┐
         │  VCC   GND   OUT    │
         │  Photo Interrupter  │
         │   ┌─────────────┐   │
         │   │   ┌─────┐   │   │
         │   │   │ LED │   │   │
         │   │   │     │───┼───┼─ Disco pasa aquí
         │   │   │  PT │   │   │
         │   │   └─────┘   │   │
         │   └─────────────┘   │
         └─────────────────────┘
                  │
                  │
              ┌───▼───┐
              │ Disco │
              │   █   │
              └───┬───┘
                  │
             ┌────┴────┐
             │  Motor  │
             └─────────┘
```

## Solución de Problemas

### Problema: No detecta cambios

**Causas:**
1. Disco no pasa por el gap
2. Ranura muy pequeña o no atraviesa
3. Sensor desconectado

**Soluciones:**
- Verificar posición del disco respecto al gap
- Ensanchar ranura a 4-5mm
- Verificar conexiones VCC/GND/OUT

### Problema: Detecta múltiples veces por revolución

**Causas:**
1. Disco con múltiples ranuras no intencionales
2. Rebotes (bouncing) del sensor
3. Disco con grietas o transparencias

**Soluciones:**
- Inspeccionar disco, debe tener solo 1 ranura
- Aumentar tiempo de debounce en código
- Usar disco opaco (pintar de negro)

### Problema: No cuenta todas las revoluciones

**Causas:**
1. Ranura muy estrecha
2. Velocidad muy alta
3. Debounce muy largo

**Soluciones:**
- Ensanchar ranura a 4-5mm
- Reducir velocidad del motor (aumentar DELAY_MS)
- Reducir tiempo de debounce a 5-10ms

### Problema: Cuenta revoluciones extra

**Causas:**
1. Vibración del disco (wobble excesivo)
2. Rebotes electrónicos
3. Múltiples ranuras accidentales

**Soluciones:**
- Re-centrar y pegar disco firmemente
- Añadir capacitor de 100nF en OUT
- Verificar que disco tenga solo 1 ranura

## Mejoras Avanzadas

### Disco de Alta Precisión

**Características:**
- Material: Acrílico transparente con marcas opacas
- Precisión de corte: Laser cutter
- Múltiples ranuras equidistantes (8, 16 o 20)
- Resolución: 45°, 22.5° o 18° respectivamente

### Sensor Dual (Cuadratura)

**Uso de 2 sensores:**
```
Sensor A y Sensor B separados 90° mecánicos

Permite:
- Detección de dirección (CW vs CCW)
- Resolución ×4 (modo cuadratura)
- Mayor confiabilidad
```

### Filtrado por Hardware

**Circuito RC para debounce:**
```
OUT ────┬──── Arduino Pin 2
        │
       ┌┴┐
       │ │ 10kΩ
       └┬┘
        │
       ═╪═ 100nF
        │
       GND
```

## Checklist de Instalación

Antes de ejecutar sketch de calibración:

- [ ] Disco codificador construido (1 ranura, 30mm)
- [ ] Disco pegado al eje (centrado, sin wobble)
- [ ] Sensor montado en soporte estable
- [ ] Disco atraviesa completamente el gap del sensor
- [ ] Conexiones correctas (VCC, GND, OUT a Pin 2)
- [ ] Sketch 10 ejecutado exitosamente
- [ ] Sensor detecta cambios LIBRE ↔ BLOQUEADO
- [ ] Cuenta 10 revoluciones correctamente (prueba manual)
- [ ] Sin rebotes (conteo estable)

**¡Una vez completado, estás listo para sketch 11 de calibración automática!**

## Comparación: IR Switch vs AS5600

| Característica | Photo Interrupter | AS5600 (Magnético) |
|----------------|-------------------|--------------------|
| **Costo** | ~$1-2 | ~$3-8 |
| **Resolución** | 1 revolución (o N ranuras) | 4096 posiciones (0.088°) |
| **Precisión** | Conteo de vueltas | Ángulo absoluto |
| **Montaje** | Simple (disco + sensor) | Requiere imán + alineación |
| **Complejidad** | Muy simple | Media (I2C) |
| **Calibración** | Revoluciones completas | Grados/Minutos/Segundos |
| **Interferencia** | Sensible a luz ambiental | Inmune a luz, sensible a campos magnéticos |
| **Desgaste** | Sin contacto | Sin contacto |
| **Aplicación** | Conteo, velocidad | Posición angular precisa |

**Recomendación:**
- **Photo Interrupter:** Ideal para empezar, bajo costo, suficiente para calibración de steps/rev
- **AS5600:** Para aplicaciones que requieran posicionamiento angular preciso

---

**Documentado:** 2025-12-30
**Para uso con:** Sketches 10 y 11
**Kit:** Keyestudio Sensor Kit
