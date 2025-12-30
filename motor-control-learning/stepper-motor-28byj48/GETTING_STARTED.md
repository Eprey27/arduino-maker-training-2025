# Guía de Inicio Rápido - Motor 28BYJ-48

## Bienvenido

Esta guía te ayudará a comenzar con tu motor paso a paso 28BYJ-48 y el driver ULN2003.

## Antes de Empezar

### ⚠️ IMPORTANTE - Verificación del Hardware

1. **Inspecciona el motor**
   - Verifica que el cable conector de 5 pines esté en buen estado
   - No debe haber cables rotos o sueltos

2. **Verifica el driver ULN2003**
   - Inspecciona los pines de entrada (IN1-IN4)
   - Los 4 LEDs indicadores deben estar visibles
   - El conector para el motor debe estar limpio

3. **Alimentación**
   - Usa 5V desde Arduino para el motor 28BYJ-48 de 5V
   - USB de computadora normalmente es suficiente (500mA típico)
   - Asegura que puedes suministrar al menos 300mA

## Conexiones del Circuito

```
Arduino          Driver ULN2003
───────          ──────────────
Pin 8      →     IN1
Pin 9      →     IN2
Pin 10     →     IN3
Pin 11     →     IN4
5V         →     + (positivo)
GND        →     - (tierra)

Driver     →     Motor
──────           ─────
Conector de 5 pines → Conector del motor
```

### Diagrama Visual

```
    Arduino UNO
   ┌─────────────┐
   │             │
   │  D8  ─────────┐
   │  D9  ───────┐ │
   │  D10 ─────┐ │ │
   │  D11 ───┐ │ │ │
   │         │ │ │ │        Driver ULN2003
   │  5V  ───┼─┼─┼─┼────┐  ┌──────────────┐
   │  GND ───┼─┼─┼─┼──┐ │  │              │
   └─────────┘ │ │ │ │ │ └──→ +       Motor │
               │ │ │ │ └────→ -        ←──┘  │
               │ │ │ └──────→ IN1           │
               │ │ └────────→ IN2           │
               │ └──────────→ IN3           │
               └────────────→ IN4           │
                             └──────────────┘
```

## Camino de Aprendizaje

### Paso 1: Prueba Básica (15 minutos)
**Archivo**: [01_basic_stepper_test.ino](sketches/01_basic_stepper_test/01_basic_stepper_test.ino)

**Qué aprenderás**:
- Cómo funciona la secuencia de pasos
- Observar los LEDs del driver
- Entender el movimiento paso a paso

**Qué hacer**:
1. Sube el sketch a tu Arduino
2. Abre el Serial Monitor (115200 baud)
3. Observa los LEDs del driver mientras el motor gira
4. Verifica que el motor complete una revolución completa

**Señales de éxito**:
- Los 4 LEDs del driver se encienden en secuencia
- El motor gira suavemente sin saltos
- Escuchas un sonido rítmico de los pasos

---

### Paso 2: Uso de Librería (15 minutos)
**Archivo**: [02_stepper_library.ino](sketches/02_stepper_library/02_stepper_library.ino)

**Qué aprenderás**:
- Usar la librería Stepper.h de Arduino
- Control más simple y directo
- Diferencias con el control manual

**Qué hacer**:
1. Sube el sketch
2. Observa las diferentes velocidades
3. Mide el tiempo de cada revolución
4. Compara con el experimento 1

**Nota importante**: El orden de los pines es diferente (IN1, IN3, IN2, IN4)

---

### Paso 3: Mediciones (30 minutos)
**Archivo**: [03_stepper_measurement.ino](sketches/03_stepper_measurement/03_stepper_measurement.ino)

**Qué aprenderás**:
- Límites de velocidad del motor
- Precisión de posicionamiento
- Comportamiento bajo diferentes condiciones

**Qué hacer**:
1. Sube el sketch (ejecuta pruebas automáticas)
2. Marca la posición inicial del eje del motor
3. Anota los resultados en [test-results-template.md](docs/test-results-template.md)
4. Verifica si el motor regresa a su posición inicial

**Materiales adicionales**:
- Papel y lápiz para anotar resultados
- Opcionalmente, un cronómetro

---

### Paso 4: Control Avanzado (30-60 minutos)
**Archivo**: [04_stepper_advanced_control.ino](sketches/04_stepper_advanced_control/04_stepper_advanced_control.ino)

