#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\sensors.cpp"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : sensors.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Inicializar y configurar sensores.
//      - Implementación de funciones para la lectura y gestión de
//        sensores AS5600.
// =======================================================================

#include <Arduino.h>

#include "config.h"
#include "sensors.h"
#include "utils.h"

// VARIABLES GLOBALES DEL SISTEMA
// -----------------------------------------------------------------------
float sensor1Offset = 0;
float sensor2Offset = 0;

// INICIALIZACION DE SENSORES
// -----------------------------------------------------------------------
void sensorsInit() {
    pinMode(HALL_1, INPUT_PULLUP);
    pinMode(HALL_2, INPUT_PULLUP);
    pinMode(HALL_3, INPUT_PULLUP);

    Wire.setSDA(AS5600_1_SDA);
    Wire.setSCL(AS5600_1_SCL);
    Wire.begin();

    Wire1.setSDA(AS5600_2_SDA);
    Wire1.setSCL(AS5600_2_SCL);
    Wire1.begin();
}
// LECTURA DE ANGULO CRUDO
// -----------------------------------------------------------------------
uint16_t sensorReadRawAngle(TwoWire &wire) {
    wire.beginTransmission(AS5600_ADDR);
    wire.write(0x0E);
    wire.endTransmission(false);
    wire.requestFrom(AS5600_ADDR, (uint8_t)2);

    if (wire.available() < 2)
        return 0;

    uint8_t high = wire.read();
    uint8_t low = wire.read();

    return ((high & 0x0F) << 8) | low;
}

// OFFSET DE HOMING
// -----------------------------------------------------------------------
float sensorHomingOffset(TwoWire &wire) {
    const uint8_t samples = 30;
    float sum = 0;

    float firstAngle = rawToDegrees(sensorReadRawAngle(wire));

    for (uint8_t i = 0; i < samples; i++) {
        float angle = rawToDegrees(sensorReadRawAngle(wire));

        // corrección de salto 0/360
        if (fabs(angle - firstAngle) > 180) {
            if (angle < firstAngle)
                angle += 360;
            else
                angle -= 360;
        }

        sum += angle;
    }

    float offset = sum / samples;

    if (offset >= 360)
        offset -= 360;
    if (offset < 0)
        offset += 360;

    return offset;
}

// CORRECTURA DEL OFFSET DE HOMING EN EL SENSOR (ANGULO 0–360 ALINEACIÓN FISICA)
// -----------------------------------------------------------------------
// Si el sensor tiene un offset "phi" con respecto al cero mecánico
// -> entonces toma ese valor como cero absoluto
float sensorCorrectedAngle(TwoWire &wire, float offset) {
    float angle = rawToDegrees(sensorReadRawAngle(wire)) - offset;

    if (angle >= 360)
        angle -= 360;
    if (angle < 0)
        angle += 360;

    return angle;
}

// CÁLCULO DE ANGULO TEÓRICO (INCLUYENDO ALINEACIÓN FISICA DEL SENSOR)
// -----------------------------------------------------------------------
float estimateSensorAngle(
    float targetAngle,
    float reduction,
    float homingOffset,
    int8_t motorDirection) {
    float motorAngle = targetAngle * reduction;

    // Normalizar vueltas, el operador módulo % o fmod() devuelve el “sobrante”
    // ejemplo rest = fmod(2700, 360) = 180
    float rest = fmod(motorAngle, 360.0f);

    // Convierte negativos a rango positivo
    if (rest < 0)
        rest += 360.0f;

    float estimatedSensorAngle = motorDirection * rest + homingOffset;

    // Wrap final verifica si pasó de 360° o quedo negativo
    estimatedSensorAngle = fmod(estimatedSensorAngle, 360.0f);
    if (estimatedSensorAngle < 0)
        estimatedSensorAngle += 360.0f;

    return estimatedSensorAngle;
}

// CALCULO DEL ERROR = ANGULO TEORICO - ANGULO REAL
// -----------------------------------------------------------------------
float calculateError(
    float targetAngle,
    TwoWire &wire,
    const MotorConfig &config,
    float sensorOffset) {
    float estimatedSensorAngle = estimateSensorAngle(
        targetAngle,
        config.reduction,
        sensorOffset,
        config.motorDirection);
    estimatedSensorAngle = round1Decimal(estimatedSensorAngle);

    float realSensorAngle = rawToDegrees(sensorReadRawAngle(wire));
    realSensorAngle = round1Decimal(realSensorAngle);

    float error = estimatedSensorAngle - realSensorAngle;

    if (error > 180.0f)
        error -= 360.0f;

    if (error < -180.0f)
        error += 360.0f;

    return error;
}
