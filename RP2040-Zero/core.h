// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : core.h
//  Autor      : Klaus Michalsky
//  Fecha      : Mar-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Declaración de funciones centrales del robot.
// =======================================================================

#pragma once

#include <Arduino.h>

#include "config.h"
#include "homing.h"

extern bool dynamicAngle1;
extern bool dynamicAngle2;
extern float sensor1Offset;
extern float sensor2Offset;

void coreInit();
void coreHomeAll();
void coreHomeSingleMotor();
void coreUpdate();
