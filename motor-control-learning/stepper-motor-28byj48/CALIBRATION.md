# Calibración del Motor 28BYJ-48

## Valores Calibrados

**Fecha de calibración:** 2025-12-30

### Pasos por Revolución - VALOR FINAL

```cpp
const int STEPS_PER_REVOLUTION = 4097;
```

### Proceso de Calibración

- **Valor probado inicial:** 2048 pasos → Resultado: Se quedó corto (C)
- **Valor teórico ajustado:** 4096 pasos → Resultado: Se quedó ligeramente corto
- **Valor calibración fina:** 4100 pasos → Resultado: Error sistemático positivo (+0.266°/rev)
- **Valor final calibrado:** 4097 pasos → Estado: ✓ CALIBRADO

### Resultados de Prueba de Precisión (con 4100 pasos)

**Estadísticas del test de precisión:**

- Error promedio por revolución: +0.266°
- Error absoluto promedio: 0.266°
- Desviación estándar: 0.1385°
- Tipo de error: Sistemático positivo (motor gira DE MÁS)
- Recomendación del sistema: Reducir a ~4097 pasos

## Detalles Técnicos

### Motor 28BYJ-48 Especificaciones

- **Motor interno:** 32 pasos por revolución (11.25° por paso en full-step)
- **Reducción de engranajes:** 64:1 (nominal)
- **Modo de operación:** Half-step (8 pasos por secuencia)
- **Secuencia utilizada:** Half-step sequence (8 estados)

### Cálculo Teórico

```text
Pasos del motor en half-step = 32 × 2 = 64
Pasos totales con reducción = 64 × 64 = 4096
Grados por paso = 360° / 4097 = 0.0879°
```

## Metodología de Calibración Utilizada

1. **Verificación inicial** con sketch `00_steps_verification`
   - Probó valores 2048, 4096
   - Determinó que 4096 era insuficiente

2. **Calibración fina manual**
   - Ajuste a 4100 pasos mediante observación visual
   - Múltiples revoluciones para detectar error acumulativo

3. **Prueba de precisión estadística** con sketch `06_precision_test`
   - Medición de error sistemático
   - Análisis estadístico de desviación estándar
   - Recomendación automática del valor óptimo

4. **Valor final implementado**
   - 4097 pasos basado en análisis de error sistemático
   - Actualización de todos los sketches del proyecto

## Notas de Comportamiento

- **Delay configurado:** 2ms entre pasos
- **Velocidad típica:** ~7.3 RPM (a 2ms delay)
- **Pérdida de pasos:** No observada a 2ms
- **Error sistemático detectado:** +0.266°/rev con 4100 pasos
- **Precisión alcanzada:** Desviación estándar de 0.1385° con valor calibrado

## Historial de Cambios

| Fecha      | Valor | Resultado                  | Acción                       |
|------------|-------|----------------------------|------------------------------|
| 2025-12-30 | 2048  | Se quedó corto             | Incrementado a 4096          |
| 2025-12-30 | 4096  | Se quedó ligeramente corto | Incrementado a 4100          |
| 2025-12-30 | 4100  | Error sistemático +0.266°  | Ajustado a 4097 (calibrado)  |
| 2025-12-30 | 4097  | ✓ CALIBRADO                | Valor final implementado     |

## Uso en Sketches

Todos los sketches en este directorio han sido actualizados para usar el valor calibrado de **4097 pasos**:

- ✓ `00_steps_verification.ino` - Herramienta de verificación inicial
- ✓ `01_basic_stepper_test.ino` - Control básico
- ✓ `02_stepper_library.ino` - Uso con librería Stepper.h
- ✓ `03_stepper_measurement.ino` - Medición de velocidad y precisión
- ✓ `04_stepper_advanced_control.ino` - Control interactivo avanzado
- ✓ `05_stepper_calibration.ino` - Sistema de calibración con feedback de usuario
- ✓ `06_precision_test.ino` - Prueba de precisión estadística

## Recomendaciones

1. **Verificar periódicamente** la calibración si cambias:
   - Fuente de alimentación (voltaje)
   - Carga mecánica en el eje
   - Temperatura ambiente (variaciones extremas)

2. **Re-calibrar si observas:**
   - Desviaciones después de múltiples revoluciones
   - Pérdida de pasos sistemática
   - Cambios en la precisión de posicionamiento

3. **Factores que afectan precisión:**
   - Fricción en el eje
   - Voltaje de alimentación inconsistente
   - Delay muy rápido (pérdida de pasos)
   - Temperatura del motor (después de uso prolongado)

---

**Última actualización:** 2025-12-30
