# ♟️ ChessBot---Zero

Sistema de control para un robot de ajedrez basado en RP2040,
con comunicación UART entre Pi Zero y RP2040 Zero, visualización
mediante OLED SH1106.

---

## 🔧 Hardware utilizado
- Power Supply 12V > 8A
- Buck Converter 12V-5V > 8A 
- RP2040 Zero (control principa de motores y sensores) -> Zero
- Raspberry Pi Zero 2 W (control del arlgoritmo) -> Raspy
- Pantalla OLED SH1106 1.3" via I2C (visualizacion de estados del hardware)
- Motor Nema17 0.42Ncm
- Motor Nema17 0,17Ncm
- Mini 15mm 2-Phase Stepper 55mm Stroke linear actuator
- Drivers TMC2209 V4.0
- Level shifter 5V-3V3
- Touchscreen para Pi Zero (ToDo!)

---

## 💻 Software
- MicroPython (Pi Zero)
- Arduino / C++ (RP2040 Zero)
- Comunicación UART bidireccional

---

## ▶️ Ejecución básica
1. Flashear MicroPython en el Pi Zero.
2. Cargar el firmware Arduino en el RP2040 Zero.
3. Conectar ambos dispositivos por UART.
4. Ejecutar `main.py` en el Pi Zero.
5. Ingresar comandos por consola o botones
6. Mas adelante Touchscreen con GUI basica

<div style="page-break-before: always;"></div>

---

## 📘 Documentación
### 🔹 Archivos de Estructura
- `Estructura de funciones Raspy`
- `Estructura de funciones Zero`
- `Flujo de main.py`
- `Flujo de main_zero.ino`
- `Maquina de estados Raspy`
- `Maquina de estados Zero`

---

### 🔹 Archivos de Frimware para Raspberry Pi Zero 2 (MicroPython)
- `chess_engine`   (Algoritmo iA independiente)
- `comandos`      (API de comandos / Pi Zero as master -> RP2040 Zero as slave)
- `config`        (Configuracion general del sistema Pi Zero)
- `framebuf`      (Libreria par control de OLED SSD1306)
- `main`          (Main loop)
- `oled-manual`   (Control de OLED SSD1306)
- `ssd1306`       (Driver para OLED SSD1306)
- `uart_raspy`    (Comunicación UART entre RP2040 Zero y Pi Zero)


### 🔹 Archivos de Frimware para RP2040 Zero (Arduino / C++)
- `comunicacion`  (Comunicación UART entre RP2040 Zero y Pi Zero)
- `config`        (Configuración general del sistema RP2040 Zero)
- `filtro`        (Filtros y procesamiento de señales)
- `homing`        (Control de homing)
- `led`           (Control del indicador LED para maquina de estados)
- `main_zero.ino` (Main loop)
- `motores`       (Control y lógica de motores)
- `sensores`      (Lectura y gestión de sensores)

---

## ⚠️ Notas
- Los archivos `framebuf.py` y `ssd1306.py` son drivers base y no deben modificarse.
- El proyecto está pensado para uso experimental y educativo.

---
