# Práctica 5: Adquisición, Almacenamiento y Procesamiento en Tiempo Real de Datos del Acelerómetro (Python)

Este directorio contiene los scripts en Python desarrollados para la recepción de datos enviados vía UART (puerto serie) desde la tarjeta **Arduino Nano 33 BLE**, su almacenamiento en formato delimitado por `;` (para importación en Excel) y la visualización en tiempo real con filtrado de ventana deslizante y cálculo de estadísticas.

---

##  Requisitos Previos e Instalación

### Requisitos del Entorno

* **Python**: v3.8 o superior.
* **PyCharm** (o cualquier IDE de Python).

### Librerías Necesarias

Instala las dependencias necesarias ejecutando el siguiente comando en la terminal de PyCharm:

```bash
pip install pyserial matplotlib numpy

```

---

##  Estructura del Proyecto

* **`apartado_3_1.py`**: Script de verificación inicial. Crea un archivo con estructura fija delimitada por `;` y saltos de línea para validar el formato compatible con Microsoft Excel.
* **`apartado_3_2.py`**: Conexión al puerto serie y captura de datos en tiempo real. Almacena las lecturas reales del acelerómetro en un fichero local.
* **`apartado_4.py`**: Aplicación principal con representación en tiempo real (`FuncAnimation`). Mantiene un búfer circular de 5 segundos (100 muestras), calcula la **media** ($\mu$) y **desviación estándar** ($\sigma$) dinámicamente y dibuja 3 subgráficas independientes ($A_x, A_y, A_z$).
* **`datos_acelerometro.txt`**: Fichero de salida generado por `apartado_3_2.py` con los datos capturados.
* **`prueba_fija.txt`**: Fichero de prueba generado por `apartado_3_1.py`.

---

##  Descripción de los Módulos

### 1. Apartado 3.1: Prueba de Escritura Fija

Comprueba el acceso a disco y valida que la separación entre columnas sea un punto y coma `;` para la correcta apertura automática en Excel sin truncar decimales.

### 2. Apartado 3.2: Lectura Serie y Exportación a `.txt`

* Abre el puerto serie especificado (ej. `COM3`).
* Lee las cadenas de texto enviadas cada **50 ms** por el Arduino Nano 33 BLE.
* Limpia los saltos de línea y escribe la trama `Ax;Ay;Az\n` haciendo `flush()` inmediato en el disco.

### 3. Apartado 4: Gráfica Dinámica en Tiempo Real (Ventana de 5s)

* **Período de Muestreo:** $50\text{ ms}$ ($20\text{ Hz}$).
* **Tamaño de la Ventana Móvil:** $5\text{ s} \times 20\text{ Hz} = 100\text{ muestras}$.
* **Estructura de Datos:** `collections.deque(maxlen=100)` para mantener automáticamente solo los últimos 5 segundos sin acumular memoria excesiva.
* **Backend de Matplotlib:** Usa `matplotlib.use('TkAgg')` para abrir una ventana flotante interactiva y evitar congelamientos en el visor SciView de PyCharm.

---

##  Instrucciones de Ejecución

1. **Configurar el Puerto Serie:**
Abre el script que deseas ejecutar (`apartado_3_2.py` o `apartado_4.py`) y verifica que la constante `PUERTO` coincida con el puerto asignado a tu Arduino:
```python
PUERTO = 'COM4'  # Cambiar por tu puerto activo (ej. COM4, COM5, /dev/ttyACM0)
BAUDIOS = 115200 # Asegúrate de usar los mismos baudios declarados en Arduino

```


2. **Cerrar otros programas del puerto COM:**
Asegúrate de tener cerrado el **Monitor Serie** de Arduino IDE o **TERMITE** antes de ejecutar Python.
3. **Ejecutar el programa:**
Haz clic derecho sobre `apartado_4.py` en PyCharm y selecciona **Run 'apartado_4'**. Se abrirá la ventana gráfica mostrando las 3 subgráficas en vivo con las estadísticas actualizadas muestra a muestra.