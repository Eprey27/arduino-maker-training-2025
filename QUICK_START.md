# 🚀 GUÍA DE INICIO RÁPIDO

## Cómo usar este proyecto con tu agente LLM

### Paso 1: Configuración Inicial

1. **Descarga** la carpeta completa `motor-control-learning`
2. **Abre** la carpeta en VS Code
3. **Verifica** que tu agente LLM (Claude Sonnet 4.5 / GitHub Copilot) está activo

### Paso 2: Configurar el Agente

El archivo **`AGENT_INSTRUCTIONS.md`** contiene todas las instrucciones para que el agente te guíe paso a paso.

**Opciones de configuración según tu IDE:**

#### Para Cursor IDE:
```bash
# Renombrar el archivo a .cursorrules
mv AGENT_INSTRUCTIONS.md .cursorrules
```

#### Para GitHub Copilot:
```bash
# Mover a la carpeta .github
mkdir -p .github
mv AGENT_INSTRUCTIONS.md .github/copilot-instructions.md
```

#### Para Claude Code CLI:
- El agente leerá automáticamente `AGENT_INSTRUCTIONS.md` si está en la raíz
- No necesitas renombrar

#### Alternativa Universal:
- Simplemente **pega el contenido** de `AGENT_INSTRUCTIONS.md` en tu primera conversación
- El agente lo tomará como contexto de proyecto

### Paso 3: Primera Interacción

Abre una conversación con tu agente y di:

```
"Hola, soy Emilio. Estoy listo para empezar el proyecto de control de motor 
Arduino. He leído el README. Comencemos con la Fase 0, Test 0.1: prueba 
directa del motor. ¿Qué necesito hacer?"
```

El agente te guiará desde ahí.

### Paso 4: Workflow de Trabajo

1. **El agente te indica** qué test realizar
2. **Tú realizas** el test con hardware real
3. **Reportas** los resultados medidos (voltajes, corrientes, comportamiento)
4. **El agente actualiza** la documentación automáticamente
5. **El agente decide** si puedes avanzar o necesitas investigar más

### 🎯 Estado Actual del Proyecto

**Fase activa**: 0 - Verificación de Componentes
**Tarea actual**: Esperando que realices Test 0.1 (motor directo)
**Siguiente**: Test 0.2 (potenciómetro), luego Test 0.3 (MOSFET)

### 📁 Estructura de Archivos Importantes

```
motor-control-learning/
│
├── AGENT_INSTRUCTIONS.md          ← Lee esto primero (instrucciones para el agente)
├── README.md                       ← Contexto general del proyecto
├── QUICK_START.md                  ← Este archivo
│
├── docs/
│   ├── test-results.md            ← Aquí se registran tus mediciones
│   ├── project-log.md             ← Diario del proyecto
│   ├── learning-notes.md          ← Conceptos aprendidos
│   └── component-specs.md         ← Specs medidas de componentes
│
├── sketches/
│   └── 00_component_tests/
│       ├── motor_direct_test/     ← Primer test (documentación)
│       ├── pot_sweep_test/        ← Segundo test (Arduino)
│       └── mosfet_switch_test/    ← Tercer test (Arduino)
│
├── schematics/
│   └── phase_00_mosfet_switch.txt ← Diagramas ASCII
│
└── hardware/
    └── bill_of_materials.md       ← Inventario de componentes
```

### ⚡ Comandos Útiles para el Agente

Di estas frases para que el agente actúe:

**Reportar resultados:**
```
"He completado el Test 0.1. Aquí están los resultados:
- Voltaje: 5.02V
- Corriente: 75mA
- Motor gira suave, sin ruido"
```

**Pedir ayuda:**
```
"El motor no arranca, ¿qué debería verificar?"
```

**Ver estado:**
```
"¿Cuál es el estado actual del proyecto?"
```

**Saltar adelante (no recomendado):**
```
"Ya verifiqué todos los componentes, quiero pasar a Fase 1"
```

**Ver próximos pasos:**
```
"¿Qué tengo que hacer a continuación?"
```

### 🔧 Antes de Empezar - Checklist

- [ ] Tengo todos los componentes listos (ver `hardware/bill_of_materials.md`)
- [ ] Tengo al menos un multímetro funcional
- [ ] Tengo breadboard y cables de conexión
- [ ] Mi fuente de 5V está lista y verificada
- [ ] He leído el README.md completo
- [ ] Entiendo que vamos paso a paso (sin saltar fases)

### 📚 Recursos de Aprendizaje

Los archivos en `docs/learning-notes.md` se irán llenando con:
- Explicaciones de conceptos (MOSFETs, resistencias pull-down, etc.)
- Analogías fáciles de entender
- Troubleshooting sistemático
- "Aha moments" cuando entiendes algo nuevo

### ⚠️ Reglas del Proyecto

1. **NO saltes tests** - Cada uno valida el siguiente
2. **Documenta TODO** - Mediciones reales, no "funcionó"
3. **Si algo no funciona**, PARA y diagnostica (no sigas adelante)
4. **El agente es tu guía**, pero TÚ haces el trabajo físico
5. **Seguridad primero** - Desconecta al modificar circuitos

### 🆘 Si Algo Sale Mal

1. **Desconecta alimentación**
2. **Describe el problema exactamente** al agente
3. **Proporciona mediciones** (voltajes, corrientes, temperatura)
4. **El agente te ayudará** a diagnosticar sistemáticamente

### 🎓 Filosofía del Proyecto

> "No se trata solo de que funcione, sino de ENTENDER por qué funciona
> y cómo diagnosticar cuando no funciona."

### 📞 Primera Conversación Sugerida

Copia y pega esto en tu agente:

```
Hola, soy Emilio. Acabo de configurar el proyecto "Arduino Motor Control Learning".

He revisado:
✓ README.md
✓ AGENT_INSTRUCTIONS.md
✓ Esta guía (QUICK_START.md)

Tengo listos:
✓ Arduino UNO
✓ Motor DC (60-90mA)
✓ IRF520 MOSFET
✓ Potenciómetro 10K
✓ Diodo 1N4007 (verificado: Vf=0.69V)
✓ Fuente 5V @ 100mA
✓ Multímetros

Estoy listo para empezar con Fase 0, Test 0.1: Prueba directa del motor.

¿Qué necesito hacer primero?
```

---

**¡Buena suerte con tu proyecto!** 

Recuerda: el objetivo es APRENDER, no solo construir. Disfruta el proceso. 🚀

---

**Creado**: 2024-12-28  
**Proyecto**: Arduino Motor Control Learning  
**Versión**: 1.0
