# Test Results - Motor Control Learning Project

## Índice de Tests

- [Fase 0: Verificación de Componentes](#fase-0-verificación-de-componentes)
  - [Test 0.1: Motor Directo](#test-01-motor-directo)
  - [Test 0.2: Potenciómetro Barrido](#test-02-potenciómetro-barrido)
  - [Test 0.3: MOSFET Switching](#test-03-mosfet-switching)
  - [Test 0.4: Diodo Verificación](#test-04-diodo-verificación)

---

## Fase 0: Verificación de Componentes

### Test 0.1: Motor Directo

**Objetivo**: Verificar que el motor funciona correctamente y medir su consumo real

**Fecha**: [Pendiente]

**Setup**:
```
5V Fuente (+) ──→ Motor (+)
Motor (-) ──→ GND Fuente
Multímetro en serie para medir corriente
```

**Predicciones**:
- Motor debería arrancar y girar suavemente
- Consumo esperado: 60-90mA según especificaciones previas
- Voltaje en bornes del motor: ~5V

**Resultados Medidos**:
- Voltaje motor: _____ V
- Corriente consumo: _____ mA
- Comportamiento: [ ] Suave [ ] Errático [ ] No arranca
- Temperatura después de 30s: [ ] Frío [ ] Tibio [ ] Caliente
- Ruido: [ ] Silencioso [ ] Normal [ ] Ruidoso

**Observaciones**:
[Espacio para notas del estudiante]

**Voltaje Mínimo de Arranque**:
- Reducir voltaje gradualmente hasta que motor se detenga
- Voltaje mínimo medido: _____ V

**Conclusión**: [ ] PASS [ ] FAIL [ ] NEEDS INVESTIGATION

**Notas**:
[Análisis de resultados]

---

### Test 0.2: Potenciómetro Barrido

**Objetivo**: Verificar que el potenciómetro tiene variación suave de 0-10K

**Fecha**: [Pendiente]

**Setup**:
- Multímetro en modo resistencia (Ω)
- Medir entre diferentes pines mientras se gira

**Predicciones**:
- Pin 1 a Pin 3: ~10KΩ constante (no varía al girar)
- Pin central a extremos: varía 0-10K según posición

**Resultados Medidos**:

| Posición Pot | Pin1-Pin3 | Pin2-Pin1 | Pin2-Pin3 | Notas |
|--------------|-----------|-----------|-----------|-------|
| Extremo A    | _____ Ω   | _____ Ω   | _____ Ω   |       |
| Centro       | _____ Ω   | _____ Ω   | _____ Ω   |       |
| Extremo B    | _____ Ω   | _____ Ω   | _____ Ω   |       |

**Dirección de rotación**:
- Sentido horario: [ ] Aumenta [ ] Disminuye resistencia Pin2-Pin1
- Sentido antihorario: [ ] Aumenta [ ] Disminuye resistencia Pin2-Pin1

**Problemas detectados**:
- [ ] Zonas muertas
- [ ] Saltos en la resistencia
- [ ] Ruido/scratching al girar
- [ ] Ninguno

**Conclusión**: [ ] PASS [ ] FAIL [ ] NEEDS INVESTIGATION

**Notas**:
[Análisis de resultados]

---

### Test 0.3: MOSFET Switching

**Objetivo**: Verificar que el IRF520 puede encender/apagar el motor correctamente

**Fecha**: [Pendiente]

**Setup**:
```
5V ──→ Motor (+)
Motor (-) ──→ DRAIN (pin 2, centro)
SOURCE (pin 3, derecha) ──→ GND
GATE (pin 1, izquierda) ──→ [variable]
```

**Predicciones**:
- Gate flotante o a GND: Motor APAGADO
- Gate a 5V: Motor ENCENDIDO

**Test 1: Gate Flotante (sin conexión)**
- Estado motor: [ ] Apagado [ ] Encendido [ ] Errático
- Voltaje Gate-Source: _____ V
- Corriente motor: _____ mA

**Test 2: Gate a GND (a través de resistencia 10K)**
- Estado motor: [ ] Apagado [ ] Encendido [ ] Errático
- Voltaje Gate-Source: _____ V
- Corriente motor: _____ mA

**Test 3: Gate a 5V (conectado directamente)**
- Estado motor: [ ] Apagado [ ] Encendido [ ] Errático
- Voltaje Gate-Source: _____ V
- Corriente motor: _____ mA
- Temperatura MOSFET después de 30s: [ ] Frío [ ] Tibio [ ] Caliente

**Test 4: Voltaje Umbral (Vgs threshold)**
- Reducir voltaje de gate gradualmente
- Voltaje donde motor empieza a reducir velocidad: _____ V
- Voltaje donde motor se detiene completamente: _____ V

**Conclusión**: [ ] PASS [ ] FAIL [ ] NEEDS INVESTIGATION

**Notas**:
[¿El IRF520 funciona adecuadamente con 5V? ¿Se calienta mucho?]

---

### Test 0.4: Diodo Verificación

**Objetivo**: Confirmar que el diodo 1N4007 funciona correctamente

**Fecha**: 2024-12-28 (COMPLETADO)

**Herramienta**: FNIRSI DSO-TC3 Component Tester

**Resultados Medidos**:
- Vf (Forward voltage): **0.69V** ✓
- Corriente forward: 73nA
- Tipo: Diodo rectificador confirmado

**Verificación con multímetro** (modo diodo):
- Dirección directa (ánodo +, cátodo -): _____ V
- Dirección inversa (cátodo +, ánodo -): [ ] OL/Infinito

**Conclusión**: ✅ PASS

**Notas**: Diodo funciona correctamente. Vf de 0.69V es normal para 1N4007.

---

## Plantilla para Nuevos Tests

```markdown
### Test X.Y: [Nombre del Test]

**Objetivo**: [Qué estamos verificando]

**Fecha**: [YYYY-MM-DD]

**Setup**:
```
[Diagrama ASCII del circuito]
```

**Predicciones**:
- [Qué esperamos que pase]

**Resultados Medidos**:
- [Mediciones reales]

**Observaciones**:
- [Notas durante el test]

**Conclusión**: [ ] PASS [ ] FAIL [ ] NEEDS INVESTIGATION

**Notas**:
- [Análisis y aprendizajes]
```
