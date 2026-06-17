# =======================================================================
#             🔹 C H E S S   S Q U A R E   T O   X Y 🔹
# =======================================================================
#  Archivo    : chess_square_to_xy.py
#  Autor      : Klaus Michalsky
#  Fecha      : May-2026
# -----------------------------------------------------------------------
#  ▫️ DESCRIPCIÓN
# -----------------------------------------------------------------------
#  Sistema de conversión de casillas de ajedrez (A1–H8) a coordenadas
#  cartesianas (X, Y) para control de un brazo robótico.
#  Diseñado para integración con sistema de
#  cinemática inversa para el robot KAYRON.
#
#   - Lee casillas desde consola (ej: E2)
#   - Valida formato de casillas de ajedrez
#   - Convierte la casilla a índices de tablero
#   - Calcula coordenadas físicas en mm aplicando offsets del tablero físico
#   - Sirve como base de posicionamiento del efector final
#   - Imprime resultados por consola
#
#  ✅ Funcional
# =======================================================================

import time
import config


def chess_square_to_xy(square):
    if len(square) != 2:
        return None

    file = square[0].upper()
    rank = square[1]

    if file < 'A' or file > 'H':
        return None

    if rank < '1' or rank > '8':
        return None

    file_index = ord(file) - ord('A')
    rank_index = ord(rank) - ord('1')

    x = config.A1_OFFSET_X + (file_index * config.SQUARE_SIZE)
    y = config.A1_OFFSET_Y + (rank_index * config.SQUARE_SIZE)

    return x, y


def print_board_xy():
    print("\n-------- BOARD XY DEBUG --------")

    for file in range(ord('A'), ord('H') + 1):
        for rank in range(ord('1'), ord('8') + 1):

            square = chr(file) + chr(rank)

            result = chess_square_to_xy(square)

            if result:
                x, y = result
                print(f"{square} -> X: {x} Y: {y}")

    print("-------- END BOARD --------\n")


# =========================================================
# 🔥 SOLO SE EJECUTA SI CORRÉS ESTE ARCHIVO DIRECTO
# =========================================================
if __name__ == "__main__":
    print("Iniciando...")
    print_board_xy()

    while True:
        square = input("Introduce casilla (ej: E2): ").strip()
        result = chess_square_to_xy(square)

        if result:
            x, y = result
            print(f"{square.upper()} -> X={x} Y={y}")
        else:
            print("Casilla inválida")
