# Guía de Montaje del Sensor AS5600

## Descripción General

El AS5600 es un encoder magnético rotatorio de 12 bits que detecta la posición angular de un imán diametral. Para nuestro sistema de calibración automática, debe montarse en el eje del motor 28BYJ-48.

## Materiales Necesarios

### Hardware Principal
- **AS5600 Encoder Module** (placa breakout con pines)
- **Imán diametral** (incluido normalmente con el módulo)
  - Tamaño típico: 6mm diámetro × 2.5mm altura
  - Polo Norte hacia abajo (hacia el sensor)

### Materiales de Montaje
- Soporte impreso en 3D o plástico/acrílico
- Pegamento de cianoacrilato (super glue) o cinta adhesiva de doble cara
- Tornillos M2 o M3 (opcional, según el soporte)
- Espaciadores/arandelas para ajustar altura

### Herramientas
- Calibrador/regla (para medir distancia)
- Destornillador pequeño
- Marcador permanente (para marcar posiciones)

## Especificaciones Técnicas del AS5600

### Dimensiones del Sensor
- **Chip AS5600:** Centrado en la placa
- **Área sensible:** Circular, ~2mm de diámetro
- **Placa breakout:** Típicamente 15mm × 15mm

### Requisitos del Imán
- **Tipo:** Imán diametral (polarización norte-sur)
- **Diámetro:** 4mm - 8mm (óptimo: 6mm)
- **Altura:** 2mm - 3mm
- **Material:** Neodimio (NdFeB) grado N35 o superior
- **Polarización:** Diametral (no axial)

### Tolerancias de Montaje
- **Distancia vertical (air gap):** 0.5mm - 3.0mm
  - Óptimo: 1.0mm - 2.0mm
- **Desalineación lateral:** <0.5mm (centrado)
- **Inclinación máxima:** <5°

## Proceso de Montaje Paso a Paso

### Paso 1: Preparar el Eje del Motor

1. **Limpiar el eje**
   - Usar alcohol isopropílico para limpiar grasa/polvo
   - Secar completamente

2. **Marcar posición del imán**
   - El eje del 28BYJ-48 es plano en un lado
   - Marcar con lápiz el centro del eje

3. **Montar el imán**
   ```
   IMPORTANTE: Verificar polaridad
   - Usar brújula o sensor hall
   - Polo Norte hacia ABAJO (hacia el sensor)
   - Polo Sur hacia ARRIBA
   ```

4. **Pegar el imán**
   - Aplicar una gota pequeña de cianoacrilato
   - Centrar el imán en el eje
   - Presionar firmemente 10-15 segundos
   - Dejar curar 5 minutos

5. **Verificar centrado**
   - Girar el motor manualmente
   - Observar que no haya wobble (bamboleo)
   - Si hay wobble, remover y volver a pegar

### Paso 2: Diseñar/Construir Soporte

Opciones de soporte:

#### Opción A: Soporte Impreso 3D (Recomendado)

**Diseño del soporte:**
```
Vista lateral:
                    [Placa AS5600]
                    _______________
                   |               |
                   |   [  chip  ]  | <- Sensor
                   |_______________|
                          |
    Tornillos -> []===[]===[]===[] <- Ranuras de ajuste
                    |       |
                    |  1-2mm air gap
                    |       |
                  ----- ⊗ ----- <- Imán en eje
                    |_______|
                    |       |
                    | Motor |
                    |_______|
```

**Características del soporte:**
- Ranuras de ajuste vertical (para calibrar distancia)
- Agujeros para tornillos M2 o M3
- Apertura para el eje del motor
- Base que se fija al motor o breadboard

**Archivos STL:**
- (Pendiente: crear diseño en TinkerCAD/Fusion360)

#### Opción B: Soporte Manual (Temporal)

Materiales:
- Cartón rígido o acrílico de 2-3mm
- Cinta adhesiva de doble cara
- Clips o abrazaderas pequeñas

Pasos:
1. Cortar pieza en forma de "L"
2. Hacer agujero para el eje
3. Pegar el AS5600 en la parte horizontal
4. Fijar la parte vertical al motor o base
5. Ajustar altura con arandelas/espaciadores

### Paso 3: Posicionar el Sensor

1. **Alineación inicial**
   - Posicionar el sensor sobre el imán
   - Centrar visualmente (chip sobre imán)

2. **Ajustar distancia vertical**
   ```
   Método 1: Medición directa
   - Usar calibrador para medir air gap
   - Objetivo: 1.0-2.0mm

   Método 2: Prueba y error
   - Conectar AS5600 a Arduino
   - Ejecutar sketch 08_sensor_verification
   - Leer magnitud del campo magnético
   - Ajustar hasta magnitud = 1500-3000
   ```

3. **Fijar posición**
   - Una vez encontrada la distancia óptima
   - Apretar tornillos o pegar soporte
   - Verificar que no haya movimiento

### Paso 4: Verificación del Montaje

1. **Prueba de wobble**
   - Girar motor lentamente a mano
   - Observar con lupa si el imán se balancea
   - Tolerancia: <0.5mm de movimiento lateral

