# Configuración de Photo Interrupter con 36 Pulsos/Revolución

## Descripción General

Tu sensor IR está configurado para generar **36 pulsos por cada revolución completa** del motor. Esto proporciona una resolución angular de **10 grados por pulso**, lo cual es excelente para calibración de precisión.

## Especificaciones de tu Configuración

### Resolución Angular

| Parámetro | Valor |
|-----------|-------|
| Pulsos por revolución | 36 |
| Grados por pulso | 10° |
| Minutos por pulso | 600' |
| Segundos por pulso | 36000" |

### Comparación de Resoluciones

```text
Configuración Simple (1 ranura):
  360° / 1 = 360° por pulso
  Resolución: Solo cuenta revoluciones completas

Tu Configuración (36 ranuras):
  360° / 36 = 10° por pulso
  Resolución: Mide posición cada 10 grados
  ¡36 veces más preciso!
```

## Ventajas de 36 Pulsos/Revolución

### 1. Mayor Precisión de Calibración

**Error medible:**
- Con 1 pulso/rev: Solo detectas error de revoluciones completas
- Con 36 pulsos/rev: Detectas error con precisión de 10°

**Ejemplo:**
```text
Motor ejecuta 4097 pasos (esperamos 1 revolución):

Con 1 pulso/rev:
  - Si detecta 1 pulso → "perfecto"
  - Si detecta 0 o 2 pulsos → error grande
  - Resolución: ±360°

Con 36 pulsos/rev:
  - Si detecta 36 pulsos → perfecto
  - Si detecta 35 pulsos → error de -10°
  - Si detecta 37 pulsos → error de +10°
  - Resolución: ±10°
```

### 2. Calibración Más Rápida

Con mayor resolución, puedes obtener resultados precisos con menos revoluciones:

| Pulsos/Rev | Revoluciones Necesarias | Tiempo |
|------------|------------------------|--------|
| 1 | 50-100 | ~5-10 min |
| 36 | 10-20 | ~1-2 min |

### 3. Detección de Errores Pequeños

Puedes detectar y corregir errores de pocos grados:

```text
Error de 5 grados en 1 revolución:

Con 1 pulso/rev:
  - No detectable (< 360°)
  - Necesitas muchas revoluciones para acumular error

Con 36 pulsos/rev:
  - Detectable como 0.5 pulsos de diferencia
  - En 2 revoluciones ya es 1 pulso de error
```

## Implementación del Disco Codificador

### Diseño del Disco con 36 Ranuras

```text
Vista superior (simplificada):

        _______________
       /||||||||||||||||\
      ||||||| ⊗ |||||||||  <- 36 ranuras equiespaciadas
      |||||||||||||||||||     cada 10 grados
       \|||||||||||||||/
```

**Especificaciones:**
- **Número de ranuras:** 36
- **Espaciado angular:** 10° entre cada ranura
- **Ancho de ranura:** ~2-3mm
- **Ancho de parte sólida:** ~2-3mm
- **Diámetro típico:** 40-60mm

### Métodos de Fabricación

#### Opción 1: Encoder Comercial (Recomendado)

**Tipo:** Encoder óptico incremental 36 PPR

**Características:**
- Disco pre-fabricado con 36 ranuras
- Alta precisión
- Incluye sensor montado
- Costo: $5-15 USD

**Ejemplos:**
- HEDS-5500 (Avago/Broadcom)
- E6B2-CWZ6C (Omron) - 360 PPR (ajustable)
- Encoder genérico 36 PPR de AliExpress

#### Opción 2: Disco Impreso (DIY)

**Proceso:**
1. Descargar plantilla de disco codificador
2. Imprimir en papel fotográfico o acetato
3. Pegar en cartón/acrílico rígido
4. Recortar y montar

**Plantilla:**
```
Archivo: encoder_disk_36_slots.svg
Software: Inkscape, Adobe Illustrator
Formato: SVG vectorial para máxima calidad
```

#### Opción 3: Corte Láser (Profesional)

**Material:** Acrílico negro 3mm

**Especificaciones:**
- Precisión: ±0.1mm
- Ranuras: 36 × 2.5mm ancho
- Diámetro externo: 50mm
- Agujero central: 5mm (eje del motor)

**Costo:** $5-10 por disco en servicio de corte láser

## Cálculos de Calibración

### Fórmula Básica

```text
STEPS_PER_REVOLUTION = (Pasos Ejecutados) / (Pulsos Detectados / 36)

Ejemplo:
  Ejecutamos: 40970 pasos (objetivo: 10 revoluciones)
  Detectamos: 361 pulsos

  Revoluciones reales = 361 / 36 = 10.0278 rev
  STEPS_PER_REV = 40970 / 10.0278 = 4086 pasos/rev
```

### Precisión Alcanzable

**Error mínimo detectable:**
```text
1 pulso de diferencia en 36 pulsos/rev = 1/36 rev = 0.0278 rev = 10°

En términos de pasos:
  4097 pasos/rev × (1/36) = ±114 pasos de error detectable
```

