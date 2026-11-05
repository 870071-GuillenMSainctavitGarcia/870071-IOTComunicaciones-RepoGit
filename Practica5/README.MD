# Práctica 5: Adquisición, Transmisión y Procesamiento en Tiempo Real de Datos Inerciales

Este repositorio contiene la solución completa de la **Práctica 5**, cuyo objetivo es establecer un sistema end-to-end de adquisición de datos inerciales empleando la IMU integrada en el microcontrolador **Arduino Nano 33 BLE**, su envío por comunicación serie (UART) hacia un PC cada 50 ms y su posterior captura, almacenamiento en fichero compatible con Excel y representación gráfica en tiempo real con estadísticas deslizantes en **Python**.

---

##  Arquitectura del Sistema

```
[ Sensor IMU ] --(I2C)--> [ Arduino Nano 33 BLE ] --(UART/USB @ 115200 baud)--> [ Script Python ]
                                                                                      │
                                                                   ┌──────────────────┴──────────────────┐
                                                                   ▼                                     ▼
                                                         [ Fichero .txt (CSV) ]         [ Matplotlib TkAgg (Tiempo Real) ]
                                                         Separador: ';'                  Ventana móvil: 5s (100 pts)
                                                         Fila: Ax;Ay;Az\n                Estadísticas: Promedio y Desviación

```

---

##  Estructura del Repositorio

```text
.
├── arduino/
│   └── practica5_nano33ble.ino   # Firmware C++ para lectura I2C y envío UART cada 50 ms
│
├── python/
│   ├── apartado_3_1.py           # Validación de generación de fichero .txt fijo (; y \n)
│   ├── apartado_3_2.py           # Captura continua por UART y guardado en archivo
│   ├── apartado_4.py             # Monitorización en tiempo real (3 subgráficas + estadísticas)
│   ├── datos_acelerometro.txt    # Registro de lecturas generado por apartado_3_2
│   └── prueba_fija.txt           # Fichero de prueba de estructura
│
├── .gitignore                    # Filtro de temporales (.vs, .idea, __pycache__, .venv)
└── README.md                     # Documentación general del proyecto

```

---

##  Requisitos del Sistema

### Hardware

* Placa **Arduino Nano 33 BLE** (o BLE Sense).
* Cable Micro-USB con capacidad de transmisión de datos.

### Software

* **Arduino IDE** (v2.0 o superior recomendada).
* **Python** (v3.8 o superior).
* **PyCharm** o entorno de desarrollo equivalente.

### Librerías Requeridas

#### En Arduino IDE:

* `Arduino_LSM9DS1` (para placas Nano 33 BLE Rev1) **o** `Arduino_BMI270_BMM150` (para la revisión Rev2).

#### En Python:

Instalar mediante terminal o consola de PyCharm:

```bash
pip install pyserial matplotlib numpy

```

---

##  Especificaciones Técnicas

| Parámetro | Valor / Especificación |
| --- | --- |
| **Microcontrolador** | nRF52840 (Arduino Nano 33 BLE) |
| **Sensor de Aceleración** | IMU Integrada LSM9DS1 / BMI270 (Lectura en $g$) |
| **Baud Rate (UART)** | `115200 bps` (ó 9600 bps) |
| **Período de Muestreo** | `50 ms` (Frecuencia: $20\text{ Hz}$) |
| **Formato de Trama Serie** | `Ax;Ay;Az\n` (Números de precisión flotante) |
| **Ventana Temporal (Apartado 4)** | `5 segundos` ($5\text{ s} \times 20\text{ Hz} = 100\text{ muestras}$) |
| **Estadísticas Calculadas** | Promedio ($\mu$) y Desviación Estándar ($\sigma$) por eje |

---

##  Guía de Puesta en Marcha

### Paso 1: Carga del Firmware en Arduino

1. Abre `arduino/practica5_nano33ble.ino` en **Arduino IDE**.
2. Conecta la placa **Arduino Nano 33 BLE** al ordenador por USB.
3. Selecciona la tarjeta en `Herramientas > Placa > Arduino Mbed OS Nano Boards > Arduino Nano 33 BLE`.
4. Elige el puerto COM asignado en `Herramientas > Puerto`.
5. Compila y sube el código.

>  **Importante:** Cierra el Monitor Serie de Arduino IDE antes de pasar a la etapa de Python para evitar el error de puerto serie ocupado (`Access Denied`).

---

### Paso 2: Ejecución de la Parte Python

#### Apartado 3.1 — Verificación de Fichero Fijo

Ejecuta el script para comprobar la creación limpia de ficheros delimitados por `;`:

```bash
python python/apartado_3_1.py

```

*Generará el archivo `prueba_fija.txt` apto para importar directamente desde Microsoft Excel.*

#### Apartado 3.2 — Almacenamiento Serie en Tiempo Real

Asegúrate de ajustar el nombre del puerto (`PUERTO = 'COM3'`) en el script al puerto correspondiente de tu placa:

```bash
python python/apartado_3_2.py

```

*Capturará las tramas del acelerómetro y las guardará continuamente en `datos_acelerometro.txt`.*

#### Apartado 4 — Representación Gráfica Animada (Ventana de 5s)

Ejecuta la aplicación principal de visualización:

```bash
python python/apartado_4.py

```

* **Comportamiento:**
* Abre una ventana flotante mediante el backend `TkAgg` de Matplotlib.
* Muestra 3 subgráficas independientes para los ejes $A_x$, $A_y$ y $A_z$.
* Utiliza un búfer circular `deque(maxlen=100)` que mantiene dinámicamente solo los últimos 5 segundos de muestreo.
* Recalcula en cada frame de `FuncAnimation` el promedio y la desviación estándar de la ventana activa.



---

##  Resolución de Problemas Frecuentes

* **`serial.serialutil.SerialException: PermissionError`**: El puerto COM está abierto en otro software. Cierra TERMITE, PuTTY o el Monitor Serie de Arduino IDE.
* **Gráfica estática o congelada en PyCharm**: Se ha forzado el backend interactivo mediante `matplotlib.use('TkAgg')` al inicio de `apartado_4.py` para evitar que la ventana integrada "SciView" de PyCharm detenga el bucle de actualización.
* **`Desv: 0.000` permanente**: Indica que solo está llegando una muestra. Revisa que la velocidad en baudios coincida en Arduino y Python (`115200`) y que el `delay(50)` en Arduino no esté dentro de un bloque condicional cerrado.