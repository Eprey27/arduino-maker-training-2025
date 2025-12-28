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
