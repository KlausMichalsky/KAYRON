#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\Z_Axis.h"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : z_axis.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Declaración de funciones para movimiento del eje Z
//      - Sincronización de movimiento del eje Z
//      - Controll del electroimán
// =======================================================================

#include <Arduino.h>

#include "config.h"

#pragma once

// estado global del Z
extern MovingStateZ movingStateZ;

void zMoveHome();
void zMoveDown();
void zMoveTravel();
void magnetON();
void magnetOFF();
void startZPick();
void startZPlace();
void updateZ();