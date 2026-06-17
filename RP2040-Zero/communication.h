// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : communication.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Declaración de la interfaz de comunicación UART
// =======================================================================

#include <Arduino.h>

#pragma once

#define DEBUG_UART 0 // 1 = debug activado, 0 = debug desactivado

void debug(const String &msg); // función para debug condicional
void communicationInit();
