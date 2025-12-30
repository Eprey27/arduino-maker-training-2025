# Calibración del Motor 28BYJ-48

## Valores Calibrados

**Fecha de calibración:** 2025-12-30

### Pasos por Revolución

```cpp
const int STEPS_PER_REVOLUTION = 4096;
```

### Resultado de Verificación

- **Valor probado inicial:** 2048 pasos
- **Resultado:** Se quedó corto (C)
- **Valor ajustado:** 4096 pasos
- **Estado:** Pendiente de verificación final

## Detalles Técnicos

### Motor 28BYJ-48 Especificaciones

- **Motor interno:** 32 pasos por revolución (11.25° por paso en full-step)
- **Reducción de engranajes:** 64:1 (nominal)
- **Modo de operación:** Half-step (8 pasos por secuencia)
- **Secuencia utilizada:** Half-step sequence (8 estados)

### Cálculo Teórico

```
Pasos del motor en half-step = 32 × 2 = 64
Pasos totales con reducción = 64 × 64 = 4096
Grados por paso = 360° / 4096 = 0.088°
```

## Próximos Pasos

1. **Ejecutar sketch 00_steps_verification** con valor 4096
   - Verificar si 5 revoluciones regresan exactamente a la posición inicial
   - Si es exacto → Calibración completa ✓
   - Si se pasa → Reducir a ~4076-4086
   - Si se queda corto → Aumentar a ~4106-4116

2. **Opcional: Ejecutar sketch 05_stepper_calibration**
   - Para ajuste fino si hay desviaciones pequeñas
   - Generará un factor de corrección preciso

3. **Documentar valor final**
   - Actualizar este archivo con el valor definitivo
   - Anotar cualquier observación sobre comportamiento

## Notas de Comportamiento

- **Delay configurado:** 2ms entre pasos
- **Velocidad típica:** ~7.3 RPM (a 2ms delay)
- **Pérdida de pasos:** No observada a 2ms
- **Variaciones:** [Pendiente de documentar]

## Historial de Cambios

| Fecha | Valor | Resultado | Acción |
|-------|-------|-----------|--------|
| 2025-12-30 | 2048 | Se quedó corto | Incrementado a 4096 |
| 2025-12-30 | 4096 | Pendiente | Verificación en proceso |

## Uso en Sketches

Todos los sketches en este directorio han sido actualizados para usar el valor calibrado:

- ✓ `01_basic_stepper_test.ino`
- ✓ `02_stepper_library.ino`
- ✓ `03_stepper_measurement.ino`
- ✓ `04_stepper_advanced_control.ino`
- ✓ `05_stepper_calibration.ino`

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
