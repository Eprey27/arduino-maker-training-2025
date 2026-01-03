/*
 * Experimento 8: Verificación del Sensor AS5600
 *
 * OBJETIVO:
 * - Verificar comunicación I2C con el sensor AS5600
 * - Leer ángulos en tiempo real con alta precisión
 * - Validar resolución y precisión del sensor
 * - Mostrar ángulos en formato Grados/Minutos/Segundos
 * - Detectar el imán y verificar intensidad de campo magnético
 *
 * HARDWARE REQUERIDO:
 * - Arduino UNO o ESP32
 * - Sensor AS5600 (Encoder Magnético I2C)
 * - Imán diametral (incluido con AS5600)
 * - Display OLED I2C (opcional)
 *
 * CONEXIONES AS5600:
 * AS5600 VCC  -> Arduino 5V (o 3.3V para ESP32)
 * AS5600 GND  -> Arduino GND
 * AS5600 SDA  -> Arduino A4 (UNO) / GPIO21 (ESP32)
 * AS5600 SCL  -> Arduino A5 (UNO) / GPIO22 (ESP32)
 * AS5600 DIR  -> GND (sentido horario) o VCC (antihorario)
 *
 * MONTAJE DEL IMÁN:
 * - Distancia del imán al sensor: 0.5mm - 3mm
 * - Imán centrado sobre el sensor AS5600
 * - Polo Norte hacia abajo (hacia el sensor)
 *
 * LIBRERÍAS REQUERIDAS:
 * - Wire (incluida en Arduino IDE)
 * - AS5600 (instalar desde Library Manager)
 *   O usar implementación I2C directa en este sketch
 */

#include <Wire.h>

// ============================================
// Direcciones y Registros del AS5600
// ============================================

#define AS5600_ADDRESS 0x36  // Dirección I2C del AS5600

// Registros de configuración
#define AS5600_ZMCO        0x00  // Contador de programaciones permanentes
#define AS5600_ZPOS_H      0x01  // Posición cero high byte
#define AS5600_ZPOS_L      0x02  // Posición cero low byte
#define AS5600_MPOS_H      0x03  // Posición máxima high byte
#define AS5600_MPOS_L      0x04  // Posición máxima low byte
#define AS5600_MANG_H      0x05  // Ángulo máximo high byte
#define AS5600_MANG_L      0x06  // Ángulo máximo low byte
#define AS5600_CONF_H      0x07  // Configuración high byte
#define AS5600_CONF_L      0x08  // Configuración low byte

// Registros de salida
#define AS5600_RAW_ANGLE_H 0x0C  // Ángulo raw high byte
#define AS5600_RAW_ANGLE_L 0x0D  // Ángulo raw low byte
#define AS5600_ANGLE_H     0x0E  // Ángulo procesado high byte
#define AS5600_ANGLE_L     0x0F  // Ángulo procesado low byte

// Registros de estado
#define AS5600_STATUS      0x0B  // Estado del sensor
#define AS5600_AGC         0x1A  // Control automático de ganancia
#define AS5600_MAGNITUDE_H 0x1B  // Magnitud del campo magnético high
#define AS5600_MAGNITUDE_L 0x1C  // Magnitud del campo magnético low

// Bits de estado
#define AS5600_STATUS_MH   0x08  // Magnet High - imán demasiado fuerte
#define AS5600_STATUS_ML   0x10  // Magnet Low - imán demasiado débil
#define AS5600_STATUS_MD   0x20  // Magnet Detected - imán detectado

// ============================================
// Estructura para mediciones angulares
// ============================================

struct AngleMeasurement {
  uint16_t rawValue;        // Valor raw del sensor (0-4095)
  float degrees;            // Grados decimales (0-360)
  int deg;                  // Grados enteros (0-359)
  int minutes;              // Minutos de arco (0-59)
  float seconds;            // Segundos de arco (0-59.99)
  unsigned long timestamp;  // Timestamp en milisegundos
};

// Variables globales
AngleMeasurement currentAngle;
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 100;  // Actualizar cada 100ms

// ============================================
// Funciones del AS5600
// ============================================

// Inicializar comunicación I2C y verificar sensor
bool initAS5600() {
  Wire.begin();
  Wire.setClock(400000);  // I2C a 400kHz (Fast Mode)

  delay(10);

  // Verificar comunicación I2C
  Wire.beginTransmission(AS5600_ADDRESS);
  byte error = Wire.endTransmission();

  if (error != 0) {
    Serial.println(F("ERROR: AS5600 no detectado en I2C"));
    Serial.print(F("Codigo de error I2C: "));
    Serial.println(error);
    return false;
  }

  Serial.println(F("OK: AS5600 detectado correctamente"));
  return true;
}

