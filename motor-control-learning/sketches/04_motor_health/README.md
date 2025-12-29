# Motor Health Monitoring System

Sistema completo de diagnóstico y monitoreo de salud para motores DC.

## 📂 Estructura del Módulo

```
04_motor_health/
├── README.md                           # Este archivo
├── 00_individual_tests/                # Tests de sensores individuales
│   ├── rpm_sensor_test/               # Test TCRT5000 (RPM)
│   ├── current_sensor_test/           # Test INA219 (corriente/voltaje)
│   ├── temp_sensor_test/              # Test MLX90614 + DHT22
│   ├── vibration_sensor_test/         # Test ADXL345
│   └── sound_sensor_test/             # Test KY-038
└── 01_integrated_monitoring/           # Sistema integrado
    └── motor_health_analyzer/         # Analizador completo
```

## 🎯 Propósito

Este sistema permite evaluar la salud de un motor DC antes de usarlo en tareas exigentes, midiendo:

1. **RPM** - Velocidad y estabilidad
2. **Corriente/Voltaje** - Consumo energético y eficiencia
3. **Temperatura** - Calentamiento y fricción
4. **Vibraciones** - Desbalanceo y desgaste mecánico
5. **Ruido** - Desgaste de escobillas y rodamientos

## 🚀 Inicio Rápido

### Paso 1: Tests Individuales

Empieza probando cada sensor por separado en orden:

1. **current_sensor_test** - Más importante, prueba primero
   - Verifica consumo de corriente del motor
   - Establece baseline de consumo

2. **rpm_sensor_test** - Segundo más importante
   - Mide velocidad del motor
   - Requiere marca reflectante en eje

3. **temp_sensor_test** - Detecta sobrecalentamiento
   - Mide temperatura motor vs ambiente
   - Calcula Delta-T

4. **vibration_sensor_test** - Detecta problemas mecánicos
   - CRÍTICO: montaje rígido del sensor
   - Calibra baseline con motor apagado

5. **sound_sensor_test** - Detecta desgaste acústico
   - Ajusta sensibilidad del micrófono
   - Captura baseline de ruido ambiente

### Paso 2: Sistema Integrado

Una vez que todos los sensores funcionan individualmente:

1. Sube **motor_health_analyzer.ino**
2. Abre Serial Monitor (115200 baud)
3. Envía comando `d` para iniciar diagnóstico
4. Espera ~5 minutos
5. Revisa el reporte de salud

## 📊 Interpretación de Resultados

### Health Score (Puntuación de Salud)

- **90-100**: EXCELENTE - Motor perfecto, usar sin restricciones
- **70-89**: BUENO - Desgaste normal, monitorear periódicamente
- **50-69**: REGULAR - Desgaste elevado, planear mantenimiento
- **30-49**: MALO - Problemas significativos, limitar uso
- **0-29**: CRÍTICO - No usar para tareas exigentes

### Parámetros Normales (Motor pequeño 5V)

| Parámetro | Valor Normal | Advertencia | Crítico |
|-----------|-------------|-------------|---------|
| Corriente @ 5V | 60-90mA | 90-120mA | >120mA |
| Delta-T (5 min) | <10°C | 10-15°C | >15°C |
| Vibración | <0.3 m/s² | 0.3-0.5 m/s² | >0.5 m/s² |
| RPM variación | <5% | 5-10% | >10% |

## 🔌 Conexiones Hardware

### Pines I2C (compartidos)
```
Arduino A4 (SDA) ──┬── INA219 SDA
                   ├── ADXL345 SDA
                   └── MLX90614 SDA

Arduino A5 (SCL) ──┬── INA219 SCL
                   ├── ADXL345 SCL
                   └── MLX90614 SCL
```

### Direcciones I2C
- `0x40` - INA219 (corriente/voltaje)
- `0x53` - ADXL345 (acelerómetro)
- `0x5A` - MLX90614 (temperatura IR)

### Pines Individuales
- `D2` - TCRT5000 Digital Out (RPM, con interrupción)
- `D4` - DHT22 Data (temperatura ambiente)
- `D9` - Motor PWM control
- `A0` - KY-038 Analog (sonido)

## 📚 Librerías Necesarias

Instalar vía Arduino IDE > Tools > Manage Libraries:

```
- Adafruit INA219          (corriente/voltaje)
- Adafruit MLX90614        (temperatura IR)
- Adafruit ADXL345         (acelerómetro)
- Adafruit Unified Sensor  (dependencia)
- DHT sensor library       (temperatura ambiente)
```

## 🛒 Hardware Necesario

Ver lista completa en: [motor-health-sensors-bom.md](../../hardware/motor-health-sensors-bom.md)

### Sensores Esenciales (~€18)
- INA219 - Corriente/voltaje (€4)
- TCRT5000 - RPM (€1)
- ADXL345 - Vibración (€3)
- DHT22 - Temp ambiente (€4)
- KY-038 - Sonido (€1)
- Montaje/cables (€5)

