// =======================================================================
//                          🔹 K A Y R O N 🔹
// =======================================================================
//  Archivo    : utils.cpp
//  Autor      : Klaus Michalsky
//  Fecha      : Feb-2026
// -----------------------------------------------------------------------
//  ▫️ DESCRIPCIÓN
//      - Implementación de funciones de cálculo y conversiónes
//      - Ángulos a pasos de motores
//      - Casilla a coordenadas
//      - Casilla a ángulos
//      - Cinemática inversa
// =======================================================================

#include <Arduino.h>
#include <math.h>

#include "communication.h"
#include "config.h"
#include "sensors.h"
#include "utils.h"

// CONVERCION DE ANGULO A PASOS DE MOTOR
// -----------------------------------------------------------------------
long angleToStep(float angle, MotorID id) {
    switch (id) {
        case MotorID::J1:
            return (angle / 360.0) *
                   motor1Config.microstepping *
                   motor1Config.reduction *
                   motor1Config.stepsPerRevolution;
        case MotorID::J2:
            return (angle / 360.0) *
                   motor2Config.microstepping *
                   motor2Config.reduction *
                   motor2Config.stepsPerRevolution;
        default:
            // Por si llega un MotorID inválido
            COMM.println("ERROR: MotorID inválido en angleToStep");
            return 0;
    }
}

// CONVERCION DE VALOR BRUTO DE 12 BITS DEL AS5600 (0-4095) A GRADOS
// -----------------------------------------------------------------------
float rawToDegrees(uint16_t rawAngle) {
    return (rawAngle * 360.0) / 4096.0;
}

// REDONDEO DE FLOAT A 1 DECIMAL
// -----------------------------------------------------------------------
float round1Decimal(float value) {
    return ((int)(value * 10 + 0.5)) / 10.0;
}

// CONVERSIÓN DE CASILLA → XY
// -----------------------------------------------------------------------
bool chessSquareToXY(
    const String &square,
    float &x,
    float &y) {
    // Validar longitud
    if (square.length() != 2) { // Verifica que el texto tenga exactamente 2 caracteres.
        return false;
    }

    // Obtener letra y numero
    char file = toupper(square[0]); // Optener la primera letra y convertir a mayuscula
    char rank = square[1];          // obtener el numero

    // Validar fila y ángulo
    if (file < 'A' || file > 'H') {
        return false;
    }
    if (rank < '1' || rank > '8') {
        return false;
    }

    // Convertir a índices
    int fileIndex = file - 'A'; // 'A' -> 0, 'B' -> 1, ..., 'H' -> 7
    int rankIndex = rank - '1'; // '1' -> 0, '2' -> 1, ..., '8' -> 7

    // Convertir a coordenadas en centro de casillas
    x = A1_OFFSET_X + (fileIndex * SQUARE_SIZE);
    y = A1_OFFSET_Y + (rankIndex * SQUARE_SIZE);

    return true;
}

