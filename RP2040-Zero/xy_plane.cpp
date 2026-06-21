// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : xy_axis.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Definición de funciones de movimientos
//      - Sincronización de movimientos
//      - Corrección de error (Feedback)
// =======================================================================

#include <Arduino.h>

#include "config.h"
#include "motors.h"
#include "sensors.h"
#include "utils.h"
#include "xy_plane.h"
#include "z_axis.h"

// VARIABLES LOCALES
// -----------------------------------------------------------------------
static float targetShoulderAngle = 0;
static float targetElbowAngle = 0;
static unsigned long settleStart = 0;

// DEFINICION DE MAQUINA DE ESTADOS PARA MOVIMIENTO XY-PLANE
// -----------------------------------------------------------------------
MovingStateXY movingStateXY = MovingStateXY::IDLE;
MoveSequenceState moveSeqState = MoveSequenceState::IDLE;
CaptureSequenceState captureSeqState = CaptureSequenceState::IDLE;

float startT1 = 0;
float startT2 = 0;
float endT1 = 0;
float endT2 = 0;
float captureT1 = 0;
float captureT2 = 0;
static float finalT1 = 0;
static float finalT2 = 0;

extern float currentShoulderAngle;
extern float currentElbowAngle;

// STATUS
// -----------------------------------------------------------------------
bool xyIsMoving() {
    return movingStateXY != MovingStateXY::IDLE;
}

// ASIGNACION DE DESTINO EN GRADOS (NO MUEVE TODAVIA)
// -----------------------------------------------------------------------
void moveToAngles(float shoulder, float elbow) {
    if (movingStateXY != MovingStateXY::IDLE)
        return;

    motorsEnableXY();

    // 🔥 1. CORRECCIÓN DE CONTINUIDAD (AQUÍ ES DONDE VA)
    float correctedShoulder = shortestAngle(shoulder, currentShoulderAngle);
    float correctedElbow = shortestAngle(elbow, currentElbowAngle);

    // 🔥 2. guardar targets corregidos
    targetShoulderAngle = correctedShoulder;
    targetElbowAngle = correctedElbow;

    // 🔥 3. convertir a pasos ya con valores suaves
    long sSteps = angleToStep(correctedShoulder, MotorID::J1);
    long eSteps = angleToStep(correctedElbow, MotorID::J2);

    motor1.moveTo(sSteps);
    motor2.moveTo(eSteps);

    movingStateXY = MovingStateXY::MOVING_TO_TARGET;
}

// ASIGNACION DEL DESTINO DEL ERROR EN GRADOS
// (UNA SOLA EJECUCIÓN POR CICLO, NO MUEVE TODAVIA)
// -----------------------------------------------------------------------
void correctErrorOnce() {
    motorsEnableXY();

    float errorShoulder =
        calculateError(targetShoulderAngle,
                       Wire,
                       motor1Config,
                       sensor1Offset);

    float errorElbow =
        calculateError(targetElbowAngle,
                       Wire1,
                       motor2Config,
                       sensor2Offset);

    bool needsCorrection = false;

    if (fabs(errorShoulder) > 0.5f) {
        float correctedShoulder =
            targetShoulderAngle +
            (motor1Config.motorDirection *
             errorShoulder / motor1Config.reduction);

        motor1.moveTo(
            angleToStep(correctedShoulder, MotorID::J1));

        needsCorrection = true;
    }

    if (fabs(errorElbow) > 0.5f) {
        float correctedElbow =
            targetElbowAngle +
            (motor2Config.motorDirection *
             errorElbow / motor2Config.reduction);

        motor2.moveTo(
            angleToStep(correctedElbow, MotorID::J2));

        needsCorrection = true;
    }

    if (needsCorrection) {
        movingStateXY = MovingStateXY::CORRECTING;
    } else {
        movingStateXY = MovingStateXY::IDLE;
    }
}

