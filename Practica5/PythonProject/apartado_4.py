import matplotlib
matplotlib.use('TkAgg')  # IMPORTANTE: Abre ventana flotante real y evita el bloqueo de PyCharm

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from collections import deque

# --- PARÁMETROS DE CONFIGURACIÓN ---
PUERTO = 'COM4'         # Ajusta tu puerto COM
BAUDIOS = 115200        # Debe coincidir con Serial.begin en Arduino
FREQ_SAMPLING_MS = 50   # Muestreo cada 50 ms
TIEMPO_VENTANA_S = 5    # Ventana acumulativa de 5 segundos

MAX_MUESTRAS = int((TIEMPO_VENTANA_S * 1000) / FREQ_SAMPLING_MS)  # 100 muestras (5s)

# Buffers de ventana móvil
ax_buffer = deque(maxlen=MAX_MUESTRAS)
ay_buffer = deque(maxlen=MAX_MUESTRAS)
az_buffer = deque(maxlen=MAX_MUESTRAS)

# Conexión serie
try:
    ser = serial.Serial(PUERTO, BAUDIOS, timeout=0.01)
    ser.reset_input_buffer()
except Exception as e:
    print(f"Error al conectar con {PUERTO}: {e}")
    exit()

# --- FIGURA Y 3 SUBGRÁFICAS INDEPENDIENTES ---
fig, (ax_x, ax_y, ax_z) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)
fig.canvas.manager.set_window_title('Apartado 4 - Muestreo Acelerómetro en Tiempo Real')

# Eje X
linea_x, = ax_x.plot([], [], 'r-', label='Ax', lw=1.5)
ax_x.set_ylabel('Ax (g)')
ax_x.set_ylim(-2, 2)
ax_x.grid(True, linestyle='--', alpha=0.6)
ax_x.legend(loc='upper right')
txt_x = ax_x.text(0.02, 0.75, '', transform=ax_x.transAxes, fontsize=9,
                  bbox=dict(boxstyle='round', facecolor='whitesmoke', alpha=0.8))

# Eje Y
linea_y, = ax_y.plot([], [], 'g-', label='Ay', lw=1.5)
ax_y.set_ylabel('Ay (g)')
ax_y.set_ylim(-2, 2)
ax_y.grid(True, linestyle='--', alpha=0.6)
ax_y.legend(loc='upper right')
txt_y = ax_y.text(0.02, 0.75, '', transform=ax_y.transAxes, fontsize=9,
                  bbox=dict(boxstyle='round', facecolor='whitesmoke', alpha=0.8))

# Eje Z
linea_z, = ax_z.plot([], [], 'b-', label='Az', lw=1.5)
ax_z.set_xlabel('Muestras (Últimas 100 muestras = 5s)')
ax_z.set_ylabel('Az (g)')
ax_z.set_ylim(-2, 2)
ax_z.grid(True, linestyle='--', alpha=0.6)
ax_z.legend(loc='upper right')
txt_z = ax_z.text(0.02, 0.75, '', transform=ax_z.transAxes, fontsize=9,
                  bbox=dict(boxstyle='round', facecolor='whitesmoke', alpha=0.8))

ax_x.set_xlim(0, MAX_MUESTRAS)

def actualizar(frame):
    # Lectura de todos los paquetes pendientes en la cola del puerto serie
    while ser.in_waiting:
        try:
            linea = ser.readline().decode('utf-8', errors='ignore').strip()
            if linea:
                partes = linea.split(';')
                if len(partes) == 3:
                    vx = float(partes[0])
                    vy = float(partes[1])
                    vz = float(partes[2])

                    ax_buffer.append(vx)
                    ay_buffer.append(vy)
                    az_buffer.append(vz)
        except ValueError:
            pass

    # Actualización automática de gráficas y estadísticas tras recibir muestras
    if len(ax_buffer) > 0:
        eje_t = range(len(ax_buffer))

        linea_x.set_data(eje_t, list(ax_buffer))
        linea_y.set_data(eje_t, list(ay_buffer))
        linea_z.set_data(eje_t, list(az_buffer))

        # Cálculo dinámico de Promedio y Desviación Estándar de la ventana actual (hasta 100 muestras)
        mx, sx = np.mean(ax_buffer), np.std(ax_buffer)
        my, sy = np.mean(ay_buffer), np.std(ay_buffer)
        mz, sz = np.mean(az_buffer), np.std(az_buffer)

        txt_x.set_text(f"Pts: {len(ax_buffer)}/100 | Prom: {mx:+.3f} g | Desv: {sx:.3f}")
        txt_y.set_text(f"Pts: {len(ay_buffer)}/100 | Prom: {my:+.3f} g | Desv: {sy:.3f}")
        txt_z.set_text(f"Pts: {len(az_buffer)}/100 | Prom: {mz:+.3f} g | Desv: {sz:.3f}")

    return linea_x, linea_y, linea_z, txt_x, txt_y, txt_z

# FuncAnimation con refresco a 30 ms para una renderización fluida en tiempo real
ani = animation.FuncAnimation(
    fig, actualizar, interval=30, blit=False, cache_frame_data=False
)

try:
    plt.tight_layout()
    plt.show()
finally:
    if ser.is_open:
        ser.close()