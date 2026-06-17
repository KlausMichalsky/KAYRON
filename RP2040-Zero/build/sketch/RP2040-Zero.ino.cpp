#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\RP2040-Zero.ino"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : RP2040-Zero.ino
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Setup de inicialización
//      - Loop principal
// =======================================================================

#include <Arduino.h>
#include <Wire.h>

#include <AccelStepper.h>

#include "command.h"
#include "communication.h"
#include "config.h"
#include "core.h"
#include "homing.h"
#include "motors.h"
#include "sensors.h"
#include "utils.h"

// SETUP
// -----------------------------------------------------------------------
#line 29 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\RP2040-Zero.ino"
void setup();
#line 46 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\RP2040-Zero.ino"
void loop();
#line 29 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\RP2040-Zero.ino"
void setup() {
    delay(1000);
    communicationInit();
    coreInit();
    homingInitXY(motor1Homing);
    homingInitXY(motor2Homing);
    homingInitZ(motor3Homing);
    motorsInit();
    sensorsInit();

    pinMode(MAGNET, OUTPUT);
    digitalWrite(MAGNET, LOW);
    pinMode(LED, OUTPUT);
}

// LOOP
// -----------------------------------------------------------------------
void loop() {
    if (commandAvailable()) {
        String cmd = readCommand();
        if (cmd.length() > 0) // 🔥 SOLO procesar si hay comando completo.
        {
            processCommand(cmd);
        }
    }
    coreUpdate();
}

