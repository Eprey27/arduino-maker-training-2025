# Diagrama de Conexiones Detallado - Motor 28BYJ-48

## Vista General del Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                    SISTEMA COMPLETO                              │
│                                                                  │
│  ┌──────────────┐      ┌───────────────┐      ┌──────────────┐ │
│  │              │      │               │      │              │ │
│  │   Arduino    │─────→│  Driver       │─────→│    Motor     │ │
│  │   UNO/ESP32  │      │  ULN2003      │      │   28BYJ-48   │ │
│  │              │      │               │      │              │ │
│  └──────────────┘      └───────────────┘      └──────────────┘ │
│         ↑                      ↑                                │
│         │                      │                                │
│         └──────────────────────┘                                │
│              Alimentación 5V                                    │
└─────────────────────────────────────────────────────────────────┘
```

## Conexiones Paso a Paso

### 1. Arduino a Driver ULN2003

```
    Arduino UNO                     Driver ULN2003
   ┌─────────┐                    ┌──────────────┐
   │         │                    │              │
   │   D8 ───┼────────────────────┼──→ IN1       │
   │         │   Cable jumper     │              │
   │   D9 ───┼────────────────────┼──→ IN2       │
   │         │                    │              │
   │   D10 ──┼────────────────────┼──→ IN3       │
   │         │                    │              │
   │   D11 ──┼────────────────────┼──→ IN4       │
   │         │                    │              │
   │   5V ───┼────────────────────┼──→ + (VCC)   │
   │         │                    │              │
   │   GND ──┼────────────────────┼──→ - (GND)   │
   │         │                    │              │
   └─────────┘                    └──────────────┘
```

### 2. Driver ULN2003 a Motor 28BYJ-48

```
    Driver ULN2003                 Motor 28BYJ-48
   ┌──────────────┐               ┌──────────────┐
   │              │               │              │
   │   Conector   │   Cable       │   Conector   │
   │   5 pines ───┼───────────────┼──→ 5 pines   │
   │              │   incluido    │              │
   └──────────────┘   con motor   └──────────────┘

   Nota: Este cable NO se puede conectar al revés,
         el conector tiene orientación única.
```

### 3. Alimentación

```
   Opción 1: Alimentación desde Arduino
   ─────────────────────────────────────

   USB Computer ──→ Arduino UNO ──→ Driver ULN2003 ──→ Motor
                    (5V, GND)         (Hasta ~240mA)


   Opción 2: Alimentación externa (recomendada para ESP32)
   ────────────────────────────────────────────────────────

   Fuente 5V ──┬──→ Driver ULN2003 (+ y -)
               │
               └──→ Arduino/ESP32 (compartir GND)
                    (IMPORTANTE: GND común)
```

## Vista Detallada del Driver ULN2003

### Panel de Conexiones

```
┌────────────────────────────────────────┐
│         Driver ULN2003APG              │
│                                        │
│  ┌───┬───┬───┬───┬───┬───┐           │
│  │IN1│IN2│IN3│IN4│ + │ - │  Entrada  │
│  └───┴───┴───┴───┴───┴───┘   Arduino │
│                                        │
│   ●   ●   ●   ●   ●   ●   ●           │
│  IN1 IN2 IN3 IN4 IN5 IN6 IN7          │
│  (Solo usamos IN1-IN4)                │
│                                        │
│  [LED-A] [LED-B] [LED-C] [LED-D]      │
│     ●       ●       ●       ●          │
│                                        │
│     Alimentación: - + (5V-12V)         │
│     Algunos modelos incluyen jumper    │
│                                        │
│        Conector Motor                  │
│  ┌─────────────────────┐               │
│  │ ● ● ● ● ●           │ 5 pines       │
│  └─────────────────────┘               │
│                                        │
└────────────────────────────────────────┘
```

### Descripción de Componentes

#### Entradas (IN1-IN4)
- **IN1**: Control bobina 1 → conectar a Arduino D8
- **IN2**: Control bobina 2 → conectar a Arduino D9
- **IN3**: Control bobina 3 → conectar a Arduino D10
- **IN4**: Control bobina 4 → conectar a Arduino D11

#### LEDs Indicadores
- **LED A**: Indica cuando IN1 está activo (bobina 1 energizada)
- **LED B**: Indica cuando IN2 está activo (bobina 2 energizada)
- **LED C**: Indica cuando IN3 está activo (bobina 3 energizada)
- **LED D**: Indica cuando IN4 está activo (bobina 4 energizada)

Durante la operación normal, verás estos LEDs encendiéndose en secuencia.

#### Alimentación

El driver ULN2003APG acepta un rango de voltaje de 5V a 12V.

**Para el motor 28BYJ-48 de 5V:**

- Alimentar con **5V** desde Arduino
- Conectar + del driver al 5V del Arduino
- Conectar - del driver al GND del Arduino

**Nota sobre jumpers**: Algunos modelos de fabricantes chinos incluyen un jumper adicional. La función exacta puede variar según el modelo. Para este motor de 5V, simplemente alimenta el driver con 5V.

#### Conector del Motor
- 5 pines en línea
- Cable incluido con el motor
- Orientación única (no se puede conectar al revés)

## Pinout del Motor 28BYJ-48

### Cable del Motor (vista desde el conector)

```
  ┌─────────────────┐
  │ ○ ○ ○ ○ ○       │  Conector macho
  └─────────────────┘
    │ │ │ │ │
    │ │ │ │ └──→ Pin 5: Bobina 4
    │ │ │ └────→ Pin 4: Común (VCC)
    │ │ └──────→ Pin 3: Bobina 3
    │ └────────→ Pin 2: Bobina 2
    └──────────→ Pin 1: Bobina 1

