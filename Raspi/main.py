"""
KAYRON
main.py - Controlador para Raspberry Pi
Raspberry Pi Controller
Stockfish + UART RP2040 + HOMING automático
"""

import glob
import time
import chess
import chess.engine
import serial
from chess.engine import EngineTerminatedError

# =========================
# CONFIG
# =========================

ports = glob.glob("/dev/ttyACM*") + glob.glob("/dev/ttyUSB*")

if not ports:
    raise Exception("No se encontró RP2040 conectado")

SERIAL_PORT = ports[0]
print("Usando puerto:", SERIAL_PORT)

BAUDRATE = 115200
STOCKFISH_PATH = "/usr/games/stockfish"
THINK_TIME = 0.1

# =========================
# INIT SERIAL
# =========================

print("🤖 Iniciando KAYRON...")

ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)

time.sleep(2)

# =========================
# RESET ROBOT
# =========================


def reset_robot(ser):
    ser.write(b"RESET\n")
    time.sleep(0.5)
    ser.reset_input_buffer()


print("Reset RP2040...")
reset_robot(ser)

# =========================
# STOCKFISH INIT
# =========================

print("Iniciando Stockfish...")
t0 = time.time()

engine = chess.engine.SimpleEngine.popen_uci(
    STOCKFISH_PATH,
    timeout=30.0
)

print("Stockfish OK")
print(f"Tiempo de inicio: {time.time() - t0:.2f} segundos")

board = chess.Board()

time.sleep(1)

# =========================
# UART HELPERS
# =========================


def send_to_robot(cmd):
    ser.write((cmd + "\n").encode())


def wait_done():
    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line:
            print("RP2040:", line)
            if "DONE" in line:
                break

# =========================
# HOMING
# =========================


def do_homing():
    print("🤖 Enviando HOMING...")
    ser.write(b"HOMING\n")

    while True:
        line = ser.readline().decode(errors="ignore").strip()
        if line:
            print("RP2040:", line)
            if "DONE" in line:
                print("✅ HOMING COMPLETO")
                break

# =========================
# STOCKFISH MOVE
# =========================


def get_best_move():
    result = engine.play(board, chess.engine.Limit(time=THINK_TIME))
    return result.move.uci()

# =========================
# SHUTDOWN
# =========================


def shutdown():
    try:
        engine.quit()
    except:
        pass
    try:
        ser.close()
    except:
        pass


def wait_for(expected):
    while True:
        line = ser.readline().decode(errors="ignore").strip()

        if line:
            print("RP2040:", line)

            if line == expected:
                return

# =========================
# START
# =========================


print("🤖 ZERO-CHESS READY")

do_homing()

mode = input("¿Quién empieza? (1=Humano, 2=Robot): ").strip()
human_turn = (mode == "1")

print("\n♟️ Iniciando partida...\n")


# =========================
# LOOP PRINCIPAL
# =========================

while True:

    # =====================
    # TURNO HUMANO
    # =====================

    if human_turn:

        move = input("\n♟️ Tu jugada (ej: e2e4) | q = salir: ").strip().lower()

        if move == "q":
            print("👋 Saliendo...")
            shutdown()
            break

        try:
            human_move = chess.Move.from_uci(move)

            if human_move not in board.legal_moves:
                print("❌ Jugada ilegal")
                continue

        except ValueError:
            print("❌ Formato inválido")
            continue

        board.push(human_move)
        print("👤 Humano:", move)

        human_turn = False

    # =====================
    # TURNO ROBOT
    # =====================

    else:

        ser.reset_input_buffer()

        stockfish_move = get_best_move()
        move = chess.Move.from_uci(stockfish_move)

        print("🤖 Stockfish:", stockfish_move)

        # =========================
        # CAPTURA: ORDEN CORRECTO
        # =========================
        if board.is_capture(move):

            # 1. sacar pieza enemiga
            capture_square = chess.square_name(move.to_square)
            send_to_robot(f"REMOVE {capture_square}")
            wait_for("CAPTURE DONE")

            send_to_robot(stockfish_move)
            wait_for("MOVE DONE")

            send_to_robot("HOME")
            wait_for("HOME DONE")

        else:
            # movimiento normal
            send_to_robot(stockfish_move)
            wait_done()

        # volver a home SIEMPRE al final
        send_to_robot("HOME")
        wait_done()

        board.push(move)

        human_turn = True

    # =====================
    # FIN PARTIDA
    # =====================

    if board.is_game_over():
        print("\n🏁 Fin de partida")
        print(board.result())
        shutdown()
        break
