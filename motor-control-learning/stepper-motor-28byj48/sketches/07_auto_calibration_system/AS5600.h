/*
 * AS5600.h - Librería para Encoder Magnético AS5600
 *
 * Esta librería proporciona funciones para:
 * - Comunicación I2C con AS5600
 * - Lectura de ángulos con precisión de 12 bits (4096 posiciones)
 * - Conversión a formato Grados/Minutos/Segundos
 * - Diagnóstico del campo magnético
 * - Detección de estado del imán
 *
 * Hardware: AS5600 Encoder Magnético I2C
 * Dirección I2C: 0x36
 * Resolución: 12 bits (0-4095)
 * Precisión: 0.0879° (5.27' o 316.4")
 *
 * Autor: Sistema de Calibración Motor 28BYJ-48
 * Versión: 1.0
 * Fecha: 2025-12-30
 */

#ifndef AS5600_H
#define AS5600_H

#include <Arduino.h>
#include <Wire.h>

// ============================================
// Constantes del AS5600
// ============================================

#define AS5600_ADDRESS 0x36  // Dirección I2C

// Registros de configuración
#define AS5600_ZMCO        0x00
#define AS5600_ZPOS_H      0x01
#define AS5600_ZPOS_L      0x02
#define AS5600_MPOS_H      0x03
#define AS5600_MPOS_L      0x04
#define AS5600_MANG_H      0x05
#define AS5600_MANG_L      0x06
#define AS5600_CONF_H      0x07
#define AS5600_CONF_L      0x08

// Registros de salida
#define AS5600_RAW_ANGLE_H 0x0C
#define AS5600_RAW_ANGLE_L 0x0D
#define AS5600_ANGLE_H     0x0E
#define AS5600_ANGLE_L     0x0F

// Registros de estado
#define AS5600_STATUS      0x0B
#define AS5600_AGC         0x1A
#define AS5600_MAGNITUDE_H 0x1B
#define AS5600_MAGNITUDE_L 0x1C

// Bits de estado
#define AS5600_STATUS_MH   0x08
#define AS5600_STATUS_ML   0x10
#define AS5600_STATUS_MD   0x20

// ============================================
// Estructuras de datos
// ============================================

struct AngleMeasurement {
  uint16_t rawValue;        // Valor raw (0-4095)
  float degrees;            // Grados decimales (0-360)
  int deg;                  // Grados enteros (0-359)
  int minutes;              // Minutos de arco (0-59)
  float seconds;            // Segundos de arco (0-59.99)
  unsigned long timestamp;  // Timestamp en ms
};

struct MagnetStatus {
  bool detected;            // Imán detectado
  bool tooStrong;           // Campo muy fuerte
  bool tooWeak;             // Campo muy débil
  uint16_t magnitude;       // Magnitud del campo (0-4095)
  uint8_t agc;              // Ganancia automática (0-255)
  bool optimal;             // Estado óptimo (1500-3000)
};

// ============================================
// Clase AS5600
// ============================================

class AS5600 {
  private:
    uint8_t _address;
    int _revolutionCount;     // Contador de revoluciones completas
    float _lastAngle;         // Último ángulo leído (para detectar vueltas)
    float _totalAngle;        // Ángulo total acumulado (múltiples vueltas)

    // Funciones de bajo nivel I2C
    uint8_t readRegister8(uint8_t reg);
    uint16_t readRegister16(uint8_t regHigh);

  public:
    // Constructor
    AS5600(uint8_t address = AS5600_ADDRESS);

    // Inicialización
    bool begin();
    bool isConnected();

    // Lectura de ángulos
    uint16_t readRawAngle();
    uint16_t readAngle();
    float readAngleDegrees();
    AngleMeasurement readPreciseAngle();

    // Seguimiento de múltiples revoluciones
    void resetRevolutionCount();
    int getRevolutionCount();
    float getTotalAngle();
    void updateMultiRevolution();

    // Estado del imán
    MagnetStatus getMagnetStatus();
    bool isMagnetDetected();
    uint16_t getMagnitude();
    uint8_t getAGC();

    // Configuración
    uint8_t getZMCO();  // Contador de programaciones
    uint16_t getConfig();

    // Utilidades de conversión
    static float rawToDegrees(uint16_t rawValue);
    static void decimalToDMS(float decimal, int &deg, int &min, float &sec);
    static float DMStoDecimal(int deg, int min, float sec);
    static float errorInArcSeconds(float errorDegrees);
};