// MAQUINA DE ESTADOS PARA MOVIMIENTO XY-PLANE (MOVIMIENTO DE MOTORES)
// -----------------------------------------------------------------------
void updateXY() {
    switch (movingStateXY) {
        case MovingStateXY::IDLE:
            break;

        case MovingStateXY::MOVING_TO_TARGET:
            motor1.run();
            motor2.run();
            if (motor1.distanceToGo() == 0 &&
                motor2.distanceToGo() == 0) {
                settleStart = millis();
                delay(100); // estabilizacion mecanica
                COMM.print("Moved to Target");
                // 🔥 AQUÍ VA LO IMPORTANTE
                currentShoulderAngle = targetShoulderAngle;
                currentElbowAngle = targetElbowAngle;
                // printDebugMove(targetShoulderAngle, targetElbowAngle);
                movingStateXY = MovingStateXY::SETTLING;
            }
            break;

        case MovingStateXY::SETTLING:
            if (millis() - settleStart > 100) {
                movingStateXY = MovingStateXY::CORRECTING;
                // COMM.println("Correcting Error");
                correctErrorOnce();
            }
            break;

        case MovingStateXY::CORRECTING:
            motor1.run();
            motor2.run();
            if (motor1.distanceToGo() == 0 &&
                motor2.distanceToGo() == 0) {
                settleStart = millis();
                COMM.println("Correction Done");
                COMM.println();
                movingStateXY = MovingStateXY::IDLE;
            }
            break;
    }
}

// INICIAR SECUENCIA DE MOVIMIENTO COMPLETA (START → XY → Z → XY → Z)
// -----------------------------------------------------------------------
// startT1, startT2: ángulos iniciales para el movimiento XY
// endT1, endT2: ángulos finales para el movimiento XY
void startMoveSequence(float s1, float s2, float e1, float e2) {
    startT1 = s1;
    startT2 = s2;
    endT1 = e1;
    endT2 = e2;

    moveSeqState = MoveSequenceState::MOVING_START;

    COMM.print("START T1=");
    COMM.println(s1);

    COMM.print("START T2=");
    COMM.println(s2);

    COMM.print("END T1=");
    COMM.println(e1);

    COMM.print("END T2=");
    COMM.println(e2);

    moveToAngles(startT1, startT2);
}

// INICIAR SECUENCIA DE CAPTURA COMPLETA (XY → Z → HOME)
// -----------------------------------------------------------------------
void startCaptureSequence(float captureT1_, float captureT2_, float finalT1_, float finalT2_) {
    captureT1 = captureT1_;
    captureT2 = captureT2_;
    finalT1 = finalT1_;
    finalT2 = finalT2_;

    captureSeqState = CaptureSequenceState::MOVING_TO_CAPTURE;

    moveToAngles(captureT1, captureT2);
}

// MAQUINA DE ESTADOS PARA SECUENCIA DE MOVIMIENTO COMPLETA (START → XY → Z → XY → Z)
// -----------------------------------------------------------------------
void updateMoveSequence() {
    switch (moveSeqState) {
        // 1. Mover pieza (START → XY)
        case MoveSequenceState::MOVING_START:

            if (!xyIsMoving()) {
                startZPick(); // 🔥 baja Z y agarra pieza
                moveSeqState = MoveSequenceState::PICKING;
            }
            break;

        // 2. Esperar Pick Terminado (Z)
        case MoveSequenceState::PICKING:

            if (movingStateZ == MovingStateZ::IDLE) {
                moveToAngles(endT1, endT2); // 🔥 ir a destino
                moveSeqState = MoveSequenceState::MOVING_END;
            }
            break;

        // 3. Movimiento final (END → XY)
        case MoveSequenceState::MOVING_END:

            if (!xyIsMoving()) {
                startZPlace(); // 🔥 soltar pieza
                moveSeqState = MoveSequenceState::PLACING;
            }
            break;

        // 4. Esperar final de Z PLACE
        case MoveSequenceState::PLACING:

            if (movingStateZ == MovingStateZ::IDLE) {
                COMM.println("MOVE DONE");
                moveSeqState = MoveSequenceState::GO_HOME;
            }
            break;

        // 5. IR A HOME (nuevo estado limpio)
        case MoveSequenceState::GO_HOME:

            if (!xyIsMoving()) {
                moveToHomeXY(); // 👉 ahora sí HOME real controlado
                COMM.println("GO HOME START");
                moveSeqState = MoveSequenceState::IDLE;
            }
            break;

        // IDLE
        case MoveSequenceState::IDLE:
        default:
            break;
    }
}