// CONVERSIÓN XY → ANGULOS (CINEMATICA INVERSA)
// -----------------------------------------------------------------------
// “devuelve” theta1 y theta2 porque -> & modifica variables mediante referencias
// C++ usa: sin, cos, atan2, acos -> todo en radianes -> PI rad = 180°
bool inverseKinematics(
    float x,
    float y,
    float l1,
    float l2,
    float &theta1,
    float &theta2) {
    float r2 = x * x + y * y;
    float r = sqrtf(r2);

    // Evaluar si la coordenada esta fuera de alcance
    if (r > (l1 + l2) || r < fabsf(l1 - l2)) {
        return false;
    }

    // Ley de cosenos para theta2 -> c2=a2+b2−2abcos(C)
    float cos_theta2 = (r2 - l1 * l1 - l2 * l2) / (2.0f * l1 * l2);

    // Limitacón [-1, 1] para que esta condición −1≤cos(θ2​)≤1 sea valida
    // evita que cos_theta2 tenga valores como: 1.000001 o -1.000002
    // que pueden aparecer por errores de precisión de float
    // y que causarían que acosf falle o devuelva NaN
    if (cos_theta2 > 1.0f)
        cos_theta2 = 1.0f;
    if (cos_theta2 < -1.0f)
        cos_theta2 = -1.0f;

    // Calculo de theta2 usando atan2
    // Tambien se puede calcular con θ2 = arccos(...) -> acosf(cos_theta2)
    // y eso funciona perfectamente.
    // La razón por la que muchas implementaciones usan
    // atan2f(sin_theta2, cos_theta2) es porque atan2:
    // es más estable numéricamente preserva mejor el cuadrante
    // permite elegir fácilmente entre ambas soluciones elbow-up / elbow-down
    // evita algunas ambigüedades de acos porque acos() devuelve solamente: 0 → π
    // mientras que atan2(y, x) usa: seno (y) coseno (x) para reconstruir el ángulo completo.
    // atan2f(y, x) significa: arctangent(y / x) pero usando DOS parámetros. La f es float version
    // atan(y / x) pierde información del cuadrante.
    // Ejemplo: y=1 x=1 -> 45° y=-1 x=-1 -> también 45° porque la división da el mismo resultado.
    // En cambio: atan2(y, x) sí sabe en qué cuadrante estás.
    // θ = atan2(sinθ, cosθ) reconstruye el ángulo completo usando ambas componentes trigonométricas.
    theta2 = atan2f(
        sqrtf(1.0f - cos_theta2 * cos_theta2),
        cos_theta2);

    // Flip según lado
    // Ese bloque cambia el signo de theta2 dependiendo de qué lado del eje X está el objetivo.
    // si el punto está a la derecha (x > 0) → usa el ángulo normal
    // si el punto está a la izquierda (x < 0) → invierte el ángulo
    // invierte el seno: sin(−θ)=−sin(θ) pero mantiene el coseno: cos(−θ)=cos(θ)
    // Entonces el brazo conserva la misma distancia al objetivo, pero se dobla hacia el otro lado.
    // En cinemática inversa de brazos 2D siempre existen dos soluciones: codo arriba y codo abajo
    // Ese -theta2 está seleccionando la otra solución dependiendo del lado del tablero/plano.
    if (x < 0.0f) {
        theta2 = -theta2;
    }

    // Cinemática inversa
    // componentes auxiliares para calcular theta1
    // Geométricamente forman el vector resultante desde la base hasta el objetivo intermedio del brazo.
    // k1 es la componente horizontal efectiva.
    // l1 → primer brazo + l2*cos(theta2) → proyección horizontal del segundo brazo
    // k2 es la componente vertical del segundo brazo -> cuánto sube/baja el segundo segmento
    // atan2(y,x) -> ángulo hacia el objetivo
    // atan2(k2,k1) = ángulo interno del triángulo del brazo
    // y la resta da el ángulo real del hombro
    // Visualmente: base ---- l1 ---- joint ---- l2 ---- target
    // k1 y k2 son básicamente las coordenadas del vector combinado del brazo respecto a la articulación base
    float k1 = l1 + l2 * cosf(theta2);
    float k2 = l2 * sinf(theta2);

    float theta1_raw = atan2f(y, x) - atan2f(k2, k1);

    // Offset mecánico
    // sirve para:
    // rotar el sistema de referencia
    // adaptar la matemática al robot real
    // cambiar dónde está el “0°”
    // posiblemente invertir el sentido de giro
    // matemáticamente atan2() usa este sistema:
    // 0° → eje X positivo (derecha)
    // 90° → arriba
    // pero muchos brazos robóticos usan otro sistema, por ejemplo:
    // 0° → arriba
    // +ángulos → sentido horario
    // Entonces necesitás convertir entre:
    // coordenadas matemáticas
    // coordenadas mecánicas del robot
    // Supongamos:
    // theta1_raw = 0°
    // Eso en matemáticas significa:
    // apuntando a la derecha
    // Pero quizá en tu robot:
    // 0° debe ser hacia arriba
    // Entonces hacés:
    // 90° - 0° = 90°
    // y el sistema queda alineado con tu montaje físico.
    theta1 = (float)(M_PI_2)-theta1_raw; // theta1 = 90° - theta1_raw

    // Simetría física
    // Si el robot tiene una simetría que hace que el mismo ángulo de motor
    // produzca un movimiento en dirección opuesta, entonces se puede invertir el ángulo.
    // Por ejemplo, si el motor está montado de tal forma que un giro positivo
    // hace que el brazo se mueva hacia la izquierda en lugar de hacia la derecha, entonces invertir theta1
    theta1 = -theta1;

    // Normalización [-pi, pi]
    // normaliza theta1 para que siempre quede dentro del rango: −π≤θ1​≤π
    theta1 = fmodf(theta1 + (float)M_PI, 2.0f * (float)M_PI); // fmodf(a,b) devuelve el resto de la división:
    if (theta1 < 0)
        theta1 += 2.0f * (float)M_PI;
    theta1 -= (float)M_PI;

    // reflejan los ángulos
    // cambian el cuadrante
    // mantienen continuidad
    // evitan que el brazo dé una vuelta completa innecesaria
    if (x < 0.0f) {
        theta1 = (float)M_PI - theta1;
        theta2 = -((float)M_PI) - theta2;
    } else if (x > 0.0f) {
        theta1 = -((float)M_PI) - theta1;
        theta2 = (float)M_PI - theta2;
    }

    return true;
}

// CONVERSION DE CASSILA A ANGULOS
// ➡️ Casilla -> XY -> IK (radianes) -> Converción en ángulos
bool chessSquareToAngles(
    const String &square,
    float &theta1Deg,
    float &theta2Deg) {
    float x, y;

    // Casilla -> XY
    if (!chessSquareToXY(square, x, y)) {
        return false;
    }

    float theta1Rad, theta2Rad;

    // XY → IK
    if (!inverseKinematics(
            x,
            y,
            LINK1,
            LINK2,
            theta1Rad,
            theta2Rad)) {
        return false;
    }

    // Rad -> deg
    theta1Deg = theta1Rad * 180.0f / M_PI;
    theta2Deg = theta2Rad * 180.0f / M_PI;

    return true;
}

// DEBUG
// TEST: IMPRIMIR TODAS LAS CASILLAS COMO COORDENADAS XY
// -----------------------------------------------------------------------
void printBoardXY() {
    COMM.println();
    COMM.println("-------- BOARD XY DEBUG --------");

    for (char file = 'A'; file <= 'H'; file++) {
        for (char rank = '1'; rank <= '8'; rank++) {
            String square = String(file) + String(rank);

            float x, y;

            delay(10);

            if (chessSquareToXY(square, x, y)) {
                COMM.print(square);
                COMM.print(" -> X: ");
                COMM.print(x);
                COMM.print(" Y: ");
                COMM.println(y);
            }
        }
    }

    COMM.println("-------- END BOARD --------");
    COMM.println();
}

float shortestAngle(float target, float current) {
    float diff = target - current;

    while (diff > 180)
        diff -= 360;
    while (diff < -180)
        diff += 360;

    return current + diff;
}
