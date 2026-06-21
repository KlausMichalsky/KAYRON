// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : xy_axis.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Declaracion de funciones de movimientos
//      - Sincronización de movimientos
//      - Corrección de error (Feedback)
// =======================================================================

#pragma once

#include <Arduino.h>

extern float sensor1Offset;
extern float sensor2Offset;

void moveToAngles(float targetShoulder, float targetElbow);
void moveToHomeXY();
void updateXY();
void updateMoveSequence();
bool xyIsMoving();
void printDebugMove(float motor1Angle, float motor2Angle);
void correctErrorOnce();
void startMoveSequence(float s1, float s2, float e1, float e2);
void startCaptureSequence(float theta1, float theta2);
void updateCaptureSequence();
void resetXYState();
void cancelMoveSequence();