void updateCaptureSequence() {
    switch (captureSeqState) {
        // 1. Ir a pieza enemiga
        case CaptureSequenceState::MOVING_TO_CAPTURE:

            if (movingStateXY == MovingStateXY::IDLE) {
                startZPick();
                captureSeqState = CaptureSequenceState::PICKING;
            }
            break;

        // 2. Agarrar pieza
        case CaptureSequenceState::PICKING:

            if (movingStateZ == MovingStateZ::IDLE) {
                moveToHomeXY();
                captureSeqState = CaptureSequenceState::MOVING_HOME;
            }
            break;

        // 3. Ir a HOME con pieza capturada
        case CaptureSequenceState::MOVING_HOME:

            if (!xyIsMoving()) {
                startZPlace();
                captureSeqState = CaptureSequenceState::PLACING;
            }
            break;

        // 4. Soltar pieza capturada
        case CaptureSequenceState::PLACING:

            if (movingStateZ == MovingStateZ::IDLE) {
                COMM.println("CAPTURE DONE");

                // 🔥 AQUÍ ENTRA EL NUEVO FLUJO
                moveToHomeXY();
                captureSeqState = CaptureSequenceState::GO_TO_FINAL_MOVE;
            }
            break;

        // 5. MOVIMIENTO FINAL HACIA LA POSICIÓN DE DESTINO
        case CaptureSequenceState::GO_TO_FINAL_MOVE:

            if (!xyIsMoving()) {
                moveToAngles(finalT1, finalT2);

                COMM.println("FINAL MOVE START");

                captureSeqState = CaptureSequenceState::IDLE;
            }
            break;

        case CaptureSequenceState::IDLE:
        default:
            break;
    }
}

// MOVIMIENTO A HOME DEL PLANO XY
// (NO ES HOMING SOLO REGRESAR A POSICION CERO DESPUES DEL MOVIMIENTO)
// -----------------------------------------------------------------------
void moveToHomeXY() {
    moveToAngles(0.0f, 0.0f);
}

// DEBUG
// -----------------------------------------------------------------------
void printDebugMove(float motor1Angle, float motor2Angle) {
    COMM.println();
    COMM.println("-------- MOTOR 1 --------");

    float sensor1 = estimateSensorAngle(
        targetShoulderAngle,
        motor1Config.reduction,
        sensor1Offset,
        motor1Config.motorDirection);

    COMM.print("HomingOffset: ");
    COMM.println(sensor1Offset, 1);

    COMM.print("Estimated Sensor Angle: ");
    COMM.println(sensor1, 1);

    COMM.print("Real Sensor Angle: ");
    COMM.println(rawToDegrees(sensorReadRawAngle(Wire)), 1);

    COMM.println("-------- MOTOR 2 --------");

    float sensor2 = estimateSensorAngle(
        targetElbowAngle,
        motor2Config.reduction,
        sensor2Offset,
        motor2Config.motorDirection);

    COMM.print("HomingOffset: ");
    COMM.println(sensor2Offset, 1);

    COMM.print("Estimated Sensor Angle: ");
    COMM.println(round1Decimal(sensor2), 1);

    COMM.print("Real Sensor Angle: ");
    COMM.println(rawToDegrees(sensorReadRawAngle(Wire1)), 1);

    COMM.println();

    float errorShoulder =
        calculateError(targetShoulderAngle, Wire, motor1Config, sensor1Offset);

    float errorElbow =
        calculateError(targetElbowAngle, Wire1, motor2Config, sensor2Offset);

    COMM.print("Error1: ");
    COMM.println(errorShoulder, 1);

    COMM.print("Error2: ");
    COMM.println(errorElbow, 1);

    COMM.println();
}

void resetXYState() {
    // =========================
    // RESET DE MÁQUINA XY
    // =========================

    movingStateXY = MovingStateXY::IDLE;
    moveSeqState = MoveSequenceState::IDLE;
    captureSeqState = CaptureSequenceState::IDLE;

    // =========================
    // RESET DE TARGETS
    // =========================

    targetShoulderAngle = 0;
    targetElbowAngle = 0;

    startT1 = 0;
    startT2 = 0;
    endT1 = 0;
    endT2 = 0;
    captureT1 = 0;
    captureT2 = 0;

    // =========================
    // RESET DE TIEMPOS
    // =========================

    settleStart = 0;

    // =========================
    // OPCIONAL SEGURIDAD
    // =========================

    motor1.stop();
    motor2.stop();

    COMM.println("XY STATE RESET");
}

void cancelMoveSequence() {
    // parar máquina de estados
    moveSeqState = MoveSequenceState::IDLE;
    captureSeqState = CaptureSequenceState::IDLE;

    // parar XY inmediatamente
    motor1.stop();
    motor2.stop();

    // opcional: reset targets
    startT1 = 0;
    startT2 = 0;
    endT1 = 0;
    endT2 = 0;
    captureT1 = 0;
    captureT2 = 0;

    // opcional: reset estado XY también
    movingStateXY = MovingStateXY::IDLE;

    COMM.println("MOVE SEQUENCE CANCELED");
}