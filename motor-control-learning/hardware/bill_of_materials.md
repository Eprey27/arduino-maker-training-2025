# Component Specifications

## Componentes Verificados

### IRF520 MOSFET (N-Channel)

**Especificaciones del Fabricante**:
- Tipo: N-Channel Enhancement Mode
- Vds max: 100V
- Id max: 9.2A
- Vgs(th): 2.0V - 4.0V (típico)
- Rds(on) @ Vgs=10V: 0.27Ω typ
- Rds(on) @ Vgs=5V: ~0.5Ω (estimado, no saturación completa)
- Package: TO-220
- Power dissipation: 60W (con heatsink adecuado)

**Pinout** (vista frontal, pines abajo):
```
Pin 1 (izquierda): GATE
Pin 2 (centro): DRAIN
Pin 3 (derecha): SOURCE
Tab (metal): Conectado internamente a DRAIN
```

**Mediciones Reales**:
- [Pendiente test de switching]
- Vgs necesario para motor 60-90mA: _____ V
- Rds(on) estimado a 5V: _____ Ω
- Temperatura en funcionamiento: _____ °C

**Notas de Uso**:
- NO es "logic level" puro (necesita 10V para saturación completa)
- Con 5V funciona para cargas ligeras (<200mA)
- Para >500mA usar IRLZ44N o IRL540N
- Usar resistencia pull-down 10K en gate
- Heatsink recomendado para >1A

**Datasheet**: [Link al datasheet si disponible]

---

### 1N4007 Diodo Rectificador

**Especificaciones del Fabricante**:
- Tipo: Silicon rectifier diode
- If max (continuous): 1A
- Vrrm (reverse voltage): 1000V
- Vf (forward voltage) @ 1A: 1.1V max, 0.7V típico
- Package: DO-41 (axial)

**Identificación**:
- Banda plateada/negra: CÁTODO (negativo)
- Sin marca: ÁNODO (positivo)

**Mediciones Reales** (2024-12-28):
- Vf medido con component tester: **0.69V** ✓
- Corriente de fuga: 73nA (despreciable)
- Test con multímetro modo diodo:
  - Directo: _____ V
  - Inverso: OL (infinito) ✓

**Notas de Uso**:
- Usar EN PARALELO con motor para protección flyback
- Cátodo conectado a motor (+) / 5V
- Ánodo conectado a motor (-) / MOSFET drain
- Polaridad inversa respecto al flujo normal de corriente

---

### Potenciómetro 10KΩ

**Especificaciones Nominales**:
- Resistencia: 10KΩ ±20% típico
- Tipo: Rotativo
- Taper: Lineal (confirmado)
- Potencia: 0.125W - 0.25W típico
- Pines: 3 (dos extremos + wiper central)

**Mediciones Reales** (2024-12-28):

- ✅ Test de barrido completado
- **Resistencia total (pin 1 a 3)**: 9.22 KΩ (tolerancia -7.8%, dentro de ±20%)
- **Resistencia mínima (wiper a extremo)**: 1.2 Ω (excelente)
- **Resistencia máxima (wiper a extremo)**: 9.22 KΩ
- **Resistencia en centro**: ~4.87 KΩ (50% del total, confirma taper lineal)
- **Variación suave**: [X] Sí [ ] No
- **Problemas detectados**: [X] Ninguno [ ] Zonas muertas [ ] Ruido

**Pinout**:

```text
Pin 1: Extremo A (conectar a GND en nuestro circuito)
Pin 2: Wiper/Cursor (conectar a Gate MOSFET)
Pin 3: Extremo B (conectar a 5V en nuestro circuito)
```

**Dirección de Rotación**:

- Sentido horario: **Aumenta** resistencia Pin2-Pin1
- Sentido antihorario: **Disminuye** resistencia Pin2-Pin1

**Notas de Uso**:

- Funciona como divisor de voltaje variable
- Salida en wiper: 0-5V según posición (cuando Pin3=5V, Pin1=GND)
- Corriente típica: <0.5mA (despreciable)
- Rotación horaria = más voltaje en wiper = motor más rápido (en configuración final)

---

### Motor DC

**Especificaciones Conocidas**:
- Voltaje nominal: <5V (fabricante)
- Consumo reportado: 60-90mA @ voltaje desconocido
- Tipo: DC brushed motor (con escobillas)

**Mediciones Reales** (2024-12-28):

