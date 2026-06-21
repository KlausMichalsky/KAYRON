// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : z_axis.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Definición de funciones para movimiento del eje Z
//      - Sincronización de movimiento del eje Z
//      - Controll del electroimán
// =======================================================================

#include <Arduino.h>

#include "config.h"
#include "homing.h"
#include "motors.h"
#include "z_axis.h"

// ESTADO GLOBAL Z
// -----------------------------------------------------------------------
MovingStateZ movingStateZ = MovingStateZ::IDLE;

// MOVIMIENTOS BASE (NO BLOQUEANTES)
// -----------------------------------------------------------------------
void zMoveDown() {
    motor3.setMaxSpeed(motor3Config.fastSpeed);
    motor3.setAcceleration(motor3Config.acceleration);
    motor3.moveTo(Z_STEPS_DOWN);
}

void zMoveUp() {
    motor3.setMaxSpeed(motor3Config.fastSpeed);
    motor3.setAcceleration(motor3Config.acceleration);
    motor3.moveTo(0);
}

// ELECTROIMÁN
// -----------------------------------------------------------------------
void magnetON() {
    digitalWrite(MAGNET, HIGH);
}

void magnetOFF() {
    digitalWrite(MAGNET, LOW);
}

// INICIO DE SECUENCIAS
// -----------------------------------------------------------------------
void startZPick() {
    motorEnableZ();
    movingStateZ = MovingStateZ::PICK_DOWN;
    zMoveDown();
}

void startZPlace() {
    motorEnableZ();
    movingStateZ = MovingStateZ::PLACE_DOWN;
    zMoveDown();
}

// MAQUINA DE ESTADOS PARA SECUENCIA DE MOVIMIENTO DEL EJE Z (PICK → PLACE)
// -----------------------------------------------------------------------
void updateZ() {
    switch (movingStateZ) {
        // Mover hacia abajo y activar electroimán
        case MovingStateZ::PICK_DOWN:

            motor3.run();

            if (motor3.distanceToGo() == 0) {
                COMM.println("Z: GRIP");
                magnetON();
                movingStateZ = MovingStateZ::PICK_GRIP;
                zMoveUp(); // siguiente acción inmediata
            }
            break;

        // Mover hacia arriba
        case MovingStateZ::PICK_GRIP:

            motor3.run();

            if (motor3.distanceToGo() == 0) {
                movingStateZ = MovingStateZ::PICK_UP;
            }
            break;

        // Pick DONE
        case MovingStateZ::PICK_UP:

            if (motor3.distanceToGo() == 0) {
                COMM.println("Z PICK DONE");
                movingStateZ = MovingStateZ::IDLE;
            }
            break;

        // Mover hacia abajo y desactivar electroimán
        case MovingStateZ::PLACE_DOWN:

            motor3.run();

            if (motor3.distanceToGo() == 0) {
                COMM.println("Z: RELEASE");
                magnetOFF();
                movingStateZ = MovingStateZ::PLACE_RELEASE;
                zMoveUp();
            }
            break;

        // Mover hacia arriba
        case MovingStateZ::PLACE_RELEASE:

            motor3.run();

            if (motor3.distanceToGo() == 0) {
                movingStateZ = MovingStateZ::PLACE_UP;
            }
            break;

        // Place DONE
        case MovingStateZ::PLACE_UP:

            if (motor3.distanceToGo() == 0) {
                COMM.println("Z PLACE DONE");
                movingStateZ = MovingStateZ::IDLE;
            }
            break;

        // IDLE
        case MovingStateZ::IDLE:
        default:
            break;
    }
}