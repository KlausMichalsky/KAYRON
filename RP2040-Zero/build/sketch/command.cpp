#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\command.cpp"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : command.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Definición de funciones para la gestión de comandos
//        recibidos por UART
// =======================================================================

#include <Arduino.h>

#include <AccelStepper.h>

#include "Z_Axis.h"
#include "command.h"
#include "config.h"
#include "core.h"
#include "homing.h"
#include "motors.h"
#include "sensors.h"
#include "utils.h"
#include "xy_plane.h"

// VARIABLES EXTERNAS
// -----------------------------------------------------------------------
extern HomingXY motor1Homing;
extern HomingXY motor2Homing;
extern HomingZ motor3Homing;
extern bool homeAllActive;

// COMPROBACIÓN DE COMANDOS DISPONIBLES
// -----------------------------------------------------------------------
// Si usas Serial1 para comunicacion TX/RX, descomenta esta función y comenta la siguiente
// bool commandAvailable() {
//     return Serial1.available();
// }

// Si usas Serial comunicacion por USB, descomenta esta función y comenta la anterior
bool commandAvailable() {
    return COMM.available();
}

// ENVIO DE RESPUESTA DE STATUS DE TODOS LOS MOTORES Y SENSORES
// -----------------------------------------------------------------------
// Si usas Serial1 para comunicacion TX/RX, descomenta esta función y comenta la siguiente
// void commandSendStatusReport() {
//     String report = "";
//     report += motorStatus(MotorID::J1) + "\n";
//     report += motorStatus(MotorID::J2) + "\n";
//     report += motorStatus(MotorID::Z) + "\n";

//     Serial1.print(report);
// }

// Si usas Serial comunicacion por USB, descomenta esta función y comenta la anterior
void commandSendStatusReport() {
    String report = "";
    report += motorStatus(MotorID::J1) + "\n";
    report += motorStatus(MotorID::J2) + "\n";
    report += motorStatus(MotorID::Z) + "\n";

    COMM.print(report);
}

// LECTURA DE COMANDOS
// -----------------------------------------------------------------------
String readCommand() {
    static String buffer = "";
    while (COMM.available()) {
        char c = COMM.read();
        if (c == '\n') {
            String cmd = buffer;
            buffer = "";
            cmd.trim();
            return cmd;
        } else {
            buffer += c;
        }
    }
    return ""; // no hay comando completo todavía
}

// MAPEO DE STRING A ENUM CLASS COMMAND
// -----------------------------------------------------------------------
Command parseCommand(const String &cmd) {
    if (cmd == "STATUS")
        return Command::STATUS;
    else if (cmd == "RESET")
        return Command::RESET;
    else if (cmd == "ANGLES")
        return Command::ANGLES;
    else if (cmd == "BOARD")
        return Command::BOARD;
    else if (cmd == "COMMANDS")
        return Command::COMMANDS;
    else if (cmd == "HOME")
        return Command::HOME;
    else if (cmd == "HOMING")
        return Command::HOMING;
    else if (cmd.startsWith("MOVE"))
        return Command::MOVE;
    else if (cmd.startsWith("REMOVE"))
        return Command::REMOVE;
    else if (cmd.startsWith("SQUARE"))
        return Command::SQUARE;
    else
        return Command::UNKNOWN;
}

