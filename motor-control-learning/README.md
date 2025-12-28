# Arduino Motor Control Learning Project

## 🎯 Objetivo del Proyecto

Aprender control de motores DC mediante experimentación metódica con Arduino, construyendo desde componentes individuales hasta un sistema completo de control de velocidad con métricas de rendimiento.

## 📋 Descripción

Este es un proyecto educativo estructurado en fases, donde cada fase se basa en verificaciones de la anterior. El enfoque es **entender el porqué** de cada componente y decisión de diseño, no solo copiar circuitos.

## 🔧 Hardware Necesario

- Arduino UNO
- Motor DC <5V (60-90mA medido)
- MOSFET IRF520
- Potenciómetro 10KΩ
- Diodo 1N4007
- Resistencia 10KΩ (pull-down)
- Fuente 5V @ 100mA
- Multímetros/testers

## 📚 Estructura del Proyecto

```
motor-control-learning/
├── AGENT_INSTRUCTIONS.md      # Instrucciones para el agente LLM
├── README.md                   # Este archivo
├── docs/                       # Documentación
│   ├── project-log.md         # Registro cronológico
│   ├── component-specs.md     # Especificaciones medidas
│   ├── test-results.md        # Resultados de pruebas
│   └── learning-notes.md      # Notas de aprendizaje
├── sketches/                   # Código Arduino
│   ├── 00_component_tests/    # Tests de componentes individuales
│   ├── 01_motor_basic/        # Control básico motor
│   ├── 02_pwm_control/        # Control PWM
│   └── 03_rpm_counter/        # Contador de RPM
├── schematics/                 # Diagramas de circuitos
└── hardware/                   # Specs de hardware
    └── bill_of_materials.md
```

## 🚀 Fases del Proyecto

### ✅ Fase 0: Verificación de Componentes (ACTUAL)
Probar cada componente individualmente antes de integración

### ⏳ Fase 1: Control MOSFET Básico
Motor on/off con MOSFET (sin potenciómetro)

### ⏳ Fase 2: Control PWM
Velocidad variable usando PWM de Arduino

### ⏳ Fase 3: Control Analógico
Reemplazar PWM con potenciómetro (circuito puramente analógico)

### ⏳ Fase 4: Protección con Diodo
Añadir diodo flyback para protección

### ⏳ Fase 5: Contador RPM
Medir revoluciones del motor

### ⏳ Fase 6: Análisis de Rendimiento
Caracterización completa del sistema

## 📖 Metodología

1. **Probar componentes individualmente** ANTES de integrar
2. **Documentar resultados esperados** ANTES de realizar pruebas
3. **Medir y registrar** valores reales (voltaje, corriente, comportamiento)
4. **Analizar discrepancias** entre esperado y real
5. **Solo avanzar** cuando la fase actual esté verificada

## 🎓 Objetivos de Aprendizaje

- Entender operación de MOSFETs y control de gate
- Comprender resistencias pull-down
- Diferenciar conexiones serie vs paralelo
- Leer esquemáticos básicos
- Protección con diodos flyback
- Debug sistemático de circuitos
- Uso correcto de multímetro

## 📝 Estado Actual

**Fase activa**: 0 - Verificación de Componentes
**Tarea actual**: Esperando resultados de prueba directa del motor
**Próximo paso**: Test de barrido del potenciómetro

## 🛠️ Cómo Usar Este Proyecto con LLM Agent

1. Abre este proyecto en VS Code
2. Asegúrate de que el agente (Claude/Copilot) puede leer `AGENT_INSTRUCTIONS.md`
3. El agente te guiará fase por fase
4. Realiza las pruebas que te indique
5. Reporta resultados medidos
6. El agente actualizará documentación automáticamente

## ⚠️ Seguridad

- 5V es seguro, pero desconecta al modificar circuitos
- Verifica polaridad antes de conectar
- Vigila temperatura de componentes
- No excedas ratings de componentes

## 📊 Progreso

- [x] Estructura de proyecto creada
- [x] Componentes inventariados
- [ ] Motor verificado (prueba directa)
- [ ] Potenciómetro verificado
- [ ] MOSFET verificado
- [ ] Control básico funcionando
- [ ] Control con potenciómetro funcionando
- [ ] Diodo protección añadido
- [ ] Contador RPM implementado
- [ ] Análisis rendimiento completado

---

**Creado**: 2024-12-28  
**Autor**: Emilio (con asistencia de Claude Sonnet 4.5)  
**Licencia**: MIT (para propósitos educativos)
