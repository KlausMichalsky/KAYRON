# ========================================================================
#                 🔹 C H E S S B O T  -  Z E R O 🔹
# ========================================================================
#   Archivo    : config.py
#   Autor      : Klaus Michalsky
#   Fecha      : Feb. 2026
# ------------------------------------------------------------------------
#   ▫️ DESCRIPCIÓN
#       - Configuración de pines y parámetros del sistema
#   ▫️ RESPONSABILIDADES
#       - Definir parametros para UART y OLED
#       - Centralizar constantes de hardware
# ========================================================================

# CONFIGURACIÓN OLED
# ------------------------------------------------------------------------
OLED_I2C_ID = 0
OLED_I2C_SDA = 4
OLED_I2C_SCL = 5
OLED_FRQ = 400000
# Dimensiones del OLED
OLED_ANCHO = 128
OLED_ALTO = 64
CHAR_ANCHO = 8  # aproximado, según fuente SSD1306
CHAR_ALTO = 16

# CONFIGURACIÓN UART
# ------------------------------------------------------------------------
UART_ID = 0
UART_BAUD = 115200
# Pines UART (depende del hardware)
UART_TX_PIN = 0
UART_RX_PIN = 1

# CONFIGURACIÓN DEL TABLERO EN MM
# -----------------------------------------------------------------------
SQUARE_SIZE = 25.0
# Distancia desde DOF1 a centro Fila A
A1_OFFSET_X = -((3 * SQUARE_SIZE) + SQUARE_SIZE / 2.0)
A1_OFFSET_Y = 80.0  # Distancia desde DOF1 a centro Rango 1

# CONFIGURACIÓN DEL BRAZO ROBÓTICO
# -----------------------------------------------------------------------
L1 = 175.0  # Longitud del primer segmento del brazo
L2 = 95.0   # Longitud del segundo segmento del brazo