### Opcional pero Recomendado
- MLX90614 - Temp IR motor (€10) - Mejor que contacto
- OLED Display 0.96" (€4) - Visualización en tiempo real
- SD Card Module (€2) - Logging de datos

## 🔧 Solución de Problemas

### Sensor no detectado en I2C

**Síntoma**: "Sensor not found" en Serial

**Soluciones**:
1. Verifica conexiones SDA/SCL
2. Ejecuta I2C Scanner sketch para verificar dirección
3. Añade resistencias pull-up 4.7kΩ en SDA y SCL
4. Algunos módulos solo funcionan a 3.3V

### RPM siempre 0

**Síntoma**: No cuenta revoluciones

**Soluciones**:
1. Añade marca reflectante blanca en eje motor
2. Ajusta potenciómetro de sensibilidad en TCRT5000
3. Verifica distancia sensor-eje (2-5mm óptimo)
4. Revisa conexión a pin D2 (INT0)
5. Usa sketch `rpm_sensor_test` para calibrar

### Vibración muy alta incluso motor apagado

**Síntoma**: Lecturas de vibración erróneas

**Soluciones**:
1. **CRÍTICO**: ADXL345 debe estar montado RÍGIDAMENTE
2. NO usar breadboard (demasiado flexible)
3. Montar en plataforma rígida (madera/plástico duro)
4. Recalibrar baseline con motor OFF
5. Verificar Z-axis ~9.8 m/s² cuando nivel

### Temperatura no sube

**Síntoma**: Temp motor = ambiente siempre

**Soluciones MLX90614**:
1. Sensor apuntando a superficie reflectiva (lee ambiente)
2. Apuntar a parte mate/oscura del motor
3. Verificar distancia 5-10cm
4. Añadir cinta mate al motor si es muy reflectivo

**Soluciones DS18B20** (alternativa):
1. Asegurar buen contacto térmico
2. Usar pasta térmica
3. Fijar con cinta térmica

### Corriente negativa o cero

**Síntoma**: INA219 lee mal

**Soluciones**:
1. Verificar polaridad: Vin+ al supply, Vin- al motor
2. Si negativo: invertir Vin+ y Vin-
3. Verificar motor está realmente funcionando
4. Probar diferente calibración: `setCalibration_32V_1A()`

## 📖 Documentación Completa

- [Motor Health Monitoring System](../../docs/motor-health-monitoring.md) - Diseño completo
- [Sensor BOM](../../hardware/motor-health-sensors-bom.md) - Lista de compra
- Sketches individuales - Comentarios detallados en cada .ino

## 🎓 Conceptos Aprendidos

Al completar este módulo aprenderás:

- **Comunicación I2C**: Multi-dispositivo, direcciones, pull-ups
- **Interrupciones**: Medición precisa de RPM
- **Muestreo de señales**: Aliasing, Nyquist, averaging
- **Análisis multiparamétrico**: Correlación de datos
- **Diagnóstico sistemático**: Establecer baselines, detectar anomalías
- **Sensores analógicos vs digitales**: Ventajas/desventajas
- **Montaje crítico**: Rigidez para acelerómetros
- **Thermal management**: Delta-T, estabilización térmica

## 🚦 Workflow Recomendado

### Para Principiantes

1. ✅ Test individual de INA219 (1 día)
2. ✅ Test individual de TCRT5000 (1 día)
3. ✅ Test individual de DHT22 (1 día)
4. ✅ Test individual de ADXL345 (1 día, crítico montaje)
5. ✅ Test individual de KY-038 (1 día)
6. ⏸️ Pausa: Analizar resultados individuales
7. ✅ Sistema integrado (2-3 días)
8. ✅ Validación con motor nuevo vs usado

**Tiempo total estimado**: 1-2 semanas (part-time)

### Para Avanzados

1. ✅ Todos los tests individuales en paralelo (2-3 días)
2. ✅ Sistema integrado (1 día)
3. ✅ Añadir SD logging (1 día)
4. ✅ Añadir OLED display (1 día)
5. ✅ FFT analysis de vibraciones (avanzado)

## 🔬 Proyectos Futuros

Ideas para expandir el sistema:

- [ ] **FFT Analysis**: Análisis de frecuencia de vibraciones
- [ ] **Machine Learning**: Predicción de fallos
- [ ] **Wireless Monitoring**: ESP32 + MQTT
- [ ] **Database Logging**: InfluxDB + Grafana
- [ ] **Mobile App**: Control desde smartphone
- [ ] **Multi-motor**: Comparación simultánea
- [ ] **Predictive Maintenance**: Alertas tempranas

## 📞 Soporte

Si encuentras problemas:

1. Revisa comentarios en los sketches (.ino files)
2. Consulta sección de troubleshooting
3. Verifica conexiones con diagrama
4. Prueba sensores individualmente primero
5. Usa I2C scanner para debug I2C

## 📄 Licencia

MIT License - Uso educativo y personal libre

---

**Creado**: 2024-12-29
**Versión**: 1.0
**Autor**: Emilio (con asistencia de Claude Sonnet 4.5)
**Estado**: Completo y listo para usar
