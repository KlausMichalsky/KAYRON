#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\motors.cpp"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : motors.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Crear las instancias de los motores AccelStepper.
//      - Gestionar el estado interno de habilitación.
//      - Configurar velocidad y aceleración.
//      - Ejecutar movimientos coordinados en el plano XY y eje Z.
//      - Proveer parada segura y control de emergencia.
// =======================================================================

#include <Arduino.h>

#include <AccelStepper.h>

#include "config.h"
#include "core.h"
#include "motors.h"
#include "sensors.h"

// INSTANCIAS DE MOTORES
// -----------------------------------------------------------------------
AccelStepper motor1(AccelStepper::DRIVER, MOTOR1_STEP, MOTOR1_DIR);
AccelStepper motor2(AccelStepper::DRIVER, MOTOR2_STEP, MOTOR2_DIR);
AccelStepper motor3(AccelStepper::DRIVER, MOTOR3_STEP, MOTOR3_DIR);

// ESTADO INTERNO DE MOTORES
// -----------------------------------------------------------------------
static bool motorsEnabledXY = false;
static bool motorEnabledZ = false;

// API PÚBLICA DE MOTORES
// -----------------------------------------------------------------------
void motorsInit() {
    // ‼️ Adaptar también en config.h las señales ENABLE_ACTIVE/INACTIVE
    // (DIR, STEP, ENABLE) true = invertir señal. Aqui: LOW=ON HIGH=OFF
    motor1.setPinsInverted(false, false, false);
    motor2.setPinsInverted(true, false, false);
    motor3.setPinsInverted(false, false, false);

    pinMode(motor1Config.enablePin, OUTPUT);
    pinMode(motor2Config.enablePin, OUTPUT);
    pinMode(motor3Config.enablePin, OUTPUT);

    // ➡️ Esto evita movimientos inesperados al encender el sistema
    motorsDisableXY();
    motorDisableZ();

    motor1.setMaxSpeed(motor1Config.fastSpeed);
    motor2.setMaxSpeed(motor2Config.fastSpeed);
    motor3.setMaxSpeed(motor3Config.fastSpeed);

    motor1.setAcceleration(motor1Config.acceleration);
    motor2.setAcceleration(motor2Config.acceleration);
    motor3.setAcceleration(motor3Config.acceleration);
}

void motorsEnableXY() {
    digitalWrite(motor1Config.enablePin, ENABLE_ACTIVE);
    digitalWrite(motor2Config.enablePin, ENABLE_ACTIVE);
    motorsEnabledXY = true;
}

void motorEnableZ() {
    digitalWrite(motor3Config.enablePin, ENABLE_ACTIVE);
    motorEnabledZ = true;
}

void motorsDisableXY() {
    digitalWrite(motor1Config.enablePin, ENABLE_INACTIVE);
    digitalWrite(motor2Config.enablePin, ENABLE_INACTIVE);
    motorsEnabledXY = false;
}

void motorDisableZ() {
    digitalWrite(motor3Config.enablePin, ENABLE_INACTIVE);
    motorEnabledZ = false;
}

// MAPEO DE NOMBRE PARA LOS MOTORES
// -----------------------------------------------------------------------
const char *motorName(MotorID id) {
    switch (id) {
        case MotorID::J1:
            return "J1";
        case MotorID::J2:
            return "J2";
        case MotorID::Z:
            return "Z";
        default:
            return "?"; // devuelve ? si no coincide ningun valor
    }
}

// STATUS DE MOTORES
// -----------------------------------------------------------------------
String motorStatus(MotorID id) {
    String status = "";
    float angle = 0.0;

    switch (id) {
        case MotorID::J1:
            angle = sensorCorrectedAngle(Wire, sensorHomingOffset(Wire));
            switch (motor1Homing.state) {
                case HomingStateXY::OK:
                    status += "MOTOR1 OK (" + String(angle, 1) + "°); ";
                    break;
                case HomingStateXY::ERROR:
                    status += "MOTOR1 ERROR; ";
                    break;
                case HomingStateXY::INACTIVE:
                    status += "MOTOR1 INACTIVE ";
                    break;
                default:
                    status += "MOTOR1 RUNNING (" + String(angle, 1) + "°); ";
                    break;
            }
            break;

        case MotorID::J2:
            angle = sensorCorrectedAngle(Wire1, sensorHomingOffset(Wire1));
            switch (motor2Homing.state) {
                case HomingStateXY::OK:
                    status += "MOTOR2 OK (" + String(angle, 1) + "°); ";
                    break;
                case HomingStateXY::ERROR:
                    status += "MOTOR2 ERROR; ";
                    break;
                case HomingStateXY::INACTIVE:
                    status += "MOTOR2 INACTIVE ";
                    break;
                default:
                    status += "MOTOR2 RUNNING (" + String(angle, 1) + "°); ";
                    break;
            }
            break;

        case MotorID::Z:
            // Para Z usamos reference porque no hay sensor angular
            switch (motor3Homing.state) {
                case HomingStateZ::OK:
                    status += "MOTOR3 OK (" + String(motor3Homing.reference) + "); ";
                    break;
                case HomingStateZ::ERROR:
                    status += "MOTOR3 ERROR; ";
                    break;
                case HomingStateZ::INACTIVE:
                    status += "MOTOR3 INACTIVE ";
                    break;
                default:
                    status += "MOTOR3 RUNNING (" + String(motor3Homing.reference) + "); ";
                    break;
            }
            break;
    }

    if (status == "")
        status = "IDLE";

    return status;
}

void stopAllMotors() {
    motor1.stop();
    motor2.stop();
    motor3.stop();

    motor1.setCurrentPosition(motor1.currentPosition());
    motor2.setCurrentPosition(motor2.currentPosition());
    motor3.setCurrentPosition(motor3.currentPosition());

    motor1.disableOutputs();
    motor2.disableOutputs();
    motor3.disableOutputs();
}

void setCurrentPositionHome() {
    motor1.setCurrentPosition(0);
    motor2.setCurrentPosition(0);
    motor3.setCurrentPosition(0);

    currentShoulderAngle = 0.0f;
    currentElbowAngle = 0.0f;

    COMM.println("STEPPERS HOMED (SOFTWARE ZERO SET)");
}