- ✅ Test directo completado
- **Voltaje de prueba**: 5V
- **Corriente medida**: 84 mA @ 4.985V
- **Voltaje mínimo arranque**: 0.38 V
- **Voltaje mínimo estable**: 0.28 V (ultra-bajo, revoluciones muy lentas)
- **Comportamiento**: [X] Suave [ ] Errático
- **RPM estimado**: Pendiente sensor óptico (Fase 5)
- **Temperatura después de 90s @ 5V**: 24.1°C (delta +1.6°C desde 22.5°C ambiente)

**Características Físicas**:
- Diámetro: _____ mm
- Longitud: _____ mm
- Diámetro eje: _____ mm
- Tipo de conexión: [cables soldados / terminales]
- Polaridad: [ ] Marcada [ ] No marcada

**Curva de Rendimiento** (a completar en Fase 6):
| Voltaje | Corriente | RPM | Notas |
|---------|-----------|-----|-------|
| 2V      | ___ mA    | ___ | _____ |
| 3V      | ___ mA    | ___ | _____ |
| 4V      | ___ mA    | ___ | _____ |
| 5V      | ___ mA    | ___ | _____ |

**Notas de Uso**:
- Carga inductiva: requiere diodo flyback
- No exceder voltaje nominal
- Evitar bloqueos mecánicos (stall condition)

---

### Fuente de Alimentación

**Especificaciones**:
- Modelo: Arduino ELEGO Power Module
- Entrada: USB 5V o Vin hasta 12V
- Salida 1: 5V
- Salida 2: 3.3V
- Corriente máxima total: 100mA (limitada)
- Conexión: USB actual

**Mediciones Reales**:
- Voltaje salida 5V sin carga: _____ V
- Voltaje salida 5V con motor: _____ V (verificar caída)
- Corriente disponible: 100mA (según specs)

**Limitaciones**:
- ⚠️ 100mA puede ser justo para motor 60-90mA
- Sin margen para picos de corriente de arranque
- Considerar fuente externa si motor consume >80mA

**Alternativas** (si es necesario):
- USB power bank (hasta 1-2A)
- Alimentador 5V/1A
- Batería 9V con regulador 7805

---

### Resistencias

**10KΩ (Pull-down para MOSFET gate)**:
- Valor nominal: 10KΩ
- Tolerancia: ±5% (gold band) o ±1% (brown band)
- Potencia: 1/4W (0.25W) típico
- Valor medido: _____ KΩ

**Código de colores** (si aplica):
- 10KΩ ±5%: Marrón-Negro-Naranja-Dorado
- 10KΩ ±1%: Marrón-Negro-Negro-Rojo-Marrón

**Otras resistencias disponibles**:
- [Listar según inventario]

---

### Arduino UNO

**Especificaciones**:
- Microcontrolador: ATmega328P
- Voltaje operación: 5V
- Pines digitales I/O: 14 (6 con PWM)
- Pines analógicos input: 6
- Corriente máxima por pin I/O: 40mA
- Corriente máxima pin 5V: 500mA (limitada por USB o regulador)

**Pines PWM disponibles**: 3, 5, 6, 9, 10, 11
- Frecuencia PWM: ~490Hz (pines 3,9,10,11) o ~980Hz (pines 5,6)

**Motor Shield** (si se usa):
- Modelo: [Pendiente identificar]
- Canales: _____
- Corriente máxima: _____ A

**Notas**:
- USB proporciona máx 500mA total (compartido con Arduino + periféricos)
- Para motor directo desde Arduino, usar pin 5V con precaución

---

## Herramientas de Medición

### FNIRSI DSO-TC3 Component Tester
- Identifica automáticamente: transistores, diodos, resistencias, capacitores
- Mide Vf de diodos
- Mide hFE de transistores
- Útil para verificación rápida de componentes

### Multímetros Disponibles
- Cantidad: [múltiples disponibles según usuario]
- Funciones: VDC, VAC, ADC, AAC, Ω, diodo, continuidad
- Rango: [según modelos]

---

## Componentes Futuros (Para Fases Avanzadas)

### Sensor RPM (Fase 5)
**Opciones**:
1. **IR LED + Fototransistor**
   - Costo: ~€2-3
   - Requiere: marca reflectante en eje motor
   - Precisión: buena
   
2. **Sensor Hall Effect**
   - Costo: ~€3-5
   - Requiere: imán en eje motor
   - Precisión: excelente

3. **Encoder óptico**
   - Costo: ~€5-10
   - Precisión: muy alta
   - Overkill para este proyecto

**Decisión**: [Pendiente según disponibilidad]

---

**Última actualización**: 2024-12-28  
**Componentes verificados**: 1/5 (solo diodo)  
**Pendiente**: Motor, potenciómetro, MOSFET, resistencias
