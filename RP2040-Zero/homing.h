// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : homing.h
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Definir la estructura de estado del homing
//      - Proporcionar funciones públicas para inicializar y ejecutar la
//        rutina de homing.
//      - Permitir consultar el estado actual y detectar errores.
//      - Servir como interfaz para otros módulos del robot que requieran
//        funcionalidad de homing.
// =======================================================================

#pragma once

#include <Arduino.h>

#include <AccelStepper.h>

#include "config.h"

// ESTRUCTURA DE ESTADO DEL HOMING
// -----------------------------------------------------------------------
struct HomingXY {
    HomingStateXY state;     // Estado actual de la máquina de estados de homing// <-- aquí usamos el enum
    unsigned long startTime; // Tiempo (millis) en el que comenzó el homing
    long firstEdge;          // Primer flanco detectado por el sensor
    long secondEdge;         // Segundo flanco detectado por el sensor
    long centerPosition;     // Posición calculada a partir de los flancos -> referencia absoluta
    bool fault;              // Flag de error latcheado, permanece activo hasta que el usuario lo resetea
};

struct HomingZ {
    HomingStateZ state;      // Estado actual de la máquina de estados de homing// <-- aquí usamos el enum
    unsigned long startTime; // Tiempo (millis) en el que comenzó el homing
    long initialPosition;    // Posición calculada a partir de los flancos -> referencia absoluta
    long edge;               // Flanco de salida detectado por el sensor
    long reference;          // Posiciónde homing calculada
    bool fault;              // Flag de error latcheado, permanece activo hasta que se resetee
};

extern HomingXY motor1Homing;
extern HomingXY motor2Homing;
extern HomingZ motor3Homing;

extern HomeAllState homeAllState;
extern HomeSingleState homeSingleState;
extern MotorID motorToHome;
extern bool homeAllActive;

void homingInitXY(HomingXY &st);

void homingInitZ(HomingZ &st);

void homingStartXY(AccelStepper &motor,
                   const MotorConfig &cfg,
                   HomingXY &st,
                   int hallPin);

void homingStartZ(AccelStepper &motor,
                  const MotorConfig &cfg,
                  HomingZ &st,
                  int hallPin);

bool homingXYisActive(const HomingXY &st);

bool homingZisActive(const HomingZ &st);

void homingStepXY(AccelStepper &motor,
                  const MotorConfig &cfg,
                  HomingXY &st,
                  int hallPin);

void homingStepZ(AccelStepper &motor,
                 const MotorConfig &cfg,
                 HomingZ &st,
                 int hallPin);

bool homingXYhasError(const HomingXY &st);

bool homingZhasError(const HomingZ &st);

HomingStateXY homingGetStateXY(const HomingXY &st);

HomingStateZ homingGetStateZ(const HomingZ &st);