Código de colores típico:
Pin 1: Naranja  (Bobina 1)
Pin 2: Amarillo (Bobina 2)
Pin 3: Rosa     (Bobina 3)
Pin 4: Rojo     (Común - VCC)
Pin 5: Azul     (Bobina 4)

Nota: Los colores pueden variar según el fabricante
```

## Esquema Eléctrico Simplificado

```
                    +5V
                     │
                     ├──────┬──────┬──────┬──────┐
                     │      │      │      │      │
    Arduino         │      │      │      │      │
   ┌────────┐       │      │      │      │      │
   │        │       │     ┌▼┐    ┌▼┐    ┌▼┐    ┌▼┐
   │  D8 ──────┐   │     │1│    │2│    │3│    │4│  Bobinas
   │        │   │   │     └┬┘    └┬┘    └┬┘    └┬┘  Motor
   │  D9 ────┐ │   │      │      │      │      │
   │        │ │ │   │     ┌▼┐    ┌▼┐    ┌▼┐    ┌▼┐
   │  D10 ──┐│ │ │   │     │ │    │ │    │ │    │ │
   │        │││ │ │   │     └┬┘    └┬┘    └┬┘    └┬┘
   │  D11 ─┐││││ │ │   │      └──────┴──────┴──────┘
   │        ││││││ │ │   │
   │  GND ─┐│││││││ │ │   ├────┐
   └────────┘││││││││ │ │   │    │
     ││││││││││││││ │ │   │    │
     │││││││││││││└─┴─┴───┘    │
     │││││││└─┴─┴─ULN2003      │
     └┴┴┴┴┴──────────────────GND
