// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : utils.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Funciones de cálculo y conversión de valores
//      - Ángulos a pasos de motores
//      - Casilla a coordenadas
//      - Casilla a ángulos
//      - Cinemática inversa
// =======================================================================

#pragma once

#include <Arduino.h>
#include <Wire.h>

long angleToStep(float angle, MotorID id);

float rawToDegrees(uint16_t rawAngle);

float round1Decimal(float value);

bool chessSquareToXY(
    const String &square,
    float &x,
    float &y);

bool inverseKinematics(
    float x,
    float y,
    float L1,
    float L2,
    float &theta1,
    float &theta2);

bool chessSquareToAngles(
    const String &square,
    float &theta1Deg,
    float &theta2Deg);

void printBoardXY(); // Solo para Debug

float shortestAngle(float target, float current);