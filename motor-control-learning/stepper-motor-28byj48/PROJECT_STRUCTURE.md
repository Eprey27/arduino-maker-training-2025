# Estructura del Proyecto - Motor 28BYJ-48

## Estructura de Archivos

```
stepper-motor-28byj48/
│
├── README.md                          # Documentación principal del proyecto
├── GETTING_STARTED.md                 # Guía de inicio rápido (EMPIEZA AQUÍ)
├── PROJECT_STRUCTURE.md              # Este archivo - estructura del proyecto
│
├── docs/                             # Documentación y referencias
│   ├── quick-reference.md            # Referencia rápida de código y especificaciones
│   └── test-results-template.md      # Plantilla para documentar resultados
│
└── sketches/                         # Código Arduino (experimentos)
    │
    ├── 01_basic_stepper_test/
    │   └── 01_basic_stepper_test.ino     # Experimento 1: Control básico manual
    │
    ├── 02_stepper_library/
    │   └── 02_stepper_library.ino        # Experimento 2: Uso de librería Stepper.h
    │
    ├── 03_stepper_measurement/
    │   └── 03_stepper_measurement.ino    # Experimento 3: Mediciones y caracterización
    │
    └── 04_stepper_advanced_control/
        └── 04_stepper_advanced_control.ino  # Experimento 4: Control interactivo avanzado
```

## Descripción de Archivos

### 📄 Documentación Principal

#### [README.md](README.md)
**Propósito**: Documentación completa del proyecto
**Contenido**:
- Especificaciones del motor 28BYJ-48
- Detalles del driver ULN2003APG
- Conceptos fundamentales de motores paso a paso
- Diagrama de conexiones
- Plan de aprendizaje completo
- Secuencias de activación

**Cuándo usar**: Para entender la teoría y conceptos detrás del motor paso a paso

---

#### [GETTING_STARTED.md](GETTING_STARTED.md) ⭐ **EMPIEZA AQUÍ**
**Propósito**: Guía práctica paso a paso para comenzar
**Contenido**:
- Verificaciones previas de hardware
- Diagrama de conexiones visual
- Camino de aprendizaje secuencial
- Solución de problemas comunes
- Checklist de inicio

**Cuándo usar**: Tu primer archivo a leer antes de conectar cualquier cosa

---

#### [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)
**Propósito**: Mapa de navegación del proyecto (este archivo)
**Contenido**:
- Estructura de directorios
- Descripción de cada archivo
- Flujo de trabajo recomendado

**Cuándo usar**: Para orientarte en el proyecto y encontrar archivos específicos

---

### 📚 Documentación de Referencia (docs/)

#### [docs/quick-reference.md](docs/quick-reference.md)
**Propósito**: Referencia rápida para consulta durante el trabajo
**Contenido**:
- Especificaciones técnicas en formato tabla
- Código básico mínimo
- Conversiones útiles (pasos ↔ grados, delay ↔ RPM)
- Tabla de solución de problemas
- Valores típicos de delay y velocidad

**Cuándo usar**: Cuando necesites consultar rápidamente una especificación o snippet de código

---

#### [docs/test-results-template.md](docs/test-results-template.md)
**Propósito**: Plantilla para documentar tus experimentos
**Contenido**:
- Formularios para cada experimento
- Tablas de resultados
- Espacio para observaciones
- Comparaciones y conclusiones

**Cuándo usar**: Durante y después de cada experimento para documentar resultados

---

### 💻 Sketches de Arduino (sketches/)

#### [Experimento 1: 01_basic_stepper_test.ino](sketches/01_basic_stepper_test/01_basic_stepper_test.ino)
**Nivel**: Principiante
**Duración estimada**: 15 minutos
**Propósito**: Entender el funcionamiento básico del motor paso a paso

**Conceptos que aprenderás**:
- Secuencia de pasos half-step (8 pasos)
- Implementación manual de la secuencia
- Relación entre pasos y rotación
- Observación de LEDs del driver

**Características**:
- Implementación manual de secuencia
- Control de velocidad con delay
- Una revolución completa cada ciclo
- Monitoreo por Serial

**Ejercicios incluidos**:
- Cambiar velocidad (delay)
- Modificar número de pasos
- Invertir dirección
- Observar patrones de LEDs

---

#### [Experimento 2: 02_stepper_library.ino](sketches/02_stepper_library/02_stepper_library.ino)
**Nivel**: Principiante-Intermedio
**Duración estimada**: 15 minutos
**Propósito**: Aprender a usar la librería estándar Stepper.h