```

## Checklist de Conexión

### Antes de Alimentar

- [ ] Driver ULN2003 en buen estado
- [ ] Arduino Pin 8 → Driver IN1
- [ ] Arduino Pin 9 → Driver IN2
- [ ] Arduino Pin 10 → Driver IN3
- [ ] Arduino Pin 11 → Driver IN4
- [ ] Arduino 5V → Driver + (positivo)
- [ ] Arduino GND → Driver - (tierra)
- [ ] Motor conectado al driver (conector de 5 pines)
- [ ] Todas las conexiones firmes y seguras

### Después de Conectar, Antes de Cargar Código

- [ ] Inspección visual de todas las conexiones
- [ ] No hay cables sueltos o tocándose
- [ ] Driver recibiendo 5V desde Arduino
- [ ] Motor bien conectado

### Después de Cargar Código

- [ ] Serial Monitor abierto (115200 baud)
- [ ] LEDs del driver se encienden en secuencia
- [ ] Motor gira suavemente
- [ ] No hay olor a quemado
- [ ] Driver no está excesivamente caliente

## Solución de Problemas de Conexión

### Problema: Nada funciona

**Verificar**:
1. ¿Arduino recibe alimentación? (LED on-board encendido)
2. ¿Driver recibe alimentación? (verificar con multímetro si es posible)
3. ¿Código cargado correctamente? (verifica Serial Monitor)
4. ¿GND común entre Arduino y Driver?

### Problema: Motor vibra pero no gira

**Verificar**:
1. ¿Están todos los 4 pines IN1-IN4 conectados?
2. ¿Cable del motor bien insertado en el driver?
3. ¿Secuencia de pines correcta en el código?

### Problema: Gira muy lento o errático

**Verificar**:
1. ¿Delay en el código muy alto?
2. ¿Alimentación insuficiente? (caída de voltaje)
3. ¿Obstrucción mecánica en el eje?

### Problema: Se calienta demasiado

**Verificar**:

1. ⚠️ **IMPORTANTE**: ¿Estás usando 5V? (este motor es de 5V, no de 12V)
2. ¿Motor sobrecargado? (eje bloqueado)
3. ¿Ventilación adecuada?

## Notas de Seguridad

### ⚠️ Advertencias

1. **Nunca conectar/desconectar el motor con el sistema energizado**
   - Apaga la alimentación antes de manipular conexiones

2. **Verificar polaridad**
   - + del Arduino va a + del Driver
   - GND del Arduino va a - del Driver

3. **No usar voltajes superiores a 5V con este motor**
   - El motor 28BYJ-48 es de 5V, usar 12V lo dañará

4. **No bloquear el eje del motor por tiempo prolongado**
   - Puede sobrecalentar las bobinas

### ✅ Buenas Prácticas

1. **Usa cables de buena calidad**
   - Evita cables muy delgados o dañados

2. **Organiza el cableado**
   - Evita cables cruzados o enredados

3. **Documenta tu conexión**
   - Toma una foto del circuito armado

4. **Prueba paso a paso**
   - Verifica conexiones antes de alimentar
   - Carga el sketch más simple primero

## Variantes de Conexión

### Con ESP32 en lugar de Arduino UNO

```
    ESP32                          Driver ULN2003
   ┌─────────┐                    ┌──────────────┐
   │         │                    │              │
   │  GPIO25 ┼────────────────────┼──→ IN1       │
   │  GPIO26 ┼────────────────────┼──→ IN2       │
   │  GPIO27 ┼────────────────────┼──→ IN3       │
   │  GPIO14 ┼────────────────────┼──→ IN4       │
   │   5V    ┼────────────────────┼──→ +         │
   │   GND   ┼────────────────────┼──→ -         │
   └─────────┘                    └──────────────┘

Cambios en el código:
#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
```

### Con Alimentación Externa

```
    Fuente 5V 1A
   ┌───────────┐
   │  +    -   │
   └───┬───┬───┘
       │   │
       │   │    Driver ULN2003
       │   │   ┌──────────────┐
       │   └───┼──→ -         │
       └───────┼──→ +         │
               │              │
   Arduino     │              │
   ┌─────┐     │              │
   │ GND ├─────┼──→ - (común) │
   │ D8  ├─────┼──→ IN1       │
   │ D9  ├─────┼──→ IN2       │
   │ D10 ├─────┼──→ IN3       │
   │ D11 ├─────┼──→ IN4       │
   └─────┘     └──────────────┘

⚠️ IMPORTANTE: GND común entre Arduino y Driver
```

## Referencias Visuales

### Aspecto Real del Driver ULN2003

```
Vista Superior:
┌────────────────────────────┐
│  [●●●●●●●] ← Pines entrada │
│                             │
│  LED LED LED LED            │
│   A   B   C   D  ← LEDs    │
│                             │
│  [-] [+] ← Alimentación     │
│   (5V-12V según motor)      │
│                             │
│  [●●●●●] ← Conector motor  │
│                             │
│    ULN2003APG               │
└────────────────────────────┘
```

### Aspecto Real del Motor 28BYJ-48

```
Vista Frontal:
       ┌──────┐
       │  ○   │ ← Eje de salida
       └───┬──┘
           │
      ┌────┴────┐
      │  28BYJ  │
      │   -48   │ ← Cuerpo del motor
      │   5VDC  │
      └────┬────┘
           │
        ┌──┴──┐
        │Cable│ ← Cable con conector
        └─────┘    de 5 pines
```

---

## Resumen Rápido

**Conexiones mínimas requeridas**: 6 cables
1. D8 → IN1
2. D9 → IN2
3. D10 → IN3
4. D11 → IN4
5. 5V → +
6. GND → -

**Más**: Cable del motor al driver (incluido con el motor)

**Tiempo de montaje**: ~10 minutos

**¡Listo para empezar!**

Continúa con [GETTING_STARTED.md](../GETTING_STARTED.md) para comenzar con los experimentos.
