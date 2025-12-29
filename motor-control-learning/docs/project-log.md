# Project Log - Motor Control Learning

## 2024-12-28

### Inicio del Proyecto

**Hora**: Tarde

**Actividad**: Configuración inicial del proyecto

**Decisiones tomadas**:
- Estructura de proyecto definida con metodología de testing por fases
- Sistema de documentación establecido
- Inventario de componentes realizado
- Agente LLM configurado con instrucciones completas

**Componentes confirmados**:
- IRF520 MOSFET (2 unidades disponibles)
- Motor DC: consumo medido 60-90mA @ voltaje desconocido
- Fuente alimentación: 5V @ 100mA max
- Potenciómetro 10K
- Diodo 1N4007 verificado con component tester (Vf=0.69V)

**Problemas identificados**:
- En montaje previo, motor siempre activo (posible conexión incorrecta)
- Necesidad de verificar componentes individualmente antes de remontar

**Próximos pasos**:
1. Test directo del motor (conectar a 5V/GND)
2. Medir consumo real del motor
3. Verificar voltaje mínimo de arranque
4. Test de barrido del potenciómetro
5. Test de switching del MOSFET

**Notas**:
- Estudiante tiene experiencia en software, no en electrónica
- Enfoque metodológico apreciado
- Múltiples testers disponibles para verificaciones

---

### Test 0.1: Motor Directo - COMPLETADO ✅

**Hora**: Tarde

**Actividad**: Verificación individual del motor DC

**Fase actual**: Fase 0 - Verificación de Componentes

**Tests realizados**:

- [X] Conexión directa motor a fuente 5V/GND
- [X] Medición de corriente de consumo
- [X] Medición de voltaje en terminales
- [X] Observación de comportamiento durante 90 segundos
- [X] Test de voltaje mínimo de arranque

**Mediciones**:

- **Voltaje en terminales**: 4.985 V
- **Corriente consumo**: 84 mA
- **Temperatura inicial**: 22.5°C
- **Temperatura después de 90s**: 24.1°C (delta +1.6°C)
- **Voltaje mínimo arranque**: 0.38 V
- **Voltaje mínimo estable**: 0.28 V (ultra-bajo RPM)

**Resultados**:

- ✅ Motor funciona excepcionalmente bien
- ✅ Giro suave y estable en todo el rango de voltajes
- ✅ Sin vibraciones ni ruidos anormales
- ✅ Incremento de temperatura muy bajo (excelente eficiencia)
- ✅ Consumo dentro del rango esperado (60-90mA)

**Aprendizajes**:

- El motor tiene excelente calidad: puede funcionar establemente desde 0.28V hasta 5V
- El consumo de 84mA deja poco margen con la fuente de 100mA
- Los picos de arranque podrían superar el límite de la fuente
- La capacidad de operar a voltajes tan bajos será ideal para caracterización

**Decisiones tomadas**:

- ✅ Motor verificado y aprobado para el proyecto
- ⚠️ Considerar fuente externa para fases avanzadas (para evitar límites de corriente)
- 📝 Apuntado para Fase 5/6: Implementar contador óptico de RPM para generar gráfica Voltaje vs RPM vs Corriente

**Próximos pasos**:

1. Test 0.2: Verificación del potenciómetro 10K
2. Test 0.3: Verificación del MOSFET IRF520
3. Completar Fase 0 antes de avanzar a integración de circuitos

**Notas adicionales**:

- Estudiante realizó mediciones muy completas y metódicas
- Incluyó medición de temperatura con precisión (termómetro digital)
- Proactivamente testeó voltajes mínimos sin que se le pidiera explícitamente
- Expresó interés en crear gráficas de rendimiento (motivación para Fase 5/6)

---

### Test 0.2: Potenciómetro Barrido - COMPLETADO ✅

**Hora**: Tarde

**Actividad**: Verificación individual del potenciómetro 10KΩ

**Fase actual**: Fase 0 - Verificación de Componentes

**Tests realizados**:

- [X] Medición de resistencia total (Pin1-Pin3)
- [X] Medición en extremo A (Pin2-Pin1 y Pin2-Pin3)
- [X] Medición en posición central
- [X] Medición en extremo B (Pin2-Pin1 y Pin2-Pin3)
- [X] Test de suavidad al girar
- [X] Identificación de dirección de rotación

**Mediciones**:

| Posición | Pin1-Pin3 | Pin2-Pin1 | Pin2-Pin3 |
|----------|-----------|-----------|-----------|
| Extremo A | 9.22 KΩ | 1.2 Ω | 9.23 KΩ |
| Centro | 9.22 KΩ | 4.875 KΩ | 4.621 KΩ |
| Extremo B | 9.22 KΩ | 9.22 KΩ | 0.9 Ω |

**Análisis**:

- Resistencia total: **9.22 KΩ** (tolerancia -7.8% del valor nominal, dentro de ±20% especificación)
- Suma en centro: 4.875 + 4.621 = 9.496 KΩ ≈ 9.22 KΩ (pequeña diferencia por precisión de multímetro)
- Resistencia residual <2Ω en extremos (calidad excelente)
- Taper **lineal confirmado** (centro ≈ 50%)

**Resultados**:

- ✅ Potenciómetro funciona perfectamente
- ✅ Variación suave y continua, sin saltos
- ✅ Sin zonas muertas ni ruido al girar
- ✅ Taper lineal ideal para control de velocidad
- ✅ Resistencia residual muy baja en extremos

**Aprendizajes**:

- El potenciómetro funciona como divisor de voltaje variable
- Con Pin1=GND y Pin3=5V, el wiper (Pin2) entrega 0-5V según posición
- Rotación horaria aumenta resistencia Pin2-Pin1, por tanto aumenta voltaje en wiper
- Taper lineal significa que la resistencia (y voltaje) cambia proporcionalmente con el ángulo de giro
- La configuración correcta es: Pin1→GND, Pin2→Gate, Pin3→5V

**Decisiones tomadas**:

- ✅ Potenciómetro verificado y aprobado para el proyecto
- 📝 Configuración identificada para circuito final:
  - Pin 1 (Extremo A) → GND
  - Pin 2 (Wiper) → MOSFET Gate
  - Pin 3 (Extremo B) → 5V
- 📝 Rotación horaria = más voltaje = motor más rápido

**Próximos pasos**:

1. Test 0.3: Verificación del MOSFET IRF520
2. Completar Fase 0 antes de avanzar a integración de circuitos

**Notas adicionales**:

- Estudiante preguntó sobre función de cada pin y uso general del potenciómetro
- Necesita explicación conceptual de cómo funciona como divisor de voltaje
- Todas las mediciones fueron precisas y completas

---

## Plantilla para Nuevas Entradas

```markdown
## YYYY-MM-DD

### [Título de la Sesión]

**Hora**: [mañana/tarde/noche]

**Actividad**: [descripción breve]

**Fase actual**: [número y nombre]

**Tests realizados**:
- [ ] Test 1: [resultado]
- [ ] Test 2: [resultado]

**Mediciones**:
- Voltaje: [valor]
- Corriente: [valor]
- Temperatura: [observación]

**Problemas encontrados**:
- [descripción]

**Soluciones aplicadas**:
- [descripción]

**Aprendizajes**:
- [concepto aprendido]

**Próximos pasos**:
1. [acción 1]
2. [acción 2]

**Notas adicionales**:
- [observaciones]
```
