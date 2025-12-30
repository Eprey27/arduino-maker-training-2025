# Resultados de Pruebas - Motor 28BYJ-48

## Información del Experimento

**Fecha**: _________________
**Motor**: 28BYJ-48 (Serial: 20563200)
**Driver**: ULN2003APG
**Placa**: Arduino UNO / ESP32 (indicar cuál)
**Voltaje del jumper**: 5V / 12V (indicar configuración)

---

## Experimento 1: Control Básico (01_basic_stepper_test)

### Observaciones Visuales
- [ ] Los 4 LEDs se encienden en secuencia ordenada
- [ ] Se ven patrones donde 1 o 2 LEDs están encendidos simultáneamente
- [ ] El motor gira suavemente sin saltos bruscos
- [ ] La rotación es continua y estable

### Funcionamiento
**Delay utilizado**: _____ ms
**¿El motor completó una revolución completa?**: Sí / No
**¿El motor regresó a la posición inicial después de 2048 pasos?**: Sí / No

### Observaciones de Sonido
**Describe el sonido del motor**:
- Con delay de 2ms: _______________________________________________
- Con delay de 5ms: _______________________________________________
- Con delay de 10ms: ______________________________________________

### Notas Adicionales
```
(Anota cualquier comportamiento inusual, problemas encontrados, etc.)






```

---

## Experimento 2: Librería Stepper (02_stepper_library)

### Comparación con Experimento 1
**¿El comportamiento es similar al experimento 1?**: Sí / No
**¿Cuál método prefieres?**: Manual / Librería
**¿Por qué?**: ____________________________________________________

### Tiempos de Ejecución

| Velocidad (RPM) | Tiempo Teórico | Tiempo Real | Diferencia |
|-----------------|----------------|-------------|------------|
| 5 RPM           | 12.0 s         | ______ s    | ______ s   |
| 10 RPM          | 6.0 s          | ______ s    | ______ s   |
| 15 RPM          | 4.0 s          | ______ s    | ______ s   |

### Holding Torque
**¿El motor mantiene su posición cuando está detenido?**: Sí / No
**¿Puedes girar el eje manualmente con facilidad?**: Sí / No
**Describe la resistencia que sientes**: _____________________________

### Notas Adicionales
```






```

---

## Experimento 3: Mediciones (03_stepper_measurement)

### Test 1: Velocidades Diferentes

| Delay (ms) | RPM Teórico | RPM Real | Tiempo (s) | ¿Perdió pasos? |
|------------|-------------|----------|------------|----------------|
| 10         |             |          |            | Sí / No        |
| 5          |             |          |            | Sí / No        |
| 3          |             |          |            | Sí / No        |
| 2          |             |          |            | Sí / No        |
| 1          |             |          |            | Sí / No        |

**Velocidad óptima identificada**: ______ ms de delay (______ RPM)
**Velocidad máxima sin perder pasos**: ______ ms de delay (______ RPM)

### Test 2: Precisión Angular

| Ángulo Objetivo | Ángulo Real (visual) | Precisión |
|-----------------|----------------------|-----------|
| 45°             | ≈ ______°           | Buena / Regular / Mala |
| 90°             | ≈ ______°           | Buena / Regular / Mala |
| 180°            | ≈ ______°           | Buena / Regular / Mala |
| 360°            | ≈ ______°           | Buena / Regular / Mala |

**¿Después de todos los movimientos, regresó a la posición inicial?**: Sí / No
**Error estimado**: ± ______°

### Test 3: Reversibilidad

**Número de ciclos completados**: ______
**¿El motor regresó a la posición inicial?**: Sí / No
**¿Hubo pérdida de pasos durante el cambio de dirección?**: Sí / No

### Test 4: Múltiples Revoluciones

**Revoluciones ejecutadas**: ______
**Tiempo total**: ______ s
**Tiempo promedio por revolución**: ______ s
**¿El motor completó exactamente las revoluciones solicitadas?**: Sí / No

### Notas Adicionales
```






```

---

## Experimento 4: Control Avanzado (04_stepper_advanced_control)

### Pruebas Realizadas

#### Rotación Continua
**Comando 'f' (adelante)**:
- ¿Funcionó correctamente?: Sí / No
- Observaciones: ____________________________________________________

**Comando 'b' (atrás)**:
- ¿Funcionó correctamente?: Sí / No
- Observaciones: ____________________________________________________

#### Control de Velocidad
**Velocidad mínima probada (delay máximo)**: ______ ms
**Velocidad máxima probada (delay mínimo)**: ______ ms
**¿El cambio de velocidad es perceptible?**: Sí / No

#### Movimientos Precisos
**Revolución completa (comando 'r')**:
- Precisión: Excelente / Buena / Regular / Mala
- Observaciones: ____________________________________________________

**Ángulo específico (comando 'a')**:
- Ángulo probado: ______°
- Precisión: Excelente / Buena / Regular / Mala

### Aplicaciones Potenciales
```
Basándote en tus pruebas, ¿para qué aplicaciones crees que este motor
sería útil?

Ejemplos: relojes, cámaras pan-tilt, dispensadores, etc.







```

---

## Conclusiones Generales

### Ventajas del Motor 28BYJ-48
1. _______________________________________________________________
2. _______________________________________________________________
3. _______________________________________________________________

### Limitaciones Observadas
1. _______________________________________________________________
2. _______________________________________________________________
3. _______________________________________________________________

### Diferencias con el Motor DC (experimento anterior)

| Característica        | Motor DC | Motor Paso a Paso |
|-----------------------|----------|-------------------|
| Control de posición   |          |                   |
| Control de velocidad  |          |                   |
| Precisión             |          |                   |
| Facilidad de uso      |          |                   |
| Consumo energético    |          |                   |
| Ruido                 |          |                   |

### Aprendizajes Clave
```
¿Qué aprendiste sobre motores paso a paso?











```

### Próximos Pasos
```
¿Qué te gustaría probar a continuación?
- ¿Control con potenciómetro?
- ¿Integración con sensores?
- ¿Microstepping?
- ¿Otro tipo de motor?







```

---

## Problemas Encontrados y Soluciones

| Problema | Causa Posible | Solución Aplicada | ¿Funcionó? |
|----------|---------------|-------------------|------------|
|          |               |                   | Sí / No    |
|          |               |                   | Sí / No    |
|          |               |                   | Sí / No    |

---

## Fotos y Diagramas

**Incluir aquí**:
- [ ] Foto del circuito montado
- [ ] Foto del driver ULN2003 mostrando configuración del jumper
- [ ] Diagrama de conexiones (si hiciste uno propio)
- [ ] Captura del Serial Monitor mostrando resultados

---

## Verificación Final

Antes de considerar el experimento completo, verifica:

- [ ] Entiendes cómo funciona la secuencia de pasos
- [ ] Puedes controlar la dirección del motor
- [ ] Puedes controlar la velocidad del motor
- [ ] Comprendes la diferencia entre motor DC y paso a paso
- [ ] Sabes cuándo usar un motor paso a paso vs un motor DC
- [ ] Has documentado tus resultados
- [ ] Has guardado el código con tus modificaciones

**Firma**: _________________ **Fecha**: _________________