**Conceptos que aprenderás**:
- Uso de librería Stepper de Arduino
- Control de velocidad en RPM
- Dirección con valores positivos/negativos
- Diferencias con implementación manual

**Características**:
- Múltiples demos automatizadas
- Diferentes velocidades (5, 10, 15 RPM)
- Movimientos fraccionarios (1/2, 1/4 revolución)
- Medición de tiempo real vs teórico

**Ejercicios incluidos**:
- Determinar velocidad máxima sin pérdida de pasos
- Verificar precisión de posicionamiento
- Medir holding torque
- Comparar tiempos teóricos vs reales

**Nota importante**: Orden de pines diferente (IN1, IN3, IN2, IN4)

---

#### [Experimento 3: 03_stepper_measurement.ino](sketches/03_stepper_measurement/03_stepper_measurement.ino)
**Nivel**: Intermedio
**Duración estimada**: 30 minutos
**Propósito**: Caracterizar el rendimiento y límites del motor

**Conceptos que aprenderás**:
- Límites de velocidad del motor
- Precisión angular
- Reversibilidad
- Consistencia en múltiples revoluciones

**Pruebas automáticas**:
1. **Test de Velocidades**: Prueba 5 velocidades diferentes (10ms a 1ms delay)
2. **Test de Precisión Angular**: Rotaciones de 45°, 90°, 180°, 360°
3. **Test de Reversibilidad**: 5 ciclos adelante-atrás de 180°
4. **Test de Múltiples Revoluciones**: 5 revoluciones continuas

**Resultados que obtendrás**:
- Velocidad óptima para tu motor
- Velocidad máxima sin pérdida de pasos
- Precisión de posicionamiento
- Consistencia temporal

**Materiales adicionales necesarios**:
- Papel y lápiz para anotar
- Marca en el eje del motor (opcional)
- Cronómetro (opcional)

---

#### [Experimento 4: 04_stepper_advanced_control.ino](sketches/04_stepper_advanced_control/04_stepper_advanced_control.ino)
**Nivel**: Intermedio-Avanzado
**Duración estimada**: 30-60 minutos
**Propósito**: Control interactivo y aplicaciones prácticas

**Conceptos que aprenderás**:
- Control interactivo por Serial
- Cambio dinámico de velocidad
- Control de dirección en tiempo real
- Movimientos programados
- Tracking de posición

**Comandos disponibles**:
| Comando | Acción |
|---------|--------|
| `f/F` | Rotación continua adelante |
| `b/B` | Rotación continua atrás |
| `s/S` | Detener motor |
| `+` | Aumentar velocidad |
| `-` | Disminuir velocidad |
| `r/R` | 1 revolución completa |
| `h/H` | Media revolución (180°) |
| `q/Q` | Cuarto de revolución (90°) |
| `1-9` | N revoluciones |
| `a/A` | Ángulo específico (ingreso manual) |
| `i/I` | Información del estado |
| `?` | Ayuda |

**Características avanzadas**:
- Sistema de tracking de posición (en grados)
- Contador de pasos totales
- Información de estado en tiempo real
- Límites de velocidad configurables

**Ejercicios de extensión**:
- Implementar aceleración suave
- Crear posicionamiento absoluto
- Programar secuencias de movimiento
- Agregar límites de posición

---

## Flujo de Trabajo Recomendado

### 🚀 Inicio Rápido (Primera Sesión)

```
1. Lee GETTING_STARTED.md
   ↓
2. Verifica hardware (jumper en 5V)
   ↓
3. Conecta según diagrama
   ↓
4. Sube sketch 01_basic_stepper_test.ino
   ↓
5. Observa y anota en test-results-template.md
```

### 📖 Aprendizaje Completo (4 Sesiones)

**Sesión 1** (30 min):
- GETTING_STARTED.md
- Experimento 1
- Experimento 2

**Sesión 2** (45 min):
- Experimento 3
- Documentar resultados

**Sesión 3** (60 min):
- Experimento 4
- Experimentar con comandos
- Documentar observaciones

**Sesión 4** (opcional):
- Modificar sketches
- Crear proyecto propio
- Integrar con otros componentes

### 🔍 Referencia Durante el Trabajo

**Tienes una duda sobre código:**
→ docs/quick-reference.md

