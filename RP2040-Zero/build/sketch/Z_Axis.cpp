#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\Z_Axis.cpp"
// =======================================================================
//                 🔹 C H E S S B O T  —   Z E R O 🔹
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
void zMoveHome() {
    motor3.setMaxSpeed(6000);
    motor3.setAcceleration(15000);
    motor3.moveTo(0);
}

void zMoveDown() {
    motor3.setMaxSpeed(6000);
    motor3.setAcceleration(15000);
    motor3.moveTo(Z_STEPS_DOWN - Z_HOME_OFFSET); // mover hacia abajo hasta la posición de agarre
}

void zMoveTravel() { // Subir a altura segura para mover alzar la pieza (Z arriba)
    motor3.setMaxSpeed(6000);
    motor3.setAcceleration(15000);
    motor3.moveTo(Z_TRAVEL_POS);
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
                Serial1.println("Z: GRIP");
                magnetON();
                movingStateZ = MovingStateZ::PICK_GRIP;
                zMoveTravel();
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
                Serial1.println("Z PICK DONE");
                movingStateZ = MovingStateZ::IDLE;
            }
            break;

        // Mover hacia abajo y desactivar electroimán
        case MovingStateZ::PLACE_DOWN:

            motor3.run();

            if (motor3.distanceToGo() == 0) {
                Serial1.println("Z: RELEASE");
                magnetOFF();
                movingStateZ = MovingStateZ::PLACE_RELEASE;
                zMoveHome();
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
                Serial1.println("Z PLACE DONE");
                movingStateZ = MovingStateZ::IDLE;
            }
            break;

        // IDLE
        case MovingStateZ::IDLE:
        default:
            break;
    }
}