**Qué aprenderás**:
- Control interactivo por Serial
- Cambio de dirección dinámico
- Ajuste de velocidad en tiempo real
- Movimientos programados

**Qué hacer**:
1. Sube el sketch
2. Abre Serial Monitor (115200 baud)
3. Experimenta con los comandos (f, b, s, +, -, r, etc.)
4. Prueba diferentes secuencias de movimiento

**Comandos disponibles**:
- `f` = Adelante continuo
- `b` = Atrás continuo
- `s` = Detener
- `+` = Más rápido
- `-` = Más lento
- `r` = 1 revolución
- `h` = Media revolución (180°)
- `i` = Información del estado
- `?` = Ayuda

---

## Documentación de Apoyo

### Referencias Principales
1. [README.md](README.md) - Información completa del proyecto
2. [quick-reference.md](docs/quick-reference.md) - Referencia rápida de especificaciones y código
3. [test-results-template.md](docs/test-results-template.md) - Plantilla para documentar tus resultados

### Información Técnica Clave

**Especificaciones del Motor 28BYJ-48**:
- Pasos por revolución: 2048 (half-step)
- Voltaje: 5V DC
- Velocidad máxima: ~15 RPM
- Ángulo por paso: 0.176°

**Valores Recomendados**:
- Delay óptimo: 2-3 ms entre pasos
- Velocidad estable: 10-15 RPM
- Consumo: ~240mA

## Solución de Problemas Comunes

### El motor no gira
- ✓ Verifica todas las conexiones
- ✓ Confirma que el driver reciba 5V
- ✓ Asegúrate de que el cable del motor esté bien conectado
- ✓ Comprueba que los pines en el código coincidan con tu cableado

### El motor vibra pero no gira
- ✓ El delay es demasiado corto (velocidad muy alta)
- ✓ Aumenta el delay a 5-10 ms
- ✓ Verifica que no haya obstrucciones en el eje

### Los LEDs no se encienden
- ✓ Verifica conexión de alimentación (5V y GND)
- ✓ El driver puede estar dañado
- ✓ Los pines del Arduino pueden estar mal configurados

### Gira en dirección incorrecta
- ✓ Esto es normal - solo invierte la dirección en el código
- ✓ Cambia de `currentStep++` a `currentStep--`

### Se calienta demasiado

- ✓ Verifica que estés usando 5V (el motor es de 5V, no de 12V)
- ✓ El motor puede estar sobrecargado
- ✓ Un calentamiento leve es normal

## Próximos Pasos

Después de completar los 4 experimentos, puedes:

1. **Documentar tus resultados**
   - Llena la plantilla [test-results-template.md](docs/test-results-template.md)
   - Toma fotos de tu setup
   - Anota tus observaciones

2. **Experimentar con modificaciones**
   - Cambia los valores de delay
   - Crea secuencias de movimiento personalizadas
   - Implementa aceleración gradual

3. **Integrar con otros componentes**
   - Control con potenciómetro (como en el experimento DC anterior)
   - Botones para cambiar dirección
   - Display LCD para mostrar posición

4. **Proyectos sugeridos**
   - Reloj mecánico
   - Sistema pan-tilt para cámara
   - Dispensador automatizado
   - Indicador analógico (medidor)

## Recursos Adicionales

- **Arduino Reference**: https://www.arduino.cc/reference/en/libraries/stepper/
- **Datasheet ULN2003**: Búsqueda online por "ULN2003 datasheet"
- **Tutoriales**: Hay muchos tutoriales en YouTube sobre este motor

## Ayuda y Soporte

Si encuentras problemas:
1. Consulta la sección de solución de problemas arriba
2. Revisa la [quick-reference.md](docs/quick-reference.md)
3. Verifica dos veces todas las conexiones
4. Asegúrate de que el código está correctamente cargado

## Checklist de Inicio

Antes de empezar, verifica:

- [ ] Driver ULN2003 en buen estado
- [ ] Todas las conexiones realizadas según el diagrama
- [ ] Arduino conectado a la computadora
- [ ] IDE de Arduino instalado y funcionando
- [ ] Serial Monitor configurado a 115200 baud
- [ ] Tienes papel para anotar observaciones

**¡Listo para comenzar!**

Empieza con el [Experimento 1](sketches/01_basic_stepper_test/01_basic_stepper_test.ino) y sigue el camino de aprendizaje paso a paso.

---

**¡Diviértete aprendiendo!**