// Leer un registro de 8 bits
uint8_t readRegister8(uint8_t reg) {
  Wire.beginTransmission(AS5600_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(AS5600_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

// Leer un registro de 16 bits (high + low byte)
uint16_t readRegister16(uint8_t regHigh) {
  uint8_t highByte = readRegister8(regHigh);
  uint8_t lowByte = readRegister8(regHigh + 1);
  return ((uint16_t)highByte << 8) | lowByte;
}

// Leer ángulo raw (0-4095)
uint16_t readRawAngle() {
  return readRegister16(AS5600_RAW_ANGLE_H) & 0x0FFF;  // 12 bits
}

// Leer ángulo procesado (con offset y configuración aplicada)
uint16_t readAngle() {
  return readRegister16(AS5600_ANGLE_H) & 0x0FFF;  // 12 bits
}

// Leer estado del sensor
uint8_t readStatus() {
  return readRegister8(AS5600_STATUS);
}

// Leer AGC (Automatic Gain Control)
uint8_t readAGC() {
  return readRegister8(AS5600_AGC);
}

// Leer magnitud del campo magnético
uint16_t readMagnitude() {
  return readRegister16(AS5600_MAGNITUDE_H) & 0x0FFF;  // 12 bits
}

// ============================================
// Funciones de conversión angular
// ============================================

// Convertir valor raw (0-4095) a grados decimales (0-360)
float rawToDegrees(uint16_t rawValue) {
  return (rawValue * 360.0) / 4096.0;
}

// Convertir grados decimales a formato DMS (Degrees, Minutes, Seconds)
void decimalToDMS(float decimal, int &deg, int &min, float &sec) {
  deg = (int)decimal;
  float remainder = (decimal - deg) * 60.0;
  min = (int)remainder;
  sec = (remainder - min) * 60.0;
}

// Leer medición completa con conversión DMS
AngleMeasurement readPreciseAngle() {
  AngleMeasurement measurement;

  measurement.rawValue = readRawAngle();
  measurement.degrees = rawToDegrees(measurement.rawValue);
  measurement.timestamp = millis();

  decimalToDMS(measurement.degrees, measurement.deg, measurement.minutes, measurement.seconds);

  return measurement;
}

// ============================================
// Funciones de diagnóstico
// ============================================

void printMagnetStatus() {
  uint8_t status = readStatus();
  uint16_t magnitude = readMagnitude();
  uint8_t agc = readAGC();

  Serial.println(F("\n=== ESTADO DEL IMAN ==="));

  // Detectar estado del imán
  bool magnetDetected = status & AS5600_STATUS_MD;
  bool magnetTooStrong = status & AS5600_STATUS_MH;
  bool magnetTooWeak = status & AS5600_STATUS_ML;

  Serial.print(F("Iman detectado: "));
  if (magnetDetected) {
    Serial.println(F("SI"));
  } else {
    Serial.println(F("NO - VERIFICAR POSICION DEL IMAN"));
  }

  if (magnetTooStrong) {
    Serial.println(F("ADVERTENCIA: Iman demasiado FUERTE - alejar del sensor"));
  }

  if (magnetTooWeak) {
    Serial.println(F("ADVERTENCIA: Iman demasiado DEBIL - acercar al sensor"));
  }

  if (magnetDetected && !magnetTooStrong && !magnetTooWeak) {
    Serial.println(F("Estado del iman: OPTIMO"));
  }

  Serial.print(F("Magnitud campo magnetico: "));
  Serial.print(magnitude);
  Serial.print(F(" ("));

  // Rangos óptimos: 1500-3000
  if (magnitude < 1000) {
    Serial.print(F("MUY BAJO"));
  } else if (magnitude < 1500) {
    Serial.print(F("BAJO"));
  } else if (magnitude <= 3000) {
    Serial.print(F("OPTIMO"));
  } else if (magnitude <= 3500) {
    Serial.print(F("ALTO"));
  } else {
    Serial.print(F("MUY ALTO"));
  }
  Serial.println(F(")"));

  Serial.print(F("AGC (Gain): "));
  Serial.println(agc);
  Serial.println();
}

void printSensorInfo() {
  Serial.println(F("\n=== INFORMACION DEL SENSOR ==="));

  uint8_t zmco = readRegister8(AS5600_ZMCO);
  uint16_t conf = readRegister16(AS5600_CONF_H);

  Serial.print(F("Programaciones permanentes usadas: "));
  Serial.print(zmco);
  Serial.println(F(" / 3"));

  Serial.print(F("Registro de configuracion: 0x"));
  Serial.println(conf, HEX);

  Serial.print(F("Resolucion: 12 bits ("));
  Serial.print(4096);
  Serial.println(F(" posiciones)"));

  Serial.print(F("Precision teorica: "));
  Serial.print(360.0 / 4096.0, 4);
  Serial.print(F(" grados = "));
  Serial.print((360.0 / 4096.0) * 60.0, 2);
  Serial.print(F(" minutos = "));
  Serial.print((360.0 / 4096.0) * 3600.0, 1);
  Serial.println(F(" segundos de arco"));

  Serial.println();
}

// ============================================
// Funciones de visualización
// ============================================

void printAngleDMS(AngleMeasurement angle) {
  // Formato: 123°45'30.5"
  Serial.print(angle.deg);
  Serial.print(F("°"));

  if (angle.minutes < 10) Serial.print(F("0"));
  Serial.print(angle.minutes);
  Serial.print(F("'"));

  if (angle.seconds < 10) Serial.print(F("0"));
  Serial.print(angle.seconds, 2);
  Serial.print(F("\""));
}

void printAngleFull(AngleMeasurement angle) {
  // Raw value
  Serial.print(F("Raw: "));
  Serial.print(angle.rawValue);
  Serial.print(F("\t"));

  // Decimal degrees
  Serial.print(F("Dec: "));
  Serial.print(angle.degrees, 4);
  Serial.print(F("°\t"));

  // DMS format
  Serial.print(F("DMS: "));
  printAngleDMS(angle);
  Serial.print(F("\t"));

  // Timestamp
  Serial.print(F("T: "));
  Serial.print(angle.timestamp);
  Serial.println(F("ms"));
}

void printHeader() {
  Serial.println(F("\n============================================="));
  Serial.println(F("   VERIFICACION DE SENSOR AS5600"));
  Serial.println(F("   Sistema de Calibracion Automatica"));
  Serial.println(F("=============================================\n"));

  Serial.println(F("Este sketch verifica:"));
  Serial.println(F("  - Comunicacion I2C con AS5600"));
  Serial.println(F("  - Deteccion del iman"));
  Serial.println(F("  - Intensidad del campo magnetico"));
  Serial.println(F("  - Lectura de angulos en tiempo real"));
  Serial.println(F("  - Formato DMS (Grados/Minutos/Segundos)"));
  Serial.println();
}

// ============================================
// Setup y Loop
// ============================================

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);  // Esperar serial (máx 3s)

  printHeader();

  delay(1000);

  // Inicializar sensor
  Serial.println(F("Inicializando AS5600..."));
  if (!initAS5600()) {
    Serial.println(F("\nFALLO: No se pudo inicializar el sensor"));
    Serial.println(F("\nVerifica:"));
    Serial.println(F("  1. Conexiones I2C (SDA, SCL)"));
    Serial.println(F("  2. Alimentacion del sensor (VCC, GND)"));
    Serial.println(F("  3. Direccion I2C correcta (0x36)"));
    while (1) {
      delay(1000);
    }
  }

  delay(500);

  // Mostrar información del sensor
  printSensorInfo();

  // Verificar estado del imán
  printMagnetStatus();

  uint8_t status = readStatus();
  if (!(status & AS5600_STATUS_MD)) {
    Serial.println(F("ADVERTENCIA: Iman no detectado"));
    Serial.println(F("Posiciona el iman a 0.5-3mm del sensor"));
    Serial.println(F("Continuando de todas formas...\n"));
  }

  Serial.println(F("=== LECTURAS EN TIEMPO REAL ==="));
  Serial.println(F("Gira el eje manualmente para ver las lecturas"));
  Serial.println(F("Enviando datos cada 100ms...\n"));

  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();

  // Actualizar cada PRINT_INTERVAL ms
  if (currentTime - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = currentTime;

    // Leer ángulo actual
    currentAngle = readPreciseAngle();

    // Mostrar información completa
    printAngleFull(currentAngle);

    // Cada 5 segundos, mostrar estado del imán
    static unsigned long lastStatusTime = 0;
    if (currentTime - lastStatusTime >= 5000) {
      lastStatusTime = currentTime;
      printMagnetStatus();
      Serial.println(F("=== LECTURAS EN TIEMPO REAL ===\n"));
    }
  }
}

/*
 * GUÍA DE USO:
 *
 * 1. INSTALACIÓN FÍSICA:
 *    - Monta el imán en el eje del motor
 *    - Posiciona el AS5600 a 0.5-3mm del imán
 *    - Asegúrate de que estén centrados
 *
 * 2. CONEXIONES:
 *    - SDA -> A4 (UNO) o GPIO21 (ESP32)
 *    - SCL -> A5 (UNO) o GPIO22 (ESP32)
 *    - VCC -> 5V (UNO) o 3.3V (ESP32)
 *    - GND -> GND
 *
 * 3. VERIFICACIÓN:
 *    a) Cargar este sketch
 *    b) Abrir Serial Monitor (115200 baud)
 *    c) Verificar que el imán sea detectado
 *    d) Girar el eje manualmente
 *    e) Observar cambios en el ángulo
 *
 * 4. CALIBRACIÓN DE POSICIÓN:
 *    - Si la magnitud es < 1500: acercar imán
 *    - Si la magnitud es > 3000: alejar imán
 *    - Rango óptimo: 1500-3000
 *
 * 5. RESOLUCIÓN:
 *    - 12 bits = 4096 posiciones
 *    - Precisión: 0.0879° = 5.27' = 316.4"
 *    - Ideal para motor calibrado (4097 pasos)
 *
 * 6. SIGUIENTES PASOS:
 *    Una vez verificado el sensor:
 *    - Ejecutar sketch 07_auto_calibration_system
 *    - Realizar calibración automática completa
 *    - Comparar con calibración manual (4097 pasos)
 */