// ============================================
// Implementación
// ============================================

AS5600::AS5600(uint8_t address) {
  _address = address;
  _revolutionCount = 0;
  _lastAngle = 0;
  _totalAngle = 0;
}

bool AS5600::begin() {
  Wire.begin();
  Wire.setClock(400000);  // 400kHz Fast Mode
  delay(10);
  return isConnected();
}

bool AS5600::isConnected() {
  Wire.beginTransmission(_address);
  return (Wire.endTransmission() == 0);
}

uint8_t AS5600::readRegister8(uint8_t reg) {
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(_address, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

uint16_t AS5600::readRegister16(uint8_t regHigh) {
  uint8_t highByte = readRegister8(regHigh);
  uint8_t lowByte = readRegister8(regHigh + 1);
  return ((uint16_t)highByte << 8) | lowByte;
}

uint16_t AS5600::readRawAngle() {
  return readRegister16(AS5600_RAW_ANGLE_H) & 0x0FFF;
}

uint16_t AS5600::readAngle() {
  return readRegister16(AS5600_ANGLE_H) & 0x0FFF;
}

float AS5600::readAngleDegrees() {
  return rawToDegrees(readRawAngle());
}

AngleMeasurement AS5600::readPreciseAngle() {
  AngleMeasurement measurement;

  measurement.rawValue = readRawAngle();
  measurement.degrees = rawToDegrees(measurement.rawValue);
  measurement.timestamp = millis();

  decimalToDMS(measurement.degrees, measurement.deg, measurement.minutes, measurement.seconds);

  return measurement;
}

void AS5600::resetRevolutionCount() {
  _revolutionCount = 0;
  _lastAngle = readAngleDegrees();
  _totalAngle = _lastAngle;
}

int AS5600::getRevolutionCount() {
  return _revolutionCount;
}

float AS5600::getTotalAngle() {
  return _totalAngle;
}

void AS5600::updateMultiRevolution() {
  float currentAngle = readAngleDegrees();

  // Detectar cruce de 0° (vuelta completa)
  if (_lastAngle > 270 && currentAngle < 90) {
    _revolutionCount++;  // Sentido horario
  } else if (_lastAngle < 90 && currentAngle > 270) {
    _revolutionCount--;  // Sentido antihorario
  }

  _totalAngle = (_revolutionCount * 360.0) + currentAngle;
  _lastAngle = currentAngle;
}

MagnetStatus AS5600::getMagnetStatus() {
  MagnetStatus status;

  uint8_t statusReg = readRegister8(AS5600_STATUS);

  status.detected = statusReg & AS5600_STATUS_MD;
  status.tooStrong = statusReg & AS5600_STATUS_MH;
  status.tooWeak = statusReg & AS5600_STATUS_ML;
  status.magnitude = getMagnitude();
  status.agc = getAGC();

  // Rango óptimo: 1500-3000
  status.optimal = (status.detected &&
                    !status.tooStrong &&
                    !status.tooWeak &&
                    status.magnitude >= 1500 &&
                    status.magnitude <= 3000);

  return status;
}

bool AS5600::isMagnetDetected() {
  uint8_t status = readRegister8(AS5600_STATUS);
  return status & AS5600_STATUS_MD;
}

uint16_t AS5600::getMagnitude() {
  return readRegister16(AS5600_MAGNITUDE_H) & 0x0FFF;
}

uint8_t AS5600::getAGC() {
  return readRegister8(AS5600_AGC);
}

uint8_t AS5600::getZMCO() {
  return readRegister8(AS5600_ZMCO);
}

uint16_t AS5600::getConfig() {
  return readRegister16(AS5600_CONF_H);
}

// ============================================
// Funciones estáticas de conversión
// ============================================

float AS5600::rawToDegrees(uint16_t rawValue) {
  return (rawValue * 360.0) / 4096.0;
}

void AS5600::decimalToDMS(float decimal, int &deg, int &min, float &sec) {
  deg = (int)decimal;
  float remainder = (decimal - deg) * 60.0;
  min = (int)remainder;
  sec = (remainder - min) * 60.0;
}

float AS5600::DMStoDecimal(int deg, int min, float sec) {
  return deg + (min / 60.0) + (sec / 3600.0);
}

float AS5600::errorInArcSeconds(float errorDegrees) {
  return errorDegrees * 3600.0;
}

#endif // AS5600_H
