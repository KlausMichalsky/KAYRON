// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : command.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Declaración de funciones para la gestión de comandos
//        recibidos por UART
// =======================================================================

#pragma once

#include <Arduino.h>

#include "config.h"

bool commandAvailable();
void processCommand(const String &command);
String readCommand();
void commandSendStatusReport();
