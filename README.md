# Repositorio de Prácticas — Laboratorio de Comunicaciones IoT

Este repositorio reúne el código fuente, desarrollos en C++ (Arduino/ESP32) y programas en Python correspondientes a las prácticas de la asignatura **Laboratorio de Comunicaciones IoT** de la **Universidad de Zaragoza (UNIZAR)**.

* **Alumno:** Guillén Sainctavit
* **NIP:** 870071
* **Universidad:** Universidad de Zaragoza (UNIZAR)

---

##  Estructura del Repositorio

Cada práctica cuenta con su propio directorio dedicado y un archivo `README.md` detallado con instrucciones específicas de configuración, hardware y ejecución.

```text
.
├── Practica_1/    # Arquitectura Arduino Nano 33 BLE
├── Practica_2/    # Sistemas Operativos de Tiempo Real (FreeRTOS) en ESP32
├── Practica_3/    # Comunicaciones Wi-Fi y Stack IP en ESP32
├── Practica_4/    # Comunicaciones BLE y Bluetooth Classic
└── Practica_5/    # Gestión, Almacenamiento y Gráficas en Tiempo Real con Python
```

---

##  Resumen de Prácticas

###  Práctica 1: Arquitectura Arduino Nano 33 BLE
* **Objetivo:** Manejo de periféricos integrados en el procesador nRF52840 (ADC, PWM, Timers, GPIOs).
* **Contenido:** Lectura analógica por interrupciones de tiempo, generación de salidas PWM a 5 kHz, interfaz de comandos por UART, comunicación inter-placa por bus **I2C** y muestreo de sensores inerciales (IMU LSM9DS1).

###  Práctica 2: Firmware basado en RTOS con ESP32
* **Objetivo:** Diseño de aplicaciones multitarea empleando **FreeRTOS**.
* **Contenido:** Creación y priorización de tareas concurrentes (LEDs, puerto serie, lectura IMU por I2C/SPI), sincronización y exploración de modos de bajo consumo (*idleHook*).

###  Práctica 3: Comunicaciones Wi-Fi y Stack IP con ESP32
* **Objetivo:** Interconexión IP y protocolos de red en la pila TCP/IP.
* **Contenido:** Sincronización horaria vía **NTP**, sockets TCP/UDP (SocketTest), servidor web integrado, generación de payloads estructurados **SenML/JSON**, subida a servidores **FTP** e integración en la nube Adafruit IO (**HTTP POST** y **MQTT**).

###  Práctica 4: Comunicaciones BLE y Bluetooth Classic
* **Objetivo:** Emisión y recepción de tramas inalámbricas de corto alcance.
* **Contenido:** Configuración de *Advertising* (iBeacon), servidor **GATT** con características de lectura/escritura/notificación para sensores e iluminación (BLE con Nano 33), y canal de chat bidireccional mediante perfil **SPP** (Bluetooth Classic en ESP32).

###  Práctica 5: Captura y Representación de Datos con Python
* **Objetivo:** Adquisición serie desde microcontrolador, procesamiento y visualización de datos inerciales.
* **Contenido:** Lectura con `pyserial`, exportación formateada a ficheros `.txt`/`.csv` compatibles con Excel, y creación de animaciones dinámicas en tiempo real con ventana deslizante (5s) y cálculo de métricas estadísticas ($\mu, \sigma$) usando `Matplotlib` y `NumPy`.

---

##  Requisitos y Entornos de Desarrollo

* **Entorno Microcontroladores:** [Arduino IDE](https://www.arduino.cc/en/software) (con soporte para tarjetas *Mbed OS Nano* y *ESP32*).
* **Entorno Python:** [PyCharm](https://www.jetbrains.com/es-es/pycharm/) con Python 3.8+.
* **Librerías Python Principales:** `pyserial`, `matplotlib`, `numpy`.