2. **Prueba eléctrica**
   - Cargar sketch `08_sensor_verification.ino`
   - Verificar:
     - ✓ Imán detectado (Magnet Detected)
     - ✓ Magnitud: 1500-3000 (óptimo)
     - ✓ Ángulo cambia al girar eje
     - ✓ No hay saltos bruscos en lectura

3. **Prueba de estabilidad**
   - Girar el eje 360° lentamente
   - Graficar magnitud vs ángulo
   - Debe ser relativamente constante (±20%)

## Diagrama de Montaje

### Vista Superior
```
       Motor 28BYJ-48 (vista desde arriba)

           _______________
          |               |
          |    [Cable]    |
          |       |       |
          |   +-----------+
          |   |  [⊗]     |   <- Eje con imán
          |   +-----------+
          |               |
          |___   ___   ___|
              | | | | |
              +-+-+-+-+
               Driver
```

### Vista Lateral
```
     AS5600 Module
    _______________
   |    [ chip ]   |
   |_______________|
   |               |
   +-------+-------+
   |       |       |
   |   Soporte     |
   |       |       |
   |   1-2mm gap   |
   |       |       |
     ----- ⊗ -----  <- Imán (polo N abajo)
        ___|___
       |       |
       | Motor |
       |_______|
```

## Solución de Problemas

### Problema: "Imán no detectado"

**Causas posibles:**
1. Distancia demasiado grande (>3mm)
   - **Solución:** Acercar sensor al imán

2. Polaridad invertida
   - **Solución:** Rotar imán 180° o voltear sensor

3. Imán no es diametral
   - **Solución:** Usar imán diametral correcto

4. Desalineación lateral
   - **Solución:** Centrar imán sobre chip

### Problema: "Magnitud muy baja" (<1000)

**Causas:**
- Distancia excesiva
- Imán débil o pequeño

**Soluciones:**
- Reducir air gap a 0.5-1.0mm
- Usar imán más grande o más fuerte

### Problema: "Magnitud muy alta" (>3500)

**Causas:**
- Distancia muy pequeña
- Imán demasiado fuerte

**Soluciones:**
- Aumentar air gap a 2.5-3.0mm
- Usar imán más pequeño o débil

### Problema: "Lecturas inestables o con saltos"

**Causas:**
1. Wobble del imán (no centrado)
2. Interferencia magnética externa
3. Vibración del montaje
4. Ruido eléctrico en I2C

**Soluciones:**
1. Re-centrar y pegar imán nuevamente
2. Alejar de motores/imanes cercanos
3. Reforzar soporte mecánico
4. Añadir resistencias pull-up I2C (4.7kΩ)
5. Usar cables I2C cortos (<20cm)

### Problema: "Ángulo no cambia al girar"

**Causas:**
- Imán no está montado en eje giratorio
- Sensor está girando con el imán

**Soluciones:**
- Verificar que imán esté pegado al eje
- Verificar que sensor esté fijo al chasis

## Mantenimiento

### Limpieza
- **Frecuencia:** Cada 6 meses o según necesidad
- **Método:**
  - Desconectar alimentación
  - Limpiar con aire comprimido
  - No tocar el chip AS5600
  - No usar líquidos cerca del sensor

### Verificación Periódica
- Ejecutar `08_sensor_verification.ino`
- Comparar magnitud con valor inicial
- Si varía >20%, revisar montaje

### Re-calibración
- Si se desmonta el imán o sensor
- Después de golpes o vibraciones fuertes
- Si las lecturas se degradan

## Especificaciones de Calidad

### Criterios de Aceptación

| Parámetro | Valor Óptimo | Rango Aceptable |
|-----------|--------------|-----------------|
| Air gap | 1.5mm | 0.5-3.0mm |
| Magnitud | 2000-2500 | 1500-3000 |
| Desalineación lateral | 0mm | <0.5mm |
| Wobble del imán | 0mm | <0.3mm |
| Estabilidad lectura | ±0.1° | ±0.5° |

### Verificación de Instalación Exitosa

✓ Imán detectado (MD bit = 1)
✓ Magnitud en rango (1500-3000)
✓ No hay advertencias MH/ML
✓ Ángulo cambia suavemente 0-360°
✓ Lectura estable (sin saltos)
✓ AGC value < 200

## Recursos Adicionales

### Datasheets
- AS5600 Datasheet: https://ams.com/as5600
- Application Note AN5600-01: Magnet Selection Guide

### Tutoriales
- SparkFun AS5600 Hookup Guide
- Adafruit Magnetic Encoder Tutorial

### Diseños 3D
- Thingiverse: "AS5600 motor mount"
- Printables: "Stepper motor encoder bracket"

## Checklist Final

Antes de proceder al sketch de calibración automática:

- [ ] Imán pegado firmemente en el eje
- [ ] Polaridad verificada (N hacia sensor)
- [ ] Sensor montado en soporte estable
- [ ] Air gap medido: 1-2mm
- [ ] Sketch 08 ejecutado exitosamente
- [ ] Magnitud: 1500-3000
- [ ] Sin wobble visible
- [ ] Ángulo cambia suavemente al girar
- [ ] Lectura estable sin saltos

**¡Una vez completado, estás listo para ejecutar el sketch 07 de calibración automática!**
