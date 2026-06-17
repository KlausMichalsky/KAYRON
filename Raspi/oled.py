# ========================================================================
#                      🔹P I C O   —   C H E S S🔹
# ========================================================================
#   Archivo    : oled.py
#   Autor      : Klaus Michalsky
#   Fecha      : 2025-12-04
# ------------------------------------------------------------------------
#   ▫️ DESCRIPCIÓN
#       Módulo encargado del control completo de la pantalla OLED.
#       Permite inicializar la pantalla, mostrar texto, centrar
#       mensajes, actualizar valores dinámicos y gestionar la interfaz.
#   ▫️ RESPONSABILIDADES:
#       - Inicializar y configurar correctamente la pantalla OLED.
#       - Mostrar texto y valores en diferentes posiciones.
#       - Centrar títulos y mensajes importantes en la pantalla.
#       - Actualizar datos dinámicos como ángulos o estado de sensores.
#       - Mantener la pantalla limpia y legible durante la ejecución.
# ========================================================================


import utime
from machine import Pin, I2C
from ssd1306 import SSD1306_I2C
from config import OLED_I2C_ID, OLED_I2C_SDA, OLED_I2C_SCL, OLED_FRQ, OLED_ANCHO, OLED_ALTO, CHAR_ANCHO, CHAR_ALTO

# OLED SSD1306
utime.sleep_ms(200)  # Pequeño delay para que el OLED termine de arrancar
i2c = I2C(OLED_I2C_ID, scl=Pin(OLED_I2C_SCL),
          sda=Pin(OLED_I2C_SDA), freq=OLED_FRQ)
oled = SSD1306_I2C(OLED_ANCHO, OLED_ALTO, i2c)


# INICIALIZACIÓN DE OLED
def oled_init():
    oled.fill(0)
    oled.text("Iniciando...", 0, 0)
    oled.show()
    utime.sleep(2)  # espera 500 ms


# MUESTRA UN TITULO CENTRADO EN LA PRIMERA LINEA
# Input: texto (str)
def mostrar_titulo(texto):
    oled.fill(0)
    centrar_texto(0, texto)
    oled.show()


# MOSTRAR TEXTO EN LINEA ESPECÍFICA
# Input: linea (int), texto (str)
def mostrar_texto(linea, texto):
    borrar_parte_inferior()
    oled.text(texto, 0, linea*CHAR_ALTO)
    oled.show()


# MOSTRAR TEXTO CENTRADO EN LINEA ESPECÍFICA
# Input: linea (int), texto (str)
def centrar_texto(linea, texto):
    borrar_parte_inferior()
    x = (OLED_ANCHO - len(texto)*CHAR_ANCHO)//2
    oled.text(texto, x, linea*CHAR_ALTO)
    oled.show()


# RELLENAR UN RECTÁNGULO (para borrar o dibujar)
# Input: (x, y, w, h). color: 0=borrar, 1=dibujar.
def fill_rect(x, y, w, h, color=0):
    for yy in range(y, y + h):
        for xx in range(x, x + w):
            oled.pixel(xx, yy, color)


# Borrar la parte inferior de la pantalla dejando la primera el Titulo (linea 0)
def borrar_parte_inferior():
    fill_rect(0, CHAR_ALTO, OLED_ANCHO, OLED_ALTO - CHAR_ALTO, 0)
    oled.show()


"""# Solicitar valores de ángulo
angulo_motor1 = "Introduce el valor del ángulo del Motor 1: "
angulo_motor2 = "Introduce el valor del ángulo del Motor 2: "
angulo_motor1 = int(input(angulo_motor1))
angulo_motor2 = int(input(angulo_motor2))

# Valor inicial
texto_titulo = "Angulos"
texto_motor1 = "Motor1:"
texto_motor2 = "Motor2:"
texto_angulo1 = f"{angulo_motor1} deg"
texto_angulo2 = f"{angulo_motor2} deg"


# Función para mostrar en OLED
def mostrar_angulos_oled(texto1, texto2, texto3, texto4, texto5):
    oled.fill(0)
    oled.text(texto1, ((OLED_ANCHO)-CHAR_ANCHO*len(texto_titulo))//2, 0)
    oled.text(texto2, 0, CHAR_ALTO)
    oled.text(texto4, 0, 2*CHAR_ALTO)
    oled.text(texto3, CHAR_ANCHO*len(texto_motor1)+CHAR_ANCHO, CHAR_ALTO)
    oled.text(texto5, CHAR_ANCHO*len(texto_motor2)+CHAR_ANCHO, 2*CHAR_ALTO)
    oled.show()


# Inicialización
mostrar_angulos_oled(texto_titulo, texto_motor1,
                     texto_angulo1, texto_motor2, texto_angulo2)
utime.sleep(1)  # espera 1 segundo para que el Zero arranque
"""
