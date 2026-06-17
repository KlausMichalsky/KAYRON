# ✅ ToDos — Proyecto: **ChessBot---Zero**


## 📚 Documentación

---

### 📐⚒️ Construccion de prototipos


| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | breadboard para test de codigo motor1, motor2, codificador1, codificador2, sensor hall1, sensor hall2| |
|      | breadboard para test de codigo motor3, sensor hall y electroiman| |
|      | breadboard para test de codigo tablero| |


---

### 🤖 Software — RP2040 Zero

> Inventario de archivos actuales del firmware del RP2040 Zero (C++).
> Control de motores, codificadores y sensores 



| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | main_zero.ino | |
|      | config.h | |
|      | comunicacion.h | |
|      | comunicacion.cpp | |
|      | comunicacion-manual.md | |
|      | filtro.h | |
|      | filtro.cpp | |
|      | filtro-manual.md | |
|      | homing.h | |
|      | homing.cpp | |
|      | homing-manual.md | |
|      | led.h | |
|      | led.cpp | |
|      | led-manual.md | |
|      | motores.h | |
|      | motores.cpp | |
|      | motores-manual.md | |
|      | sensores.h | |
|      | sensores.cpp | |
|      | sensores-manual.md | |

---

### 🧠 Software — Raspberry Pi Zero 2 W

> Archivos del software de alto nivel (Python).
> Control de juego, comunicación, lógica y display.


| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | oled-manual.md | |
|      | main.py | |
|      | config.py | |
|      | utils.py | |
|      | chess_engine.py | |
|      | comunicacion.py | |
|      | comandos.py | |
|      | framebuf.py | |
|      | oled.py | |
|      | ssd1306.py | |

---


### 💻 Código — Firmware (Microcontroladores)

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | main_zero.ino | |
|      | manejo de estados del robot | |
|      | máquina de estados de homing | |
|      | control de motores (step/dir o driver) | |
|      | control de corriente y límites | |
|      | manejo de endstops (sensores) | |
|      | debounce de botones | |
|      | detección de errores (faults) | |
|      | recuperación ante errores | |
|      | watchdog / seguridad | |
|      | configuración por constantes | |

---

### 🧠 Código — Software (Raspberry Pi Zero 2 W)

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | main.py | |
|      | integración con Stockfish | |
|      | parser de movimientos de ajedrez | |
|      | cinematica inversa → coordenadas físicas | |
|      | comunicación UART con Zero | |
|      | control de estados del juego | |
|      | detección de errores de ejecución | |
|      | logging del sistema | |
|      | modo simulación (sin hardware) | |
|      | interfaz para futura pantalla táctil | |

---

### 🔌 Electrónica

#### Placas (PCBs)

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | Mainboard_Brazo — esquemático | |
|      | Mainboard_Brazo — PCB layout | |
|      | Mainboard_Tablero — esquemático | |
|      | Mainboard_Tablero — PCB layout | |
|      | lista de componentes (BOM) | |
|      | revisión de consumo total | |

#### Cableado

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | diagrama de cableado general | |
|      | cableado motores | |
|      | cableado endstops (sensores) | |
|      | cableado sensores tablero | |
|      | cableado alimentación | |
|      | gestión de masas y ruido | |
|      | etiquetado de cables | |

---

### ⚙️ Mecánica

#### CAD

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | DOF1 — diseño CAD | |
|      | DOF2 — diseño CAD | |
|      | DOF3 — diseño CAD | |
|      | soporte del efector final | |
|      | diseño del tablero | |
|      | diseño de carcasas | |
|      | tolerancias mecánicas | |


#### Planos y fabricación

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | drawings mecánicos | |
|      | BOM de tornillería | |
|      | BOM piezas impresas 3D | |
|      | BOM piezas standad | |

#### Ensamblaje

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | ensamblaje DOF1 | |
|      | ensamblaje DOF2 | |
|      | ensamblaje DOF3 | |
|      | montaje de motores | |
|      | montaje de sensores | |
|      | ajuste mecánico fino | |

---

### ♟️ Sistema de Ajedrez

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | detección de piezas en tablero | |
|      | mapeo tablero ↔ coordenadas reales | |
|      | validación de movimientos | |
|      | gestión de capturas | |
|      | detección de enroque / jaque / jaque mate | |

---

### 🧪 Pruebas y Calibración

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | prueba individual de cada motor | |
|      | prueba de homing repetible | |
|      | calibración de pasos por mm | |
|      | prueba de precisión | |
|      | prueba de ciclo largo (stress test) | |
|      | prueba de recuperación tras error | |

---

### 🧰 Mantenimiento y Futuro

| Estado | ToDo | Comentarios |
|------|------|-------------|
|      | mejoras mecánicas futuras | |
|      | mejoras electrónicas futuras | |
|      | optimización de tiempos de movimiento | |
|      | integración de touchscreen | |
|      | modo demostración | |
|      | modo torneo | |

---

📅 *Este archivo está pensado para evolucionar junto con el proyecto.*