// PROCESAMIENTO DE COMANDOS
// -----------------------------------------------------------------------
void processCommand(const String &cmdStr) {
    String trimmedCmd = cmdStr;
    trimmedCmd.trim(); // eliminar espacios al iniciofinal,
                       // justo después de crear trimmedCmd

    Command cmd = parseCommand(trimmedCmd);

    switch (cmd) {
        case Command::STATUS:
            commandSendStatusReport();
            break;

        case Command::RESET:

            // 1. parar cualquier movimiento activo
            stopAllMotors();      // ⬅️ IMPORTANTE (si no existe, hay que crearla)
            cancelMoveSequence(); // ⬅️ clave si tenés planner
            resetXYState();       // 🔥 ESTO ES CLAVE

            // 2. reset flags de homing
            motor1Homing.fault = false;
            motor2Homing.fault = false;
            motor3Homing.fault = false;

            // 3. reset estados de homing
            homingInitXY(motor1Homing);
            homingInitXY(motor2Homing);
            homingInitZ(motor3Homing);

            // 4. reset máquina de estados global
            homeAllState = HomeAllState::IDLE;

            // 5. reset posición lógica (MUY IMPORTANTE)
            setCurrentPositionHome(); // ⬅️ o algo equivalente en tu sistema

            COMM.println("SYSTEM RESET OK");
            break;

        case Command::ANGLES:
            COMM.print("ANGLE1: ");
            COMM.println(rawToDegrees(sensorReadRawAngle(Wire)), 1);
            COMM.print("ANGLE2: ");
            COMM.println(rawToDegrees(sensorReadRawAngle(Wire1)), 1);
            COMM.println();
            break;

        case Command::BOARD:
            printBoardXY();
            break;

        case Command::COMMANDS:
            break;

        case Command::HOME:
            moveToHomeXY();
            COMM.println("HOME DONE"); // 👈 ESTO ES CLAVE
            break;

        case Command::HOMING:
            COMM.println("HOMING SEQUENCE STARTED");
            homeAllState = HomeAllState::MOTOR1;
            break;

        case Command::MOVE: {
            char startSquare[4] = {0};
            char endSquare[4] = {0};

            int parsed = sscanf(trimmedCmd.c_str(),
                                "MOVE %3s %3s",
                                startSquare,
                                endSquare);

            if (parsed != 2) {
                COMM.println("ERROR");
                break;
            }

            if (strcmp(startSquare, endSquare) == 0) {
                COMM.println("ERROR SAME SQUARE");
                break;
            }

            float s1, s2;
            float e1, e2;

            if (!chessSquareToAngles(String(startSquare), s1, s2)) {
                COMM.println("ERROR START");
                break;
            }

            if (!chessSquareToAngles(String(endSquare), e1, e2)) {
                COMM.println("ERROR END");
                break;
            }

            // SOLO UNA LLAMADA
            startMoveSequence(s1, s2, e1, e2);

            break;
        }

        case Command::REMOVE: {
            char square[4] = {0};

            int parsed = sscanf(trimmedCmd.c_str(),
                                "REMOVE %3s",
                                square);

            if (parsed != 1) {
                COMM.println("ERROR");
                break;
            }

            float theta1, theta2;

            if (!chessSquareToAngles(String(square), theta1, theta2)) {
                COMM.println("ERROR");
                break;
            }

            startCaptureSequence(theta1, theta2);

            COMM.println("REMOVE STARTED");
            break;
        }

        default: {
            // ==========================================================
            // 🧠 NUEVO: soporte para Stockfish (UCI: e2e4)
            // ==========================================================

            if (trimmedCmd.length() == 4 &&
                isalpha(trimmedCmd[0]) &&
                isdigit(trimmedCmd[1]) &&
                isalpha(trimmedCmd[2]) &&
                isdigit(trimmedCmd[3])) {
                float s1, s2;
                float e1, e2;

                String startSquare = trimmedCmd.substring(0, 2);
                String endSquare = trimmedCmd.substring(2, 4);

                if (!chessSquareToAngles(startSquare, s1, s2)) {
                    COMM.println("ERROR START");
                    break;
                }

                if (!chessSquareToAngles(endSquare, e1, e2)) {
                    COMM.println("ERROR END");
                    break;
                }

                COMM.print("OK:");
                COMM.println(trimmedCmd);

                COMM.print("START=");
                COMM.println(startSquare);

                COMM.print("END=");
                COMM.println(endSquare);

                startMoveSequence(s1, s2, e1, e2);

                break;
            }

            COMM.println("UNKNOWN COMMAND");
            break;
        }
    }
}