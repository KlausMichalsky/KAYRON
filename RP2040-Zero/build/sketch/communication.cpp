#line 1 "C:\\Users\\Klaus\\Documents\\KAYRON\\RP2040-Zero\\communication.cpp"
// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : communication.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Inicializar UART
//      - Comunicación UART con el Raspberry
//      - Gestionar la recepción de datos de forma no bloqueante.
// =======================================================================

#include <Arduino.h>

#include "communication.h"
#include "config.h"

// CONFIGURACIÓN DE DEPURACIÓN UART
// -----------------------------------------------------------------------
void debug(const String &msg) {
#if DEBUG_UART
    COMM.println("Comando recibido: " + msg);
#endif
}

// INICIALIZACIÓN DE COMUNICACION UART
// -----------------------------------------------------------------------

// Si usas Serial1 para comunicacion TX/RX, descomenta esta función y comenta la siguiente
// void communicationInit() {
//     // USB para debug (opcional)
//     COMM.begin(115200);

//     // UART hardware en pines 0 y 1
//     COMM.setTX(0);
//     COMM.setRX(1);
//     COMM.begin(115200);
//     while (COMM.available())
//         COMM.read(); // limpia buffer UART
// }

// Si usas Serial comunicacion por USB, descomenta esta función y comenta la anterior
void communicationInit() {
    // USB para debug (opcional)
    COMM.begin(115200);

    while (COMM.available())
        COMM.read(); // limpia buffer UART
}
