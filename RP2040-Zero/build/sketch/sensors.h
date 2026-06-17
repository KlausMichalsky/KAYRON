#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\sensors.h"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : sensores.h
//  Autor      : Klaus Michalsky
//  Fecha      : 2025-12-04
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      Declaración de funciones y constantes para el manejo
//      de sensores del robot de ajedrez.
//  ▫️ RESPONSABILIDADES:
//      - Proveer prototipos de funciones de inicialización y lectura.
//      - Definir constantes y macros para sensores.
//      - Servir de interfaz clara para el resto del proyecto.
// =======================================================================

#pragma once

#include <Arduino.h>
#include <Wire.h>

extern float motor1Angle;
extern float motor2Angle;

void sensorsInit();
uint16_t sensorReadRawAngle(TwoWire &wire);
float sensorHomingOffset(TwoWire &wire);
float sensorCorrectedAngle(TwoWire &wire, float offset);
float estimateSensorAngle(
    float targetAngle,
    float reduction,
    float homingOffset,
    int8_t motorDirection);
float calculateError(
    float targetAngle,
    TwoWire &wire,
    const MotorConfig &config,
    float sensorOffset);