**El motor no funciona correctamente:**
→ GETTING_STARTED.md (sección "Solución de Problemas")

**Quieres saber qué hace cada archivo:**
→ PROJECT_STRUCTURE.md (este archivo)

**Necesitas teoría de fondo:**
→ README.md

**Quieres documentar resultados:**
→ docs/test-results-template.md

---

## Comparación de Sketches

| Aspecto | Experimento 1 | Experimento 2 | Experimento 3 | Experimento 4 |
|---------|---------------|---------------|---------------|---------------|
| **Complejidad** | Básica | Básica | Media | Avanzada |
| **Librería externa** | No | Sí (Stepper.h) | No | No |
| **Interactividad** | No | No | No | Sí (Serial) |
| **Mediciones** | No | Sí (tiempo) | Sí (completas) | Sí (estado) |
| **Control de velocidad** | Fija | Variable | Variable | Dinámica |
| **Control de dirección** | Fija | Fija/Invertida | Ambas | Dinámica |
| **Duración típica** | 15 min | 15 min | 30 min | 30-60 min |

---

## Dependencias

### Hardware Requerido
- Arduino UNO o ESP32
- Motor 28BYJ-48 (5V)
- Driver ULN2003APG
- Cables de conexión (jumper wires)
- Cable USB para Arduino

### Software Requerido
- Arduino IDE (1.8.x o superior, o Arduino IDE 2.x)
- Librería Stepper.h (incluida en Arduino IDE)

### No se requieren librerías externas
Todos los sketches funcionan con Arduino IDE estándar.

---

## Próximos Pasos Después de Completar

Una vez que completes los 4 experimentos, puedes:

### Proyectos Sugeridos
1. **Reloj Analógico**: Usa el motor para mover manecillas
2. **Pan-Tilt Camera**: Sistema de dos motores para mover cámara
3. **Dispensador**: Controla porciones con rotación precisa
4. **Indicador Analógico**: Crea un medidor estilo antiguo

### Integraciones
- Control con potenciómetro (combinar con experimento DC anterior)
- Botones para control de dirección
- Display LCD para mostrar posición
- Sensores para límites de movimiento

### Experimentos Avanzados
- Microstepping (requiere driver diferente)
- Control de múltiples motores simultáneos
- Comunicación I2C/SPI
- Control remoto (Bluetooth/WiFi)

---

## Convenciones de Código

Todos los sketches siguen estas convenciones:

### Nombres de Variables
- `currentStep`: Paso actual en la secuencia (0-7)
- `delayTime`: Tiempo en ms entre pasos
- `totalSteps`: Contador acumulado de pasos
- `direction`: Dirección de rotación (1 o -1)

### Nombres de Funciones
- `executeStep(step)`: Ejecuta un paso específico de la secuencia
- `stopMotor()`: Apaga todas las bobinas
- `rotateSteps(steps, delay)`: Rota N pasos con delay específico
- `rotateRevolutions(revs)`: Rota N revoluciones completas

### Pines (Constantes)
```cpp
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11
```

### Constantes del Motor
```cpp
const int STEPS_PER_REVOLUTION = 2048;  // Half-step
const float DEGREES_PER_STEP = 0.17578125;  // 360/2048
```

---

## Recursos de Ayuda

### Dentro del Proyecto
1. Comentarios extensivos en cada sketch
2. Sección de ejercicios al final de cada sketch
3. Documentación en docs/
4. Solución de problemas en GETTING_STARTED.md

### Externos
- Arduino Reference: https://www.arduino.cc/reference/
- Arduino Forum: https://forum.arduino.cc/
- Motor 28BYJ-48 datasheet (búsqueda online)
- ULN2003 datasheet (búsqueda online)

---

## Notas de Versión

**Versión**: 1.0
**Fecha**: 2025-12-30
**Autor**: Claude (Anthropic)
**Plataformas probadas**: Arduino UNO, ESP32
**Estado**: Completo y listo para usar

---

## Licencia y Uso

Este material educativo está diseñado para aprendizaje personal.
Siéntete libre de:
- Modificar los sketches
- Crear tus propios proyectos basados en estos experimentos
- Compartir con otros aprendices
- Documentar y publicar tus resultados

---

**¡Disfruta tu viaje de aprendizaje con motores paso a paso!**

Si tienes preguntas, revisa primero:
1. GETTING_STARTED.md
2. docs/quick-reference.md
3. Comentarios en los sketches
4. README.md
