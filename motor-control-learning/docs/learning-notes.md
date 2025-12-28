# Learning Notes - Conceptos y "Aha Moments"

## Índice

- [Fundamentos de MOSFETs](#fundamentos-de-mosfets)
- [Resistencias Pull-Down](#resistencias-pull-down)
- [Diodos de Protección](#diodos-de-protección)
- [Conexiones Serie vs Paralelo](#conexiones-serie-vs-paralelo)
- [Troubleshooting Sistemático](#troubleshooting-sistemático)

---

## Fundamentos de MOSFETs

### ¿Qué es un MOSFET?

**MOSFET** = Metal-Oxide-Semiconductor Field-Effect Transistor

**Función**: Actúa como un interruptor controlado por voltaje
- **Gate** (puerta): El control (entrada de voltaje)
- **Drain** (drenador): Por donde "entra" la corriente principal
- **Source** (fuente): Por donde "sale" la corriente principal

### Analogía con una Llave de Agua

```
Gate = Tu mano girando el grifo
Drain = Entrada de agua desde la tubería
Source = Salida de agua del grifo

Cuando aplicas voltaje al gate, "abres el grifo"
permitiendo que fluya corriente entre drain y source.
```

### Tipos de MOSFET

**N-Channel** (como el IRF520):
- Source conectado a GND
- Gate positivo abre el canal
- Corriente fluye de drain (arriba) a source (abajo)

**P-Channel** (opuesto):
- Source conectado a VCC
- Gate negativo abre el canal
- Menos común en proyectos de hobby

### Logic Level vs Standard MOSFETs

**Standard MOSFET** (como IRF540):
- Necesita 10V en gate para saturar completamente
- Con 5V solo se abre parcialmente
- Se calienta más, menos eficiente

**Logic Level MOSFET** (como IRLZ44N):
- Diseñado para saturar con 5V (o menos)
- Compatible con salidas de Arduino/microcontroladores
- Más eficiente a bajos voltajes

**IRF520 es "casi" logic level**:
- Vgs(th) = 2-4V (voltaje umbral)
- Se abre parcialmente con 5V
- Para 60-90mA funciona aceptable
- Para corrientes altas (>500mA) mejor usar IRLZ44N

### Estado del MOSFET según Gate

| Voltaje Gate | Estado | Rds(on) | Corriente | Efecto Motor |
|--------------|--------|---------|-----------|--------------|
| 0V           | OFF    | ∞       | 0mA       | Apagado      |
| 1-2V         | Umbral | Alta    | Baja      | Apenas gira  |
| 3-4V         | Parcial| Media   | Media     | Velocidad media |
| 5V           | ON*    | Baja*   | Máxima*   | Máxima vel*  |

*Para IRF520, no es saturación completa, pero suficiente para este proyecto

### ¿Por qué el Gate tiene "alta impedancia"?

El gate está aislado del canal por óxido de silicio:
- No consume corriente (μA despreciables)
- Funciona por campo eléctrico, no por corriente
- Por eso puede "flotar" y captar interferencias
- **Solución**: Resistencia pull-down

### Conceptos Clave

**Vgs(th)** - Gate-Source Threshold Voltage:
- Voltaje mínimo en gate para empezar a conducir
- IRF520: 2-4V típico

**Rds(on)** - Drain-Source ON Resistance:
- Resistencia del MOSFET cuando está totalmente encendido
- Menor Rds(on) = menos calor, más eficiente
- IRF520 @ Vgs=10V: ~0.27Ω
- IRF520 @ Vgs=5V: mayor, por eso se calienta más

**Saturación**:
- Estado donde el MOSFET está completamente abierto
- Mínima resistencia, máxima eficiencia
- IRF520 no satura completamente con 5V

---

## Resistencias Pull-Down

### ¿Qué es una Resistencia Pull-Down?

Una resistencia que "tira" un pin hacia GND (0V) cuando no hay otra señal activa.

```
         Pin flotante SIN pull-down
              
         ??? V
          │
         Gate ◄── Capta ruido eléctrico del ambiente
          
         Resultado: Comportamiento errático


         Pin con pull-down
         
         5V (cuando pot activo)
          │
         Gate
          │
         10K resistor
          │
         GND ◄── Cuando pot = 0V, gate se fija a 0V
         
         Resultado: Comportamiento estable
```

### ¿Por qué 10K y no otro valor?

**Demasiado baja** (ejemplo: 100Ω):
- Desperdiciaría corriente del potenciómetro
- Carga excesiva en el circuito de control

**10K (valor ideal)**:
- Casi no consume corriente (0.5mA @ 5V)
- Suficiente para fijar el gate a GND
- Valor estándar, fácil de conseguir

**Demasiado alta** (ejemplo: 100KΩ):
- Podría no "tirar" suficiente
- Gate aún podría flotar en ciertas condiciones

### Pull-Down vs Pull-Up

**Pull-Down**: Tira a GND (0V)
```
Signal ──┬── Input
         │
        10K
         │
        GND
```

**Pull-Up**: Tira a VCC (5V)
```
        VCC
         │
        10K
         │
Signal ──┴── Input
```

Para MOSFETs N-channel usamos pull-DOWN (queremos gate a 0V cuando está apagado).

---

## Diodos de Protección

### El Problema: Motores son Cargas Inductivas

**Inductor** = componente que almacena energía en campo magnético

Un motor DC es esencialmente:
- Bobina de cobre (inductor)
- Girando dentro de un campo magnético

**¿Qué pasa cuando cortas la corriente?**
1. El campo magnético colapsa
2. La energía almacenada tiene que ir a algún lado
3. Genera un **pico de voltaje inverso** (flyback)
4. Puede alcanzar -100V o más
5. **Destruye el MOSFET** si no hay protección

### La Solución: Diodo Flyback

```
         Motor como Inductor
         
    (+) ──┬── Bobina ──┬── (-)
          │            │
      Cátodo        Ánodo
          │            │
          └── DIODO ───┘
          
Funcionamiento normal: Diodo no conduce (polaridad inversa)
Motor se apaga: Pico negativo → Diodo conduce → Energía disipada
```

### ¿Por qué EN PARALELO y no en SERIE?

**INCORRECTO - En Serie:**
```
5V → Motor → DIODO → MOSFET → GND
         ❌ Pierde 0.7V siempre
         ❌ Reduce corriente del motor
```

**CORRECTO - En Paralelo:**
```
5V ──┬── Motor ──┬── MOSFET
     │           │
 Cátodo      Ánodo
     └─ DIODO ──┘
     
✓ Sin caída de voltaje en operación normal
✓ Solo conduce cuando hay pico negativo
```

### Identificación del Diodo

**1N4007:**
- Banda plateada/negra = **Cátodo** (negativo)
- Sin banda = **Ánodo** (positivo)
- Corriente fluye de ánodo → cátodo
- En nuestro circuito: cátodo al (+), ánodo al (-)

### Valores Típicos

**Vf** (Forward voltage): 0.6-0.7V
- Caída de voltaje cuando conduce
- 1N4007: ~0.69V medido

**Test con multímetro** (modo diodo):
- Directo (ánodo +, cátodo -): ~0.6V
- Inverso (cátodo +, ánodo -): OL (infinito)

---

## Conexiones Serie vs Paralelo

### Serie: Componentes en Línea

```
5V ── R1 ── R2 ── R3 ── GND

Características:
- Misma corriente pasa por todos
- Voltajes se SUMAN (V_total = V1 + V2 + V3)
- Resistencias se SUMAN (R_total = R1 + R2 + R3)
```

### Paralelo: Componentes en Ramas

```
     ┌── R1 ──┐
5V ──┼── R2 ──┼── GND
     └── R3 ──┘

Características:
- Mismo voltaje en todos
- Corrientes se SUMAN (I_total = I1 + I2 + I3)
- Resistencia total MENOR que la más pequeña
```

### En Nuestro Circuito

**Motor + MOSFET = Serie**
```
5V → Motor → MOSFET → GND
(corriente fluye por ambos)
```

**Diodo = Paralelo con Motor**
```
5V ──┬── Motor ──┬── MOSFET
     └── Diodo ──┘
(solo conduce cuando hay pico negativo)
```

---

## Troubleshooting Sistemático

### Metodología General

1. **Definir comportamiento esperado**
   - ¿Qué DEBERÍA pasar?
   - ¿Qué parámetros verificar?

2. **Observar comportamiento actual**
   - ¿Qué ESTÁ pasando realmente?
   - ¿Mediciones concretas?

3. **Identificar discrepancia**
   - ¿Qué es diferente?
   - ¿Cuánto difiere?

4. **Generar hipótesis**
   - ¿Por qué podría pasar esto?
   - Lista de causas posibles

5. **Probar hipótesis sistemáticamente**
   - Test más simple primero
   - Cambiar UNA variable a la vez
   - Documentar cada test

6. **Verificar solución**
   - ¿Solucionó el problema original?
   - ¿Introdujo nuevos problemas?

### Árbol de Decisión: Motor No Arranca

```
Motor no arranca
│
├─ ¿Hay voltaje en motor (+)?
│  ├─ NO → Problema en fuente/conexión a 5V
│  └─ SÍ → Continuar
│
├─ ¿Hay voltaje en motor (-)?
│  ├─ SÍ → Cortocircuito
│  └─ NO → MOSFET probablemente OFF, continuar
│
├─ ¿Hay voltaje en Gate del MOSFET?
│  ├─ NO → Problema en potenciómetro/conexión
│  │  └─ Verificar: pot conectado, cables, soldaduras
│  └─ SÍ → Continuar
│
├─ ¿Cuánto voltaje en Gate?
│  ├─ <1V → Insuficiente, verificar pot está en máximo
│  ├─ 1-2V → En umbral, motor apenas girará
│  ├─ 3-5V → Debería funcionar
│  └─ >5V → ¿De dónde viene? Problema serio
│
└─ ¿Funciona motor conectado directo a 5V?
   ├─ NO → Motor dañado, reemplazar
   └─ SÍ → MOSFET probablemente dañado o mal conectado
      └─ Verificar pinout Gate/Drain/Source
```

### Herramientas de Debug

**Multímetro**:
- Voltaje DC: Verificar alimentación, voltajes de señal
- Corriente DC: Medir consumo real
- Resistencia: Verificar continuidad, componentes
- Modo diodo: Test de semiconductores

**Component Tester**:
- Identificación automática de componentes
- Verifica transistores, diodos, capacitores
- Mide valores reales vs nominales

**Serial Monitor (Arduino)**:
- Debugging por software
- Monitorizar valores de sensores
- Trazar comportamiento en tiempo real

---

## Template para Nuevas Notas

```markdown
## [Concepto/Tema]

### Pregunta Original
[¿Qué no entendía?]

### Explicación Simple
[ELI5 - Explain Like I'm 5]

### Analogía
[Comparación con algo familiar]

### Detalles Técnicos
[Explicación más profunda]

### Aplicación Práctica
[¿Cómo se usa esto en el proyecto?]

### Errores Comunes
[¿Qué NO hacer?]

### Referencias
[Links, datasheets, fuentes]
```

---

**Última actualización**: 2024-12-28  
**Conceptos documentados**: 5