**Precisión de calibración:**
```text
Con 10 revoluciones (360 pulsos):
  Error de ±1 pulso = ±0.28% de error
  Error de ±2 pulsos = ±0.56% de error

Precisión esperada: ±1-5 pasos en STEPS_PER_REVOLUTION
```

## Configuración del Sketch

Los sketches han sido actualizados con:

```cpp
// Configuración del Sensor
const int PULSES_PER_REVOLUTION = 36;  // 36 pulsos por vuelta
const float DEGREES_PER_PULSE = 360.0 / 36;  // = 10° por pulso
```

### Cambios en el Algoritmo

**Antes (1 pulso/rev):**
```cpp
// Solo contaba revoluciones completas
revolutionCount++;
actualRevolutions = (float)revolutionCount;
```

**Ahora (36 pulsos/rev):**
```cpp
// Cuenta pulsos y calcula revoluciones con precisión
pulseCount++;
actualRevolutions = (float)pulseCount / PULSES_PER_REVOLUTION;
// Ejemplo: 361 pulsos = 10.0278 revoluciones
```

## Resultados de Calibración Esperados

### Formato de Salida Mejorado

```text
========================================
RESUMEN DE PRUEBAS
========================================

Rev  Pulsos  Detect  Rev Real  Error°    Error%    Pasos/Rev
---  ------  ------  --------  ------    ------    ---------
1    36      36      1.000     +0.0°     +0.00%    4097
2    72      72      2.000     +0.0°     +0.00%    4097
5    180     181     5.028     +10.0°    +0.56%    4085
10   360     361     10.028    +10.0°    +0.28%    4086
20   720     722     20.056    +20.0°    +0.28%    4086

ANALISIS ESTADISTICO
Error promedio: +0.28%
Desviacion estandar: 0.12%
Pasos recomendados: 4086
```

### Interpretación

**Error positivo (+10°):**
- Motor gira 10° DE MÁS por cada ciclo de prueba
- El valor de STEPS_PER_REVOLUTION es muy BAJO
- Necesitas AUMENTAR los pasos

**Error negativo (-10°):**
- Motor gira 10° DE MENOS
- El valor de STEPS_PER_REVOLUTION es muy ALTO
- Necesitas REDUCIR los pasos

## Ventajas vs Otras Configuraciones

| Configuración | Resolución | Costo | Complejidad | Precisión Calibración |
|---------------|------------|-------|-------------|-----------------------|
| 1 ranura | 360° | $0 | Muy simple | ±10-50 pasos |
| 36 ranuras | 10° | $0-15 | Simple | ±1-5 pasos |
| AS5600 | 0.088° | $3-8 | Media | ±0.5 pasos |
| Encoder 600 PPR | 0.6° | $15-30 | Alta | ±0.2 pasos |

## Aplicaciones Adicionales

Con 36 pulsos/rev, tu sistema puede:

### 1. Medición de Velocidad Precisa

```cpp
float rpm = 60000000.0 / (interval_us * 36);
// Actualización cada 10° en lugar de cada 360°
```

### 2. Control de Posición

```cpp
int targetPosition = 180;  // grados
int targetPulses = targetPosition / 10;  // 18 pulsos
// Detener cuando pulseCount alcance targetPulses
```

### 3. Detección de Pérdida de Pasos

```cpp
long expectedPulses = (steps / STEPS_PER_REV) * 36;
if (abs(pulseCount - expectedPulses) > 2) {
  Serial.println("ADVERTENCIA: Pasos perdidos detectados!");
}
```

## Troubleshooting

### Problema: No detecta 36 pulsos exactos

**Posibles causas:**
1. Disco desalineado o con wobble
2. Algunas ranuras bloqueadas o no claras
3. Velocidad demasiado alta (rebotes)

**Soluciones:**
- Re-centrar disco en el eje
- Limpiar ranuras (luz debe pasar libremente)
- Reducir velocidad del motor (aumentar DELAY_MS)

### Problema: Cuenta más de 36 pulsos/rev

**Causas:**
- Rebotes del sensor (bouncing)
- Ranuras irregulares

**Soluciones:**
- Aumentar tiempo de debounce (actualmente 2ms)
- Usar disco de mejor calidad
- Añadir filtro capacitivo (100nF)

### Problema: Cuenta menos de 36 pulsos/rev

**Causas:**
- Algunas ranuras no se detectan
- Velocidad muy alta
- Mala alineación

**Soluciones:**
- Verificar que todas las ranuras sean uniformes
- Reducir velocidad del motor
- Ajustar posición del sensor

## Conclusión

Tu configuración de **36 pulsos por revolución** es excelente porque:

✓ **36× más precisa** que disco simple (1 ranura)
✓ **Suficiente resolución** para calibración de alta calidad (±1-5 pasos)
✓ **Económica** comparada con encoders de alta resolución
✓ **Fácil de implementar** con el código actualizado
✓ **Ideal para aprendizaje** - balance perfecto entre precisión y simplicidad

Esta es una configuración **profesional** que te permitirá calibrar tu motor con la misma precisión que sistemas comerciales, pero a una fracción del costo.

---

**Actualizado:** 2025-12-30
**Configuración:** 36 pulsos/revolución
**Sketches actualizados:** 10 y 11 